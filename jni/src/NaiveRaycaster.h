#pragma once

#include <cstdint>
#include <vector>
#include "PxVec3.h"
#include "PhysX.h"

struct Ray {
    physx::PxVec3 origin;
    physx::PxVec3 dir; // normalized
};

struct HitResult {
    bool hit;
    float t;
    uint32_t triIndex0, triIndex1, triIndex2;
};

bool RayIntersectsTriangle(const physx::PxVec3& orig, const physx::PxVec3& dir,
                           const physx::PxVec3& v0, const physx::PxVec3& v1, const physx::PxVec3& v2,
                           float& outT);

HitResult RaycastMesh(const TriangleMeshData& mesh, const Ray& ray);
