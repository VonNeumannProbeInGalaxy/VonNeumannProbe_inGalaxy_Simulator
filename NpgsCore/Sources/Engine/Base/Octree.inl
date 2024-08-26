#pragma once

#include "Octree.h"

#include <cmath>
#include <utility>

_NPGS_BEGIN

inline bool OctreeNode::Contains(const glm::vec3& Point) const {
    return (Point.x >= _Center.x - _Radius && Point.x <= _Center.x + _Radius &&
            Point.y >= _Center.y - _Radius && Point.y <= _Center.y + _Radius &&
            Point.z >= _Center.z - _Radius && Point.z <= _Center.z + _Radius);
}

inline int OctreeNode::CalcOctant(const glm::vec3& Point) const {
    int Octant = 0;

    if (Point.x >= _Center.x) Octant |= 4;
    if (Point.y >= _Center.y) Octant |= 2;
    if (Point.z >= _Center.z) Octant |= 1;

    return Octant;
}

inline bool OctreeNode::IntersectSphere(const glm::vec3& Point, float Radius) const {
    glm::vec3 MinBound = _Center - glm::vec3(_Radius);
    glm::vec3 MaxBound = _Center + glm::vec3(_Radius);

    glm::vec3 ClosestPoint = glm::clamp(Point, MinBound, MaxBound);
    float Distance = glm::distance(Point, ClosestPoint);

    return Distance <= Radius;
}

inline const bool OctreeNode::GetValidation() const {
    return _bIsValid;
}

inline const glm::vec3& OctreeNode::GetCenter() const {
    return _Center;
}

inline float OctreeNode::GetRadius() const {
    return _Radius;
}

inline std::unique_ptr<OctreeNode>& OctreeNode::GetNextMutable(int Index) {
    return _Next[Index];
}

inline const std::unique_ptr<OctreeNode>& OctreeNode::GetNext(int Index) const {
    return _Next[Index];
}

inline void OctreeNode::AddPoint(const glm::vec3& Point) {
    _Points.emplace_back(Point);
}

inline std::vector<glm::vec3>& OctreeNode::GetPointsMutable() {
    return _Points;
}

inline const std::vector<glm::vec3>& OctreeNode::GetPoints() const {
    return _Points;
}

inline void OctreeNode::SetValidation(bool bValidation) {
    _bIsValid = bValidation;
}

inline bool OctreeNode::IsLeafNode() const {
    for (const auto& Next : _Next) {
        if (Next != nullptr) {
            return false;
        }
    }

    return true;
}

inline void Octree::BuildEmptyTree(float LeafRadius) {
    int Depth = static_cast<int>(std::ceil(std::log2(_Root->GetRadius() / LeafRadius)));
    BuildEmptyTreeImpl(_Root.get(), LeafRadius, Depth);
}

inline void Octree::Insert(const glm::vec3& Point) {
    InsertImpl(_Root.get(), Point, 0);
}

inline void Octree::Delete(const glm::vec3& Point) {
    DeleteImpl(_Root.get(), Point);
}

inline void Octree::Query(const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const {
    QueryImpl(_Root.get(), Point, Radius, Results);
}

template <typename Func>
inline void Octree::Traverse(Func&& Pred) const {
    TraverseImpl(_Root.get(), std::forward<Func>(Pred));
}

inline std::size_t Octree::GetCapacity() const {
    return GetCapacityImpl(_Root.get());
}

inline std::size_t Octree::GetSize() const {
    return GetSizeImpl(_Root.get());
}

_NPGS_END
