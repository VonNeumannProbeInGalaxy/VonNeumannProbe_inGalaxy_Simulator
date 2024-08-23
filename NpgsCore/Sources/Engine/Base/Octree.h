#pragma once

#include <array>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

class OctreeNode {
public:
    OctreeNode(const glm::vec3& Center, float Radius);

    bool Contains(const glm::vec3& Point) const;
    int GetOctant(const glm::vec3& Point) const;

    const glm::vec3& GetCenter() const;
    float GetRadius() const;
    std::unique_ptr<OctreeNode>& GetChild(int Index);

private:
    glm::vec3 _Center;
    float     _Radius;
    std::array<std::unique_ptr<OctreeNode>, 8> _Children;
};

class NPGS_API Octree {
public:
    Octree(const glm::vec3& Center, float Radius, int MaxDepth = 10);

    void Insert(const glm::vec3& Point);
    void Query(const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const;

private:
    void InsertImpl(OctreeNode* Node, const glm::vec3& Point, int Depth);
    void QueryImpl(OctreeNode* Node, const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const;

private:
    std::unique_ptr<OctreeNode> _Root;
    int                         _MaxDepth;
};

_NPGS_END

#include "Octree.inl"
