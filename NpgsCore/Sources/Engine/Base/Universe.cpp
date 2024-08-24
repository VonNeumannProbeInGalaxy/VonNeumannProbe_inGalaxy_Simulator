#include "Universe.h"

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

void Universe::GenerateSlots(float PointRadius, int SampleLimit, int NumSample, float Density) {
    std::vector<glm::vec3> ProcessList;
    std::unordered_map<std::tuple<int, int, int>, glm::vec3, TupleHash> Grid;

    float Radius   = std::pow((3 * NumSample / (4 * kPi * Density)), (1.0 / 3.0));
    float Diameter = 2 * Radius;
    float CellSize = PointRadius / std::sqrt(3.0);

    _StarOctree = std::make_unique<Octree>(glm::vec3(0.0), Radius);

    auto AddSample = [&](const glm::vec3& Sample) -> void {
        _StarOctree->Insert(Sample);
        ProcessList.emplace_back(Sample);
        int x = static_cast<int>((Sample.x + Radius) / CellSize);
        int y = static_cast<int>((Sample.y + Radius) / CellSize);
        int z = static_cast<int>((Sample.z + Radius) / CellSize);
        Grid[std::make_tuple(x, y, z)] = Sample;
    };

    glm::vec3 InitialSample(0.0);
    AddSample(InitialSample);

    while (!ProcessList.empty() && _StarOctree->GetSize() < NumSample) {
        int Index = static_cast<int>(_Dist.Generate(_RandomEngine) * ProcessList.size());
        glm::vec3 CurrentSample = ProcessList[Index];
        bool bFound = false;

        for (int i = 0; i != SampleLimit; ++i) {
            float Angle1   = _Dist.Generate(_RandomEngine) * 2 * kPi;
            float Angle2   = _Dist.Generate(_RandomEngine) * 2 * kPi;
            float Distance = _Dist.Generate(_RandomEngine) * Radius;
            glm::vec3 NewSample = CurrentSample + glm::vec3(std::cos(Angle1) * std::cos(Angle2), std::sin(Angle1) * std::cos(Angle2), std::sin(Angle2)) * Distance;

            if (glm::length(NewSample) <= Radius) {
                std::vector<glm::vec3> Results;
                _StarOctree->Query(NewSample, PointRadius, Results);

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

_NPGS_END
