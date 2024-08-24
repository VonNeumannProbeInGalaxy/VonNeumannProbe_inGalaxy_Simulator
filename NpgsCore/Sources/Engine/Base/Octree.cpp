#include "Octree.h"

_NPGS_BEGIN

OctreeNode::OctreeNode(const glm::vec3& Center, float Radius, OctreeNode* Prev)
    : _Center(Center), _Radius(Radius), _Prev(Prev)
{}

Octree::Octree(const glm::vec3& Center, float Radius, int MaxDepth)
    : _Root(std::make_unique<OctreeNode>(Center, Radius, nullptr)), _MaxDepth(MaxDepth)
{}

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
            Node->GetNext(i) = std::make_unique<OctreeNode>(NewCenter, Radius * 0.5f, Node);
        }
    }

    int Octant = Node->GetOctant(Point);
    if (Depth == _MaxDepth) {
        Node->AddPoint(Point);
    } else {
        InsertImpl(Node->GetNext(Octant).get(), Point, Depth + 1);
    }
}

void Octree::QueryImpl(OctreeNode* Node, const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const {
    for (const auto& StoredPoint : Node->GetPoints()) {
        if (glm::distance(StoredPoint, Point) <= Radius) {
            Results.emplace_back(StoredPoint);
        }
    }

    if (Node->GetNext(0) != nullptr) {
        for (int i = 0; i != 8; ++i) {
            QueryImpl(Node->GetNext(i).get(), Point, Radius, Results);
        }
    }
}

_NPGS_END
