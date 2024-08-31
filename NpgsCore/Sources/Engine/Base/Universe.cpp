#include "Universe.h"

#include <cstdint>
#include <algorithm>
#include <future>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>

#define ENABLE_LOGGER
// #define OUTPUT_DATA
#include "Engine/Core/Constants.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN

Universe::Universe(unsigned Seed, std::size_t NumStars, std::size_t NumExtraGiants, std::size_t NumExtraMassiveStars, std::size_t NumExtraNeutronStars, std::size_t NumExtraBlackHoles, std::size_t NumExtraMergeStars, float UniverseAge) :
    _RandomEngine(Seed), _ThreadPool(ThreadPool::GetInstance()), _CommonGenerator(0.0f, 1.0f), _SeedGenerator(0.0f, static_cast<float>(std::numeric_limits<unsigned>::max())),
    _NumStars(NumStars), _NumExtraGiants(NumExtraGiants), _NumExtraMassiveStars(NumExtraMassiveStars), _NumExtraNeutronStars(NumExtraNeutronStars), _NumExtraBlackHoles(NumExtraBlackHoles), _NumExtraMergeStars(NumExtraMergeStars), _UniverseAge(UniverseAge)
{
    std::seed_seq SeedSeq{
        static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
        static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
        static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
        static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine))
    };
    _RandomEngine.seed(SeedSeq);
}

Universe::~Universe() {
    _ThreadPool->Destroy();
}

const std::vector<AstroObject::Star>& Universe::FillUniverse() {
    int MaxThread = _ThreadPool->GetMaxThreadCount();

    NpgsCoreInfo("Initializating and generating basic properties...");
    std::vector<std::future<Modules::StellarGenerator::BasicProperties>> Futures;
    std::vector<Modules::StellarGenerator> Generators;
    std::vector<Modules::StellarGenerator::BasicProperties> BasicProperties;

    auto CreateGenerators =
        [&, this](Modules::StellarGenerator::GenOption Option = Modules::StellarGenerator::GenOption::kNormal,
            float MassLowerLimit =  0.1f, float MassUpperLimit = 300.0f,   Modules::StellarGenerator::GenDistribution MassDistribution = Modules::StellarGenerator::GenDistribution::kFromPdf,
            float AgeLowerLimit  =  0.0f, float AgeUpperLimit  = 1.26e10f, Modules::StellarGenerator::GenDistribution AgeDistribution  = Modules::StellarGenerator::GenDistribution::kFromPdf,
            float FeHLowerLimit  = -4.0f, float FeHUpperLimit  = 0.5f,     Modules::StellarGenerator::GenDistribution FeHDistribution  = Modules::StellarGenerator::GenDistribution::kFromPdf) -> void {
        for (int i = 0; i != MaxThread; ++i) {
            std::seed_seq SeedSeq{
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine)),
                static_cast<unsigned>(_SeedGenerator.Generate(_RandomEngine))
            };
            Generators.emplace_back(SeedSeq, Option, _UniverseAge, MassLowerLimit, MassUpperLimit, MassDistribution, AgeLowerLimit, AgeUpperLimit, AgeDistribution, FeHLowerLimit, FeHUpperLimit, FeHDistribution);
        }
    };

    // auto GenerateBasicProperties = [&, this](std::size_t NumStars) -> void {
    //     for (std::size_t i = 0; i != NumStars; ++i) {
    //         Futures.emplace_back(_ThreadPool->Commit([&, i]() -> Npgs::Modules::StellarGenerator::BasicProperties {
    //             std::size_t ThreadId = i % Generators.size();
    //             return Generators[ThreadId].GenBasicProperties();
    //         }));
    //     }
    // };

    auto GenerateBasicProperties = [&, this](std::size_t NumStars) -> void {
        for (std::size_t i = 0; i != NumStars; ++i) {
            std::size_t ThreadId = i % Generators.size();
            BasicProperties.emplace_back(Generators[ThreadId].GenBasicProperties());
        }
    };

    if (_NumExtraGiants != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kGiant, 1.0, 30.0);
        GenerateBasicProperties(_NumExtraGiants);
    }

    if (_NumExtraMassiveStars != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kNormal, 20.0f, 300.0f, Modules::StellarGenerator::GenDistribution::kUniform, 0.0f, 3.5e6f, Modules::StellarGenerator::GenDistribution::kUniform);
        GenerateBasicProperties(_NumExtraMassiveStars);
    }

    if (_NumExtraNeutronStars != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kDeathStar, 10.0f, 20.0f, Modules::StellarGenerator::GenDistribution::kUniform, 1e7f, 1e8f, Modules::StellarGenerator::GenDistribution::kUniformByExponent);
        GenerateBasicProperties(_NumExtraNeutronStars);
    }

    if (_NumExtraBlackHoles != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kNormal, 35.0f, 300.0f, Modules::StellarGenerator::GenDistribution::kUniform, 1e7f, 1.26e10f, Modules::StellarGenerator::GenDistribution::kFromPdf, -2.0, 0.5);
        GenerateBasicProperties(_NumExtraBlackHoles);
    }

    if (_NumExtraMergeStars != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kMergeStar, 0.0f, 0.0f, Modules::StellarGenerator::GenDistribution::kUniform, 1e6f, 1e8f, Modules::StellarGenerator::GenDistribution::kUniformByExponent);
        GenerateBasicProperties(_NumExtraMergeStars);
    }

    std::size_t NumCommonStars = _NumStars - _NumExtraGiants - _NumExtraMassiveStars - _NumExtraNeutronStars - _NumExtraBlackHoles - _NumExtraMergeStars;

    Generators.clear();
    CreateGenerators(Modules::StellarGenerator::GenOption::kNormal, 0.075);
    // CreateGenerators(Modules::StellarGenerator::GenOption::kNormal, 8.0, 300.0);
    GenerateBasicProperties(NumCommonStars);

    for (auto& Future : Futures) {
        Future.wait();
    }

    NpgsCoreInfo("Basic properties generation completed.");
    NpgsCoreInfo("Interpolating stellar data as {} physical cores...", MaxThread);

    std::vector<std::future<Npgs::AstroObject::Star>> StarFutures;
    for (std::size_t i = 0; i != _NumStars; ++i) {
        StarFutures.emplace_back(_ThreadPool->Commit([&, i]() -> Npgs::AstroObject::Star {
            std::size_t ThreadId = i % Generators.size();
            auto& Properties = BasicProperties[i]; 
            // auto Properties = Futures[i].get();
            return Generators[ThreadId].GenerateStar(Properties);
        }));
    }

    for (auto& Future : StarFutures) {
        Future.wait();
    }

    for (auto& Future : StarFutures) {
        auto Star = Future.get();
        _Stars.emplace_back(Star);
    }

