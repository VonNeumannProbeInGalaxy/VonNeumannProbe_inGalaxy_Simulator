#pragma once

#include <array>
#include <functional>
#include <future>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

class NPGS_API OctreeNode {
public:
    OctreeNode(const glm::vec3& Center, float Radius, OctreeNode* Prev);

    bool Contains(const glm::vec3& Point) const;
    int CalcOctant(const glm::vec3& Point) const;
    bool IntersectSphere(const glm::vec3& Point, float Radius) const;

    const bool GetValidation() const;
    const glm::vec3& GetCenter() const;
    float GetRadius() const;
    std::unique_ptr<OctreeNode>& GetNextMutable(int Index);
    const std::unique_ptr<OctreeNode>& GetNext(int Index) const;

    void AddPoint(const glm::vec3& Point);
    void DeletePoint(const glm::vec3& Point);
    void RemoveStorage();
    std::vector<glm::vec3>& GetPointsMutable();
    const std::vector<glm::vec3>& GetPoints() const;
    void SetValidation(bool bValidation);
    bool IsLeafNode() const;

private:
    bool      _bIsValid;
    glm::vec3 _Center;
    float     _Radius;
    OctreeNode* _Prev;
    std::array<std::unique_ptr<OctreeNode>, 8> _Next;
    std::vector<glm::vec3> _Points;
};

class NPGS_API Octree {
public:
    Octree(const glm::vec3& Center, float Radius, int MaxDepth = 8);

    void BuildEmptyTree(float LeafRadius);
    void Insert(const glm::vec3& Point);
    void Delete(const glm::vec3& Point);
    void Query(const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const;

    template <typename Func = std::function<bool(const OctreeNode&)>>
    bool Find(const glm::vec3& Point, Func&& Pred = [](const OctreeNode&) -> bool { return true; }) const;

    template <typename Func>
    void Traverse(Func&& Pred) const;

    std::size_t GetCapacity() const;
    std::size_t GetSize() const;
    const OctreeNode* const GetRoot() const;

private:
    void BuildEmptyTreeImpl(OctreeNode* Node, float LeafRadius, int Depth);
    void InsertImpl(OctreeNode* Node, const glm::vec3& Point, int Depth);
    void DeleteImpl(OctreeNode* Node, const glm::vec3& Point);
    void QueryImpl(OctreeNode* Node, const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const;

    template <typename Func>
    bool FindImpl(OctreeNode* Node, const glm::vec3& Point, Func&& Pred) const {
        if (Node == nullptr) {
            return false;
        }

        if (Node->Contains(Point)) {
            if (Pred(*Node)) {
                return true;
            }
        }

        for (int i = 0; i != 8; ++i) {
            if (FindImpl(Node->GetNext(i).get(), Point, Pred)) {
                return true;
            }
        }

        return false;
    }

    template <typename Func>
    void TraverseImpl(OctreeNode* Node, Func&& Pred, int Depth) const {
        if (Node == nullptr) {
            return;
        }

        Pred(*Node);

        for (int i = 0; i != 8; ++i) {
            TraverseImpl(Node->GetNext(i).get(), Pred, Depth + 1);
        }
    }

    std::size_t GetCapacityImpl(const OctreeNode* Node) const;
    std::size_t GetSizeImpl(const OctreeNode* Node) const;

private:
    std::unique_ptr<OctreeNode> _Root;
    int  _MaxDepth;
};

_NPGS_END

#include "Octree.inl"
