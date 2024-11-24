#pragma once

#include <cmath>
#include <array>
#include <functional>
#include <future>
#include <memory>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Core/Base.h"
#include "Engine/Core/Utilities/ThreadPool.h"

_NPGS_BEGIN

template <typename LinkTarget>
class OctreeNode {
public:
    OctreeNode(const glm::vec3& Center, float Radius, OctreeNode* Prev)
        : _Center(Center), _Radius(Radius), _Prev(Prev), _bIsValid(true)
    {}

    bool Contains(const glm::vec3& Point) const {
        return (Point.x >= _Center.x - _Radius && Point.x <= _Center.x + _Radius &&
                Point.y >= _Center.y - _Radius && Point.y <= _Center.y + _Radius &&
                Point.z >= _Center.z - _Radius && Point.z <= _Center.z + _Radius);
    }

    int CalculateOctant(const glm::vec3& Point) const {
        int Octant = 0;

        if (Point.x >= _Center.x) Octant |= 4;
        if (Point.y >= _Center.y) Octant |= 2;
        if (Point.z >= _Center.z) Octant |= 1;

        return Octant;
    }

    bool IntersectSphere(const glm::vec3& Point, float Radius) const {
        glm::vec3 MinBound = _Center - glm::vec3(_Radius);
        glm::vec3 MaxBound = _Center + glm::vec3(_Radius);

        glm::vec3 ClosestPoint = glm::clamp(Point, MinBound, MaxBound);
        float Distance = glm::distance(Point, ClosestPoint);

        return Distance <= Radius;
    }

    const bool GetValidation() const {
        return _bIsValid;
    }

    const glm::vec3& GetCenter() const {
        return _Center;
    }

    float GetRadius() const {
        return _Radius;
    }

    std::unique_ptr<OctreeNode>& GetNextMutable(int Index) {
        return _Next[Index];
    }

    const std::unique_ptr<OctreeNode>& GetNext(int Index) const {
        return _Next[Index];
    }

    void AddPoint(const glm::vec3& Point) {
        _Points.emplace_back(Point);
    }

    void DeletePoint(const glm::vec3& Point) {
        auto it = std::find(_Points.begin(), _Points.end(), Point);
        if (it != _Points.end()) {
            _Points.erase(it);
        }
    }

    void RemoveStorage() {
        _Points.clear();
    }

    void AddLink(LinkTarget* Target) {
        _DataLink.emplace_back(Target);
    }

    template <typename Func>
    LinkTarget* GetLink(Func&& Pred) const {
        for (LinkTarget* Target : _DataLink) {
            if (Pred(Target)) {
                return Target;
            }
        }

        return nullptr;
    }

    void RemoveLinks() {
        _DataLink.clear();
    }

    std::vector<glm::vec3>& GetPointsMutable() {
        return _Points;
    }

    const std::vector<glm::vec3>& GetPoints() const {
        return _Points;
    }

    void SetValidation(bool bValidation) {
        _bIsValid = bValidation;
    }

    bool IsLeafNode() const {
        for (const auto& Next : _Next) {
            if (Next != nullptr) {
                return false;
            }
        }

        return true;
    }

private:
    glm::vec3   _Center;
    OctreeNode* _Prev;
    float       _Radius;
    bool        _bIsValid;

    std::array<std::unique_ptr<OctreeNode>, 8> _Next;
    std::vector<glm::vec3>   _Points;
    std::vector<LinkTarget*> _DataLink;
};

template <typename LinkTarget>
class Octree {
public:
    using NodeType = OctreeNode<LinkTarget>;

public:
    Octree(const glm::vec3& Center, float Radius, int MaxDepth = 8)
        :
        _Root(std::make_unique<NodeType>(Center, Radius, nullptr)),
        _ThreadPool(ThreadPool::GetInstance()), _MaxDepth(MaxDepth)
    {}

    void BuildEmptyTree(float LeafRadius) {
        int Depth = static_cast<int>(std::ceil(std::log2(_Root->GetRadius() / LeafRadius)));
        BuildEmptyTreeImpl(_Root.get(), LeafRadius, Depth);
    }

    void Insert(const glm::vec3& Point) {
        InsertImpl(_Root.get(), Point, 0);
    }

    void Delete(const glm::vec3& Point) {
        DeleteImpl(_Root.get(), Point);
    }

