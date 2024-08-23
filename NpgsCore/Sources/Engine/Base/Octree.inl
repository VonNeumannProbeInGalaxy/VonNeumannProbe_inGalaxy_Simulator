#pragma once

#include "Octree.h"

_NPGS_BEGIN

inline const glm::vec3& OctreeNode::GetCenter() const {
    return _Center;
}

inline float OctreeNode::GetRadius() const {
    return _Radius;
}

inline std::unique_ptr<OctreeNode>& OctreeNode::GetChild(int Index) {
    return _Children[Index];
}

_NPGS_END
