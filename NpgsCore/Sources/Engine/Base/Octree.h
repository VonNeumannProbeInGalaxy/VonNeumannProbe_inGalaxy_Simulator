#pragma once

#include <array>
#include <functional>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

class NPGS_API OctreeNode {
public:
    OctreeNode(const glm::vec3& Center, float Radius, OctreeNode* Prev);

    bool Contains(const glm::vec3& Point) const;
    int GetOctant(const glm::vec3& Point) const;
    bool IntersectsSphere(const glm::vec3& Point, float Radius) const;

    const glm::vec3& GetCenter() const;
    float GetRadius() const;
    std::unique_ptr<OctreeNode>& GetNextMutable(int Index);
    const std::unique_ptr<OctreeNode>& GetNext(int Index) const;

    void AddPoint(const glm::vec3& Point);
    const std::vector<glm::vec3>& GetPoints() const;

private:
    glm::vec3 _Center;
    float     _Radius;
    OctreeNode* _Prev;
    std::array<std::unique_ptr<OctreeNode>, 8> _Next;
    std::vector<glm::vec3> _Points;
};

class NPGS_API Octree {
public:
    Octree(const glm::vec3& Center, float Radius, int MaxDepth = 8);

    void Insert(const glm::vec3& Point);
    void Query(const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const;

    template <typename Func>
    void Traverse(Func&& Pred) const;

    std::size_t GetSize() const;

private:
    void InsertImpl(OctreeNode* Node, const glm::vec3& Point, int Depth);
    void QueryImpl(OctreeNode* Node, const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const;

    template <typename Func>
    void TraverseImpl(OctreeNode* Node, Func&& Pred) const {
        if (Node == nullptr) {
            return;
        }

        Pred(*Node);

        for (int i = 0; i != 8; ++i) {
            TraverseImpl(Node->GetNext(i).get(), Pred);
        }
    }

    std::size_t GetSizeImpl(const OctreeNode* Node) const;

private:
    std::unique_ptr<OctreeNode> _Root;
    int  _MaxDepth;
};

_NPGS_END

#include "Octree.inl"