    void Query(const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const {
        QueryImpl(_Root.get(), Point, Radius, Results);
    }

    template <typename Func = std::function<bool(const NodeType&)>>
    NodeType* Find(const glm::vec3& Point, Func&& Pred = [](const NodeType&) -> bool { return true; }) const {
        return FindImpl(_Root.get(), Point, std::forward<Func>(Pred));
    }

    template <typename Func>
    void Traverse(Func&& Pred) const {
        TraverseImpl(_Root.get(), std::forward<Func>(Pred));
    }

    std::size_t GetCapacity() const {
        return GetCapacityImpl(_Root.get());
    }

    std::size_t GetSize() const {
        return GetSizeImpl(_Root.get());
    }

    const NodeType* const GetRoot() const {
        return _Root.get();
    }

private:
    void BuildEmptyTreeImpl(NodeType* Node, float LeafRadius, int Depth) {
        if (Node->GetRadius() <= LeafRadius || Depth == 0) {
            return;
        }

        std::vector<std::future<void>> Futures;
        float NextRadius = Node->GetRadius() * 0.5f;
        for (int i = 0; i != 8; ++i) {
            glm::vec3 Offset(
                (i & 1 ? 1 : -1) * NextRadius,
                (i & 2 ? 1 : -1) * NextRadius,
                (i & 4 ? 1 : -1) * NextRadius
            );
            Node->GetNextMutable(i) = std::make_unique<NodeType>(Node->GetCenter() + Offset, NextRadius, Node);
            if (Depth == static_cast<int>(std::ceil(std::log2(_Root->GetRadius() / LeafRadius)))) {
                Futures.emplace_back(
                    _ThreadPool->Commit(&Octree::BuildEmptyTreeImpl, this, Node->GetNextMutable(i).get(), LeafRadius, Depth - 1));
            } else {
                BuildEmptyTreeImpl(Node->GetNextMutable(i).get(), LeafRadius, Depth - 1);
            }
        }

        for (auto& Future : Futures) {
            Future.get();
        }
    }

    void InsertImpl(NodeType* Node, const glm::vec3& Point, int Depth) {
        if (!Node->Contains(Point) || Depth > _MaxDepth) {
            return;
        }

        if (Node->GetNext(0) == nullptr) {
            for (int i = 0; i != 8; ++i) {
                glm::vec3 NewCenter = Node->GetCenter();
                float Radius = Node->GetRadius();
                NewCenter.x += (i & 4) ? Radius * 0.5f : -Radius * 0.5f;
                NewCenter.y += (i & 2) ? Radius * 0.5f : -Radius * 0.5f;
                NewCenter.z += (i & 1) ? Radius * 0.5f : -Radius * 0.5f;
                Node->GetNextMutable(i) = std::make_unique<NodeType>(NewCenter, Radius * 0.5f, Node);
            }
        }

        int Octant = Node->CalculateOctant(Point);
        if (Depth == _MaxDepth) {
            Node->AddPoint(Point);
        } else {
            InsertImpl(Node->GetNext(Octant).get(), Point, Depth + 1);
        }
    }

    void DeleteImpl(NodeType* Node, const glm::vec3& Point) {
        if (Node == nullptr) {
            return;
        }

        if (Node->Contains(Point)) {
            if (Node->IsLeafNode()) {
                auto& Points = Node->GetPointsMutable();
                auto it = std::find(Points.begin(), Points.end(), Point);
                if (it != Points.end()) {
                    Points.erase(it);
                }
            } else {
                int Octant = Node->CalculateOctant(Point);
                DeleteImpl(Node->GetNextMutable(Octant).get(), Point);
            }

            if (Node->IsLeafNode() && Node->GetPoints().empty()) {
                for (int i = 0; i != 8; ++i) {
                    auto& Next = Node->GetNextMutable(i);
                    Next.reset();
                }
            }
        }
    }

    void QueryImpl(NodeType* Node, const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const {
        if (Node == nullptr || Node->GetNext(0) == nullptr) {
            return;
        }

        for (const auto& StoredPoint : Node->GetPoints()) {
            if (glm::distance(StoredPoint, Point) <= Radius && StoredPoint != Point) {
                Results.emplace_back(StoredPoint);
            }
        }

        for (int i = 0; i != 8; ++i) {
            NodeType* NextNode = Node->GetNext(i).get();
            if (NextNode != nullptr && NextNode->IntersectSphere(Point, Radius)) {
                QueryImpl(NextNode, Point, Radius, Results);
            }
        }
    }

    template <typename Func>
    NodeType* FindImpl(NodeType* Node, const glm::vec3& Point, Func&& Pred) const {
        if (Node == nullptr) {
            return nullptr;
        }

        if (Node->Contains(Point)) {
            if (Pred(*Node)) {
                return Node;
            }
        }

        for (int i = 0; i != 8; ++i) {
            NodeType* ResultNode = FindImpl(Node->GetNext(i).get(), Point, Pred);
            if (ResultNode != nullptr) {
                return ResultNode;
            }
        }

        return nullptr;
    }

    template <typename Func>
    void TraverseImpl(NodeType* Node, Func&& Pred) const {
        if (Node == nullptr) {
            return;
        }

        Pred(*Node);

        for (int i = 0; i != 8; ++i) {
            TraverseImpl(Node->GetNext(i).get(), Pred);
        }
    }

    std::size_t GetCapacityImpl(const NodeType* Node) const {
        if (Node == nullptr) {
            return 0;
        }

        if (Node->GetNext(0) == nullptr) {
            return Node->GetValidation() ? 1 : 0;
        }

        std::size_t Capacity = 0;
        for (int i = 0; i != 8; ++i) {
            Capacity += GetCapacityImpl(Node->GetNext(i).get());
        }

        return Capacity;
    }

    std::size_t GetSizeImpl(const NodeType* Node) const {
        if (Node == nullptr) {
            return 0;
        }

        std::size_t Size = Node->GetPoints().size();
        for (int i = 0; i != 8; ++i) {
            Size += GetSizeImpl(Node->GetNext(i).get());
        }

        return Size;
    }

private:
    std::unique_ptr<NodeType> _Root;
    ThreadPool*               _ThreadPool;
    int                       _MaxDepth;
};

_NPGS_END
