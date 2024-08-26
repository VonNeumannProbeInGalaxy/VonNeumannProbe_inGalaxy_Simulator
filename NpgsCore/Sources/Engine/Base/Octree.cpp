#include "Octree.h"
#include <thread>

_NPGS_BEGIN

OctreeNode::OctreeNode(const glm::vec3& Center, float Radius, OctreeNode* Prev)
    : _Center(Center), _Radius(Radius), _Prev(Prev), _bIsValid(true)
{}

Octree::Octree(const glm::vec3& Center, float Radius, int MaxDepth)
    : _Root(std::make_unique<OctreeNode>(Center, Radius, nullptr)), _MaxDepth(MaxDepth)
{}

void Octree::BuildEmptyTreeImpl(OctreeNode* Node, float LeafRadius, int Depth) {
    if (Node->GetRadius() <= LeafRadius || Depth == 0) {
        return;
    }

    float NextRadius = Node->GetRadius() * 0.5f;
    for (int i = 0; i != 8; ++i) {
        glm::vec3 Offset(
            (i & 1 ? 1 : -1) * NextRadius,
            (i & 2 ? 1 : -1) * NextRadius,
            (i & 4 ? 1 : -1) * NextRadius
        );
        Node->GetNextMutable(i) = std::make_unique<OctreeNode>(Node->GetCenter() + Offset, NextRadius, Node);
        BuildEmptyTreeImpl(Node->GetNextMutable(i).get(), LeafRadius, Depth - 1);
    }
}

void Octree::InsertImpl(OctreeNode* Node, const glm::vec3& Point, int Depth) {
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
            Node->GetNextMutable(i) = std::make_unique<OctreeNode>(NewCenter, Radius * 0.5f, Node);
        }
    }

    int Octant = Node->CalcOctant(Point);
    if (Depth == _MaxDepth) {
        Node->AddPoint(Point);
    } else {
        InsertImpl(Node->GetNext(Octant).get(), Point, Depth + 1);
    }
}

void Octree::DeleteImpl(OctreeNode* Node, const glm::vec3& Point) {
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
            int Octant = Node->CalcOctant(Point);
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

void Octree::QueryImpl(OctreeNode* Node, const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const {
    if (Node == nullptr || Node->GetNext(0) == nullptr) {
        return;
    }

    for (const auto& StoredPoint : Node->GetPoints()) {
        if (glm::distance(StoredPoint, Point) <= Radius && StoredPoint != Point) {
            Results.emplace_back(StoredPoint);
        }
    }

    for (int i = 0; i != 8; ++i) {
        OctreeNode* NextNode = Node->GetNext(i).get();
        if (NextNode != nullptr && NextNode->IntersectSphere(Point, Radius)) {
            QueryImpl(NextNode, Point, Radius, Results);
        }
    }
}

std::size_t Octree::GetCapacityImpl(const OctreeNode* Node) const {
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

std::size_t Octree::GetSizeImpl(const OctreeNode* Node) const {
    if (Node == nullptr) {
        return 0;
    }

    std::size_t Size = Node->GetPoints().size();
    for (int i = 0; i != 8; ++i) {
        Size += GetSizeImpl(Node->GetNext(i).get());
    }

    return Size;
}

_NPGS_END