#ifdef OUTPUT_DATA
    NpgsCoreInfo("Outputing data...");
    return _Stars;
#endif // OUTPUT_DATA

    NpgsCoreInfo("Star detail interpolation completed.");
    NpgsCoreInfo("Building stellar octree...");
    GenerateSlots(0.1f, _NumStars, 0.004f);
    NpgsCoreInfo("Stellar octree has been built.");

    NpgsCoreInfo("Linking positions in octree to stars...");
    _StellarSystems.reserve(_NumStars);
    std::shuffle(_Stars.begin(), _Stars.end(), _RandomEngine);
    std::vector<glm::vec3> Slots;
    OctreeLinkToStars(_Stars, Slots);

    NpgsCoreInfo("Sorting...");
    std::sort(Slots.begin(), Slots.end(), [](const glm::vec3& Point1, const glm::vec3& Point2) {
        return glm::length(Point1) < glm::length(Point2);
    });

    NpgsCoreInfo("Assigning name...");
    std::string Name;
    std::ostringstream Stream;
    for (auto& System : _StellarSystems) {
        glm::vec3 Position = System.GetBaryPosition();
        auto it = std::lower_bound(Slots.begin(), Slots.end(), Position, [](const glm::vec3& Point1, const glm::vec3& Point2) -> bool {
            return glm::length(Point1) < glm::length(Point2);
        });
        std::ptrdiff_t Offset = it - Slots.begin();
        Stream << "S-" << std::setfill('0') << std::setw(8) << std::to_string(Offset);
        Name = Stream.str();
        System.SetBaryName(Name).SetBaryDistanceRank(Offset).SetBaryPosition(Position);
        Stream.str("");
        Stream.clear();

        auto& Stars = System.StarData();
        if (Stars.size() != 1) {
            std::sort(Stars.begin(), Stars.end(), [](const AstroObject::Star& Star1, const AstroObject::Star& Star2) -> bool {
                return Star1.GetMass() > Star2.GetMass();
            });

            char Rank = 'A';
            for (auto& Star : Stars) {
                Star.SetName(Star.GetName() + " " + Rank);
            }
        }
    }

    NpgsCoreInfo("Reset home star...");
    NodeType* HomeNode = _Octree->Find(glm::vec3(0.0f), [](const NodeType& Node) -> bool {
        if (Node.IsLeafNode()) {
            auto& Points = Node.GetPoints();
            return std::find(Points.begin(), Points.end(), glm::vec3(0.0f)) != Points.end();
        } else {
            return false;
        }
    });

    auto* HomeStar = HomeNode->GetLink([](StellarSystem* System) -> bool {
        return System->GetBaryPosition() == glm::vec3(0.0f);
    });
    HomeNode->RemoveStorage();
    HomeNode->AddPoint(glm::vec3(0.0f));
    HomeStar->SetBaryNormal(glm::vec2(0.0f));

    _StellarSystems;

    NpgsCoreInfo("Star generation completed.");
    _ThreadPool->Terminate();

    return _Stars;
}

