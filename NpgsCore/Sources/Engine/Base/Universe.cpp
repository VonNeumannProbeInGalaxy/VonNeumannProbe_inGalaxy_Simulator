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
#include <vector>

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

Universe::Universe(int Seed)
    : _Seed(Seed), _RandomEngine(Seed), _ThreadPool(ThreadPool::GetInstance(std::thread::hardware_concurrency())), _Dist(0.0f, 1.0f)
{}

Universe::Universe(std::random_device& RandomDevice)
    : _Seed(RandomDevice()), _RandomEngine(RandomDevice()), _ThreadPool(ThreadPool::GetInstance(std::thread::hardware_concurrency())), _Dist(0.0f, 1.0f)
{}

Universe::~Universe() {
    _ThreadPool->Destroy();
}

void Universe::FillStar(int NumStars) {
    int MaxThread = std::thread::hardware_concurrency();

    std::vector<Npgs::Modules::StellarGenerator> Generators;
    std::random_device RandomDevice;
    for (int i = 0; i != MaxThread; ++i) {
        float Seed = RandomDevice();
        // float Seed = i * _Seed;
        Generators.emplace_back(Seed, 0.075);
        NpgsCoreInfo("Stellar generator {} seed has been set to {}.", i, Seed);
    }

    NpgsCoreInfo("Generating basic properties as {} threads...", MaxThread);
    std::vector<std::future<Npgs::Modules::StellarGenerator::BasicProperties>> Futures;
    for (int i = 0; i != NumStars; ++i) {
        Futures.emplace_back(_ThreadPool->Commit([&, i]() -> Npgs::Modules::StellarGenerator::BasicProperties {
            int ThreadId = i % Generators.size();
            return Generators[ThreadId].GenBasicProperties();
        }));
    }

    for (auto& Future : Futures) {
        Future.wait();
    }

    NpgsCoreInfo("Basic properties generation completed.");
    NpgsCoreInfo("Interpolating stellar data as {} threads...", MaxThread);

    std::vector<std::future<Npgs::AstroObject::Star>> StarFutures;
    for (int i = 0; i != NumStars; ++i) {
        StarFutures.emplace_back(_ThreadPool->Commit([&, i]() -> Npgs::AstroObject::Star {
            int ThreadId = i % Generators.size();
            auto Properties = Futures[i].get();
            return Generators[ThreadId].GenerateStar(Properties);
        }));
    }

    for (auto& Future : StarFutures) {
        Future.wait();
    }

    std::vector<AstroObject::Star> Stars;
    for (auto& Future : StarFutures) {
        auto Star = Future.get();
        Stars.emplace_back(Star);
    }

    NpgsCoreInfo("Star detail interpolation completed.");
    NpgsCoreInfo("Building stellar octree...");

    GenerateSlots(0.1f, NumStars, 0.004f);

    NpgsCoreInfo("Stellar octree has been built.");
    NpgsCoreInfo("Sorting...");

    std::vector<glm::vec3> Slots;
    CopyToVector(Slots);
    std::sort(Slots.begin(), Slots.end(), [](const glm::vec3& Point1, const glm::vec3& Point2) -> bool {
        return glm::length(Point1) < glm::length(Point2);
    });

    std::string Name;
    std::ostringstream Stream;
    for (std::size_t i = 0; i != Stars.size(); ++i) {
        Stream.str("");
        Stream << "Star-" << std::setfill('0') << std::setw(8) << std::to_string(i);
        Name = Stream.str();
        Stars[i].SetName(Name);
        Stars[i].SetParentBody(AstroObject::CelestialBody::BaryCenter(Name, Slots[i]));
    }

    Stars[0].SetNormal(glm::vec2(0.0f));

    NpgsCoreInfo("Star generated.");

    _ThreadPool->Terminate();
}

void Universe::GenerateSlots(int SampleLimit, std::size_t NumSamples, float Density) {
    std::vector<glm::vec3> ProcessList;
    std::unordered_map<std::tuple<int, int, int>, glm::vec3, TupleHash> Grid;

    float PointRadius = std::pow((3 / (4 * kPi * Density)), (1.0f / 3.0f));
    float Radius      = std::pow((3 * NumSamples / (4 * kPi * Density)), (1.0f / 3.0f));
    float Diameter    = 2 * Radius;
    float CellSize    = PointRadius / std::sqrt(3.0);

    _StellarOctree = std::make_unique<Octree>(glm::vec3(0.0), Radius);

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
            float Angle1   = _Dist.Generate(_RandomEngine) * 2 * kPi;
            float Angle2   = _Dist.Generate(_RandomEngine) * 2 * kPi;
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
    float Radius     = std::pow((3 * NumSamples / (4 * kPi * Density)), (1.0f / 3.0f));
    float LeafSize   = std::pow((1.0f / Density), (1.0f / 3.0f));
    int   Power      = static_cast<int>(std::ceil(std::log2(Radius / LeafSize)));
    float LeafRadius = LeafSize * 0.5f;
    float RootRadius = LeafSize * std::pow(2, Power);

    _StellarOctree = std::make_unique<Octree>(glm::vec3(0.0), RootRadius);
    _StellarOctree->BuildEmptyTree(LeafRadius);

    _StellarOctree->Traverse([Radius](OctreeNode& Node) -> void {
        if (Node.IsLeafNode() && glm::length(Node.GetCenter()) > Radius) {
            Node.SetValidation(false);
        }
    });

    std::size_t ValidLeafCount = _StellarOctree->GetCapacity();
    std::vector<OctreeNode*> LeafNodes;

    auto CollectLeafNodes = [&LeafNodes](OctreeNode& Node) -> void {
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

    _StellarOctree->Traverse([&Dist, LeafRadius, DistMin, this](OctreeNode& Node) -> void {
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

    _StellarOctree->Find(glm::vec3(LeafRadius), [](OctreeNode& Node) -> bool {
        if (!Node.IsLeafNode()) {
            return false;
        }

        Node.RemoveStorage();
        Node.AddPoint(glm::vec3(0.0f));
        return true;
    });
}

template<typename Ty>
void Universe::CopyToVector(std::vector<Ty>& Stars) const {
    _StellarOctree->Traverse([&Stars](const OctreeNode& Node) -> void {
        if (Node.IsLeafNode() && Node.GetValidation()) {
            for (const auto& Point : Node.GetPoints()) {
                Stars.emplace_back(Point);
            }
        }
    });
}

_NPGS_END
