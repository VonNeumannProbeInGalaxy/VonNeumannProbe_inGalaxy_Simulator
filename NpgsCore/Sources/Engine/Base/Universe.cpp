#include "Universe.h"

#include <algorithm>
#include <future>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>

#define ENABLE_LOGGER
#include "Engine/Core/Constants.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN

struct TupleHash {
    template <typename Ty>
    std::size_t operator()(const Ty& Tuple) const {
        auto Hash0 = std::hash<int>{}(std::get<0>(Tuple));
        auto Hash1 = std::hash<int>{}(std::get<1>(Tuple));
        auto Hash2 = std::hash<int>{}(std::get<2>(Tuple));
        return Hash0 ^ (Hash1 << 1) ^ (Hash2 << 2);
    }
};

Universe::Universe(int Seed, std::size_t NumStars, std::size_t NumExtraLbvs, std::size_t NumExtraNeutronStars, std::size_t NumExtraBlackHoles, std::size_t NumExtraMergeStars) :
    _Seed(Seed), _RandomEngine(Seed), _ThreadPool(ThreadPool::GetInstance(std::thread::hardware_concurrency())), _Dist(0.0f, 1.0f),
    _NumStars(NumStars), _NumExtraLbvs(NumExtraLbvs), _NumExtraNeutronStars(NumExtraNeutronStars), _NumExtraBlackHoles(NumExtraBlackHoles), _NumExtraMergeStars(NumExtraMergeStars)
{}

Universe::~Universe() {
    _ThreadPool->Destroy();
}