const void Universe::ReplaceStar(std::size_t DistanceRank, const AstroObject::Star& StarData) {
    for (auto& System : _StellarSystems) {
        if (DistanceRank == System.GetBaryDistanceRank()) {
            auto& Star = System.StarData();
            if (Star.size() != 1) {
                return;
            }

            Star.clear();
            Star.emplace_back(StarData);
        }
    }
}

void Universe::GenerateSlots(float DistMin, std::size_t NumSamples, float Density) {
    float Radius     = std::pow((3.0f * NumSamples / (4 * kPi * Density)), (1.0f / 3.0f));
    float LeafSize   = std::pow((1.0f / Density), (1.0f / 3.0f));
    int   Exponent   = static_cast<int>(std::ceil(std::log2(Radius / LeafSize)));
    float LeafRadius = LeafSize * 0.5f;
    float RootRadius = LeafSize * static_cast<float>(std::pow(2, Exponent));

    _Octree = std::make_unique<Octree<StellarSystem>>(glm::vec3(0.0), RootRadius);
    _Octree->BuildEmptyTree(LeafRadius);

    _Octree->Traverse([Radius](NodeType& Node) -> void {
        if (Node.IsLeafNode() && glm::length(Node.GetCenter()) > Radius) {
            Node.SetValidation(false);
        }
    });

    std::size_t ValidLeafCount = _Octree->GetCapacity();
    std::vector<NodeType*> LeafNodes;

    auto CollectLeafNodes = [&LeafNodes](NodeType& Node) -> void {
        if (Node.IsLeafNode()) {
            LeafNodes.emplace_back(&Node);
        }
    };

    while (ValidLeafCount != NumSamples) {
        LeafNodes.clear();
        _Octree->Traverse(CollectLeafNodes);
        std::shuffle(LeafNodes.begin(), LeafNodes.end(), _RandomEngine);

        if (ValidLeafCount < NumSamples) {
            for (auto* Node : LeafNodes) {
                glm::vec3 Center = Node->GetCenter();
                float Distance = glm::length(Center);
                if (!Node->GetValidation() && Distance >= Radius && Distance <= Radius + LeafRadius) {
                    Node->SetValidation(true);
                    if (++ValidLeafCount == NumSamples) {
                        break;
                    }
                }
            }
        } else {
            for (auto* Node : LeafNodes) {
                glm::vec3 Center = Node->GetCenter();
                float Distance = glm::length(Center);
                if (Node->GetValidation() && Distance >= Radius - LeafRadius && Distance <= Radius) {
                    Node->SetValidation(false);
                    if (--ValidLeafCount == NumSamples) {
                        break;
                    }
                }
            }
        }
    }

    UniformRealDistribution<float> Dist(-LeafRadius, LeafRadius - DistMin);

    _Octree->Traverse([&Dist, LeafRadius, DistMin, this](NodeType& Node) -> void {
        if (Node.IsLeafNode() && Node.GetValidation()) {
            glm::vec3 Center(Node.GetCenter());
            glm::vec3 StellarSlot(
                Center.x + Dist.Generate(_RandomEngine),
                Center.y + Dist.Generate(_RandomEngine),
                Center.z + Dist.Generate(_RandomEngine)
            );
            Node.AddPoint(StellarSlot);
        }
    });

    NodeType* Node = _Octree->Find(glm::vec3(LeafRadius), [](const NodeType& Node) -> bool {
        return (Node.IsLeafNode());
    });

    Node->RemoveStorage();
    Node->AddPoint(glm::vec3(0.0f));
}

void Universe::OctreeLinkToStars(std::vector<AstroObject::Star>& Stars, std::vector<glm::vec3>& Slots) {
    std::size_t Index = 0;
    _Octree->Traverse([&](NodeType& Node) -> void {
        if (Node.IsLeafNode() && Node.GetValidation()) {
            for (const auto& Point : Node.GetPoints()) {
                float Theta = _CommonGenerator.Generate(_RandomEngine) * 2.0f * kPi;
                float Phi   = _CommonGenerator.Generate(_RandomEngine) * kPi;
                _StellarSystems.emplace_back(StellarSystem({ Point, { Theta, Phi }, 0, "" }, { Stars[Index] }));
                Node.AddLink(&_StellarSystems[Index]);
                Slots.emplace_back(Point);
                ++Index;
            }
        }
    });
}

_NPGS_END
