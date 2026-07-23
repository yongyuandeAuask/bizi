#include "NaiveRaycaster.h"
#include <cmath>
#include <limits>

bool RayIntersectsTriangle(const physx::PxVec3& orig, const physx::PxVec3& dir,
                           const physx::PxVec3& v0, const physx::PxVec3& v1, const physx::PxVec3& v2,
                           float& outT)
{
    const float EPSILON = 1e-6f;
    physx::PxVec3 edge1 = v1 - v0;
    physx::PxVec3 edge2 = v2 - v0;
    physx::PxVec3 h = dir.cross(edge2);
    float a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON) return false; // parallel
    float f = 1.0f / a;
    physx::PxVec3 s = orig - v0;
    float u = f * s.dot(h);
    if (u < 0.0f || u > 1.0f) return false;
    physx::PxVec3 q = s.cross(edge1);
    float v = f * dir.dot(q);
    if (v < 0.0f || u + v > 1.0f) return false;
    float t = f * edge2.dot(q);
    if (t > EPSILON) {
        outT = t;
        return true;
    }
    return false;
}

HitResult RaycastMesh(const TriangleMeshData& mesh, const Ray& ray) {
    HitResult res{};
    res.hit = false;
    float bestT = std::numeric_limits<float>::infinity();
    for (size_t i = 0; i + 2 < mesh.Indices.size(); i += 3) {
        uint32_t i0 = mesh.Indices[i];
        uint32_t i1 = mesh.Indices[i+1];
        uint32_t i2 = mesh.Indices[i+2];
        if (i0 >= mesh.Vertices.size() || i1 >= mesh.Vertices.size() || i2 >= mesh.Vertices.size()) continue;
        float t;
        if (RayIntersectsTriangle(ray.origin, ray.dir, mesh.Vertices[i0], mesh.Vertices[i1], mesh.Vertices[i2], t)) {
            if (t < bestT) {
                bestT = t;
                res.hit = true;
                res.t = t;
                res.triIndex0 = i0;
                res.triIndex1 = i1;
                res.triIndex2 = i2;
            }
        }
    }
    return res;
}