const std::vector<AstroObject::Star>& Universe::FillUniverse() {
    int MaxThread = std::thread::hardware_concurrency();

    NpgsCoreInfo("Generating basic properties as {} threads...", MaxThread);
    std::vector<std::future<Npgs::Modules::StellarGenerator::BasicProperties>> Futures;
    std::vector<Npgs::Modules::StellarGenerator> Generators;

    auto CreateGenerators =
        [&, this](double MassLowerLimit =  0.1, double MassUpperLimit = 300.0,   Modules::StellarGenerator::GenDistribution MassDistribution = Modules::StellarGenerator::GenDistribution::kFromPdf,
                  double AgeLowerLimit  =  0.0, double AgeUpperLimit  = 1.26e10, Modules::StellarGenerator::GenDistribution AgeDistribution  = Modules::StellarGenerator::GenDistribution::kFromPdf,
                  double FeHLowerLimit  = -4.0, double FeHUpperLimit  = 0.5,     Modules::StellarGenerator::GenDistribution FeHDistribution  = Modules::StellarGenerator::GenDistribution::kFromPdf) -> void {
        for (int i = 0; i != MaxThread; ++i) {
            float Seed = static_cast<float>(i * _Seed);
            Generators.emplace_back(Seed, MassLowerLimit, MassUpperLimit, MassDistribution, AgeLowerLimit, AgeUpperLimit, AgeDistribution, FeHLowerLimit, FeHUpperLimit, FeHDistribution);
        }
    };

    auto GeneratoBasicProperties = [&, this](std::size_t NumStars) -> void {
        for (std::size_t i = 0; i != NumStars; ++i) {
            Futures.emplace_back(_ThreadPool->Commit([&, i]() -> Npgs::Modules::StellarGenerator::BasicProperties {
                int ThreadId = i % Generators.size();
                return Generators[ThreadId].GenBasicProperties();
            }));
        }
    };

    if (_NumExtraLbvs != 0) {
        Generators.clear();
        CreateGenerators(8.0, 300.0, Modules::StellarGenerator::GenDistribution::kUniform, 0.0, 1e7, Modules::StellarGenerator::GenDistribution::kUniform);
        GeneratoBasicProperties(_NumExtraLbvs);
    }

    if (_NumExtraNeutronStars != 0) {
        Generators.clear();
        CreateGenerators(10.0, 20.0, Modules::StellarGenerator::GenDistribution::kUniform, 1e7, 1e8, Modules::StellarGenerator::GenDistribution::kUniformByExponent);
        GeneratoBasicProperties(_NumExtraNeutronStars);
    }

    if (_NumExtraBlackHoles != 0) {
        Generators.clear();
        CreateGenerators(35.0, 300.0, Modules::StellarGenerator::GenDistribution::kUniform, 0.0, 1.26e10, Modules::StellarGenerator::GenDistribution::kFromPdf, -2.0, 0.5);
        GeneratoBasicProperties(_NumExtraBlackHoles);
    }

    if (_NumExtraMergeStars != 0) {
        Generators.clear();
        CreateGenerators(0.0, 0.0, Modules::StellarGenerator::GenDistribution::kUniform, 1e6, 1e8, Modules::StellarGenerator::GenDistribution::kUniformByExponent);
        GeneratoBasicProperties(_NumExtraMergeStars);
    }

    std::size_t NumCommonStars = _NumStars - _NumExtraLbvs - _NumExtraNeutronStars - _NumExtraBlackHoles - _NumExtraMergeStars;

    Generators.clear();
    CreateGenerators(0.075);
    GeneratoBasicProperties(NumCommonStars);

    for (auto& Future : Futures) {
        Future.wait();
    }

    NpgsCoreInfo("Basic properties generation completed.");
    NpgsCoreInfo("Interpolating stellar data as {} threads...", MaxThread);

    std::vector<std::future<Npgs::AstroObject::Star>> StarFutures;
    for (std::size_t i = 0; i != _NumStars; ++i) {
        StarFutures.emplace_back(_ThreadPool->Commit([&, i]() -> Npgs::AstroObject::Star {
            int ThreadId = i % Generators.size();
            auto Properties = Futures[i].get();
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

    NpgsCoreInfo("Star detail interpolation completed.");
    NpgsCoreInfo("Building stellar octree...");
    GenerateSlots(0.1f, _NumStars, 0.004f);
    NpgsCoreInfo("Stellar octree has been built.");

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
    for (auto& Star : _Stars) {
        glm::vec3 Position = Star.GetParentBody().Position;
        auto it = std::lower_bound(Slots.begin(), Slots.end(), Position, [](const glm::vec3& Point1, const glm::vec3& Point2) -> bool {
            return glm::length(Point1) < glm::length(Point2);
        });
        std::ptrdiff_t Offset = it - Slots.begin();
        Stream << "S-" << std::setfill('0') << std::setw(8) << std::to_string(Offset);
        Name = Stream.str();
        Star.SetName(Name);
        Star.SetParentBody(AstroObject::CelestialBody::BaryCenter(Name, Position));
        Stream.str("");
        Stream.clear();
    }

    NpgsCoreInfo("Reset home star...");
    NodeType* Node = _StellarOctree->Find(glm::vec3(0.0f), [](const NodeType& Node) -> bool {
        if (Node.IsLeafNode()) {
            auto& Points = Node.GetPoints();
            return std::find(Points.begin(), Points.end(), glm::vec3(0.0f)) != Points.end();
        } else {
            return false;
        }
    });

    auto* HomeStar = Node->GetLink([](AstroObject::Star* Star) -> bool {
        return Star->GetParentBody().Position == glm::vec3(0.0f);
    });

    HomeStar->SetNormal(glm::vec2(0.0f));

    glm::vec3 FrontStarPos = _Stars.front().GetParentBody().Position;
    Node = _StellarOctree->Find(FrontStarPos, [&FrontStarPos](const NodeType& Node) -> bool {
        if (Node.IsLeafNode()) {
            auto& Points = Node.GetPoints();
            return std::find(Points.begin(), Points.end(), FrontStarPos) != Points.end();
        } else {
            return false;
        }
    });

    auto* FrontStar = Node->GetLink([FrontStarPos](AstroObject::Star* Star) -> bool {
        return Star->GetParentBody().Position == FrontStarPos;
    });

    std::swap(*HomeStar, *FrontStar);

    NpgsCoreInfo("Star generation completed.");
    _ThreadPool->Terminate();

    return _Stars;
}

void Universe::GenerateSlots(int SampleLimit, std::size_t NumSamples, float Density) {
    std::vector<glm::vec3> ProcessList;
    std::unordered_map<std::tuple<int, int, int>, glm::vec3, TupleHash> Grid;

    float PointRadius = static_cast<float>(std::pow((3 / (4 * kPi * Density)), (1.0f / 3.0f)));
    float Radius      = static_cast<float>(std::pow((3 * NumSamples / (4 * kPi * Density)), (1.0f / 3.0f)));
    float Diameter    = 2 * Radius;
    float CellSize    = PointRadius / std::sqrt(3.0f);

    _StellarOctree = std::make_unique<Octree<AstroObject::Star>>(glm::vec3(0.0), Radius);

    std::size_t TotalSamples = 0;

    auto AddSample = [&](const glm::vec3& Sample) -> void {
        _StellarOctree->Insert(Sample);
        ProcessList.emplace_back(Sample);
        int x = static_cast<int>((Sample.x + Radius) / CellSize);
        int y = static_cast<int>((Sample.y + Radius) / CellSize);
        int z = static_cast<int>((Sample.z + Radius) / CellSize);
        Grid[std::make_tuple(x, y, z)] = Sample;
        ++TotalSamples;
    };

    glm::vec3 InitialSample(0.0);
    AddSample(InitialSample);

    while (!ProcessList.empty() && TotalSamples < NumSamples) {
        int Index = static_cast<int>(_Dist.Generate(_RandomEngine) * ProcessList.size());
        glm::vec3 CurrentSample = ProcessList[Index];
        bool bFound = false;

        for (int i = 0; i != SampleLimit; ++i) {
            float Angle1   = _Dist.Generate(_RandomEngine) * 2.0f * static_cast<float>(kPi);
            float Angle2   = _Dist.Generate(_RandomEngine) * 2.0f * static_cast<float>(kPi);
            float Distance = _Dist.Generate(_RandomEngine) * Radius; // _Dist.Generate(_RandomEngine) * (2 * PointRadius - 0.1f) + 0.1f;
            glm::vec3 NewSample = CurrentSample + glm::vec3(std::cos(Angle1) * std::cos(Angle2), std::sin(Angle1) * std::cos(Angle2), std::sin(Angle2)) * Distance;

            if (glm::length(NewSample) <= Radius) {
                std::vector<glm::vec3> Results;
                _StellarOctree->Query(NewSample, 0.1f, Results);

                bool bValid = Results.empty();

                if (bValid) {
                    AddSample(NewSample);
                    bFound = true;
                    break;
                }
            }
        }

        if (!bFound) {
            ProcessList.erase(ProcessList.begin() + Index);
        }
    }
}

void Universe::GenerateSlots(float DistMin, std::size_t NumSamples, float Density) {
    float Radius     = static_cast<float>(std::pow((3 * NumSamples / (4 * kPi * Density)), (1.0f / 3.0f)));
    float LeafSize   = static_cast<float>(std::pow((1.0f / Density), (1.0f / 3.0f)));
    int   Power      = static_cast<int>(std::ceil(std::log2(Radius / LeafSize)));
    float LeafRadius = LeafSize * 0.5f;
    float RootRadius = LeafSize * static_cast<float>(std::pow(2, Power));

    _StellarOctree = std::make_unique<Octree<AstroObject::Star>>(glm::vec3(0.0), RootRadius);
    _StellarOctree->BuildEmptyTree(LeafRadius);

    _StellarOctree->Traverse([Radius](NodeType& Node) -> void {
        if (Node.IsLeafNode() && glm::length(Node.GetCenter()) > Radius) {
            Node.SetValidation(false);
        }
    });

    std::size_t ValidLeafCount = _StellarOctree->GetCapacity();
    std::vector<NodeType*> LeafNodes;

    auto CollectLeafNodes = [&LeafNodes](NodeType& Node) -> void {
        if (Node.IsLeafNode()) {
            LeafNodes.emplace_back(&Node);
        }
    };

    while (ValidLeafCount != NumSamples) {
        LeafNodes.clear();
        _StellarOctree->Traverse(CollectLeafNodes);
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

    _StellarOctree->Traverse([&Dist, LeafRadius, DistMin, this](NodeType& Node) -> void {
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

    NodeType* Node = _StellarOctree->Find(glm::vec3(LeafRadius), [](const NodeType& Node) -> bool {
        return (Node.IsLeafNode());
    });

    Node->RemoveStorage();
    Node->AddPoint(glm::vec3(0.0f));
}

void Universe::OctreeLinkToStars(std::vector<AstroObject::Star>& Stars, std::vector<glm::vec3>& Slots) const {
    std::size_t Index = 0;
    _StellarOctree->Traverse([&](NodeType& Node) -> void {
        if (Node.IsLeafNode() && Node.GetValidation()) {
            for (const auto& Point : Node.GetPoints()) {
                Stars[Index].SetParentBody(AstroObject::CelestialBody::BaryCenter{ "", Point });
                Node.AddLink(&Stars[Index]);
                Slots.emplace_back(Point);
                ++Index;
            }
        }
    });
}

_NPGS_END
