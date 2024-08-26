#include "Universe.h"

#include <algorithm>
#include <tuple>
#include <type_traits>
#include <unordered_map>

#include "Engine/Core/Constants.h"
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

Universe::Universe(int Seed) : _RandomEngine(Seed), _Dist(0.0f, 1.0f) {}

Universe::Universe(std::random_device& RandomDevice) : _RandomEngine(RandomDevice()), _Dist(0.0f, 1.0f) {}

void Universe::AddStar(const std::string& Name, AstroObject::CelestialBody::BaryCenter& StarSys, const AstroObject::Star& Star) {}

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

    std::println("{}", _StellarOctree->GetCapacity());

    _StellarOctree->Traverse([Radius](OctreeNode& Node) -> void {
        if (Node.IsLeafNode() && glm::length(Node.GetCenter()) > Radius) {
            Node.SetValidation(false);
        }
    });

    std::println("{}", _StellarOctree->GetCapacity());

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

    std::println("{}", _StellarOctree->GetCapacity());

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
}

_NPGS_END
