#include "Octree.h"

_NPGS_BEGIN

OctreeNode::OctreeNode(const glm::vec3& Center, float Radius) : _Center(Center), _Radius(Radius) {}

bool OctreeNode::Contains(const glm::vec3& Point) const {
    return (Point.x >= _Center.x - _Radius && Point.x <= _Center.x + _Radius &&
            Point.y >= _Center.y - _Radius && Point.y <= _Center.y + _Radius &&
            Point.z >= _Center.z - _Radius && Point.z <= _Center.z + _Radius);
}

int OctreeNode::GetOctant(const glm::vec3& Point) const {
    int Octant = 0;

    if (Point.x >= _Center.x) {
        Octant |= 4;
    }

    if (Point.y >= _Center.y) {
        Octant |= 2;
    }

    if (Point.z >= _Center.z) {
        Octant |= 1;
    }

    return Octant;
}

Octree::Octree(const glm::vec3& Center, float Radius, int MaxDepth)
    : _Root(std::make_unique<OctreeNode>(Center, Radius)), _MaxDepth(MaxDepth)
{}

void Octree::Insert(const glm::vec3& Point) {
    InsertImpl(_Root.get(), Point, 0);
}

void Octree::Query(const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const {
    QueryImpl(_Root.get(), Point, Radius, Results);
}

// Temp
int RecursiveCount = 0;

void Octree::InsertImpl(OctreeNode* Node, const glm::vec3& Point, int Depth) {
    ++RecursiveCount;
    if (!Node->Contains(Point) || Depth > _MaxDepth) {
        return;
    }

    if (Node->GetChild(0) == nullptr) {
        for (int i = 0; i != 8; ++i) {
            glm::vec3 NewCenter = Node->GetCenter();
            float Radius = Node->GetRadius();
            NewCenter.x += (i & 4) ? Radius * 0.5f : -Radius * 0.5f;
            NewCenter.y += (i & 2) ? Radius * 0.5f : -Radius * 0.5f;
            NewCenter.z += (i & 1) ? Radius * 0.5f : -Radius * 0.5f;
            std::println("Recursive: {}, New center {}: ({}, {}, {})", RecursiveCount, i, NewCenter.x, NewCenter.y, NewCenter.z);
            Node->GetChild(i) = std::make_unique<OctreeNode>(NewCenter, Radius * 0.5f);
        }
    }

    int Octant = Node->GetOctant(Point);
    std::println("New center: ({}, {}, {})", Node->GetChild(Octant).get()->GetCenter().x, Node->GetChild(Octant).get()->GetCenter().y, Node->GetChild(Octant).get()->GetCenter().z);
    std::println("Octant: {}", Octant);
    //std::system("pause");
    InsertImpl(Node->GetChild(Octant).get(), Point, Depth + 1);
}

void Octree::QueryImpl(OctreeNode* Node, const glm::vec3& Point, float Radius, std::vector<glm::vec3>& Results) const {
    if (!Node->Contains(Point)) {
        return;
    }

    if (glm::distance(Node->GetCenter(), Point) <= Radius) {
        Results.emplace_back(Node->GetCenter());
    }

    if (Node->GetChild(0) != nullptr) {
        for (int i = 0; i != 8; ++i) {
            QueryImpl(Node->GetChild(i).get(), Point, Radius, Results);
        }
    }
}

_NPGS_END
