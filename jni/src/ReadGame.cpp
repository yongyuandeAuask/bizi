// ReadGame.cpp
// 添加在 PhysX.h 的开头
#include <vector>        // 提供 std::vector
#include <unordered_map> // 提供 std::unordered_map
#include <cstdint>       // 提供 uint8_t, uint16_t, uint32_t, uint64_t

#include <thread>
#include "ReadGame.h"
#include "PhysX.h"
#include "Draw.h"
#include "VectorHelper.h"
// 在 PhysX.h 文件的开头添加
#include <vector> // 提供 std::vector 容器
#include <cstdint> // 提供 uint8_t, uint16_t, uint32_t 等类型

FGameData GameData;
MinimalViewInfo POV;

void ReadGame::Raycast() {
    TriangleMeshData* mesh_ptr = LineTrace::getNextHint(POV.Rotation, POV.Location);
    if (mesh_ptr == nullptr) {
        return;
    }
    const TriangleMeshData& mesh = *mesh_ptr;
    try {
        if (!mesh.Vertices.empty() && !mesh.Indices.empty() && static_cast<uint32_t>(mesh.Type) < 10
            && static_cast<uint32_t>(mesh.Type) >= 0
            && mesh.Vertices.size() < 1000000
            && mesh.Indices.size() < 1000000) {
            for (size_t i = 0; i < mesh.Indices.size(); i += 3) {
                if (i + 2 >= mesh.Indices.size())
                    break;
                uint32_t idx0 = mesh.Indices[i];
                uint32_t idx1 = mesh.Indices[i + 1];
                uint32_t idx2 = mesh.Indices[i + 2];
                if (idx0 >= mesh.Vertices.size() || idx1 >= mesh.Vertices.size() || idx2 >= mesh.Vertices.size()) {
                    LOGD("无效索引: %u, %u, %u (顶点总数: %zu)", idx0, idx1, idx2, mesh.Vertices.size());
                    continue;
                }
                Vector3 v0 = LineTrace::ToVec3(mesh.Vertices[idx0]);
                Vector3 v1 = LineTrace::ToVec3(mesh.Vertices[idx1]);
                Vector3 v2 = LineTrace::ToVec3(mesh.Vertices[idx2]);
                auto p0 = VectorHelper::WorldToScreen((FVector&)v0, POV);
                auto p1 = VectorHelper::WorldToScreen((FVector&)v1, POV);
                auto p2 = VectorHelper::WorldToScreen((FVector&)v2, POV);
                if (VectorHelper::IsInScreen(p0) || VectorHelper::IsInScreen(p1) || VectorHelper::IsInScreen(p2)) {
                    ImGui::GetForegroundDrawList()->AddTriangle(ImVec2(p0.X, p0.Y), ImVec2(p1.X, p1.Y), ImVec2(p2.X, p2.Y), ImColor(0, 255, 0, 255), 0.3f);
                }
            }
        }
    } catch (...) {
    }
}

bool CustomLineTrace(const Vector3& location, const Vector3& coord) {
    bool result = LineTrace::LineTraceSingle(
        FVector(static_cast<float>(location.x), static_cast<float>(location.y), static_cast<float>(location.z)),
        FVector(static_cast<float>(coord.x), static_cast<float>(coord.y), static_cast<float>(coord.z))
    );
    return !result;
}

void StartLoadMapModel() {
    DynamicLoadScene = new Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>(Physics::prunerPayloadExtractor);
    HeightFieldScene = new Physics::VisibleScene<uint64_t, Int64Hash>(Physics::int64Extractor);
    DynamicRigidScene = new Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>(Physics::prunerPayloadExtractor);
    t1 = std::thread(VisibleCheck::UpdateDynamicHeightField);
    t2 = std::thread(VisibleCheck::UpdateDynamicRigid);
    t3 = std::thread(VisibleCheck::UpdateSceneByRange);
}

void StopWorkers() {
    g_bStopWorkers = true;
    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    if (t3.joinable()) t3.join();
    delete DynamicLoadScene;
    delete DynamicRigidScene;
    delete HeightFieldScene;
    DynamicLoadScene = nullptr;
    DynamicRigidScene = nullptr;
    HeightFieldScene = nullptr;
}

void ReadGame::Initialization() {
    target_pid = ReadProcess("com.tencent.ig");//要换
    GameData.UE4 = ReadModule("libUE4.so");
    PhysxInstancePtr = read<uintptr_t>(GameData.UE4 + OffsetPhysxInstancePtr);
    StartLoadMapModel();
}
   // 地址.自身地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4+0xecf15c0) + 0x810) + 0x78) + 0x38) + 0x78) + 0x30) + 0x28b0); //一键替换by币子 @bizin


// void ReadGame::ReadGameData() {
    // GameData.UWorld                = read<uintptr_t>(GameData.UE4 + 0xecf15c0);
    // auto PlayerController          = read<uintptr_t>(read<uintptr_t>(read<uintptr_t>(GameData.UWorld + 0x810) + 0x78) + 0x30);
    // auto PlayerCameraManager       = read<uintptr_t>(PlayerController + 0x548);
    // GetAddress(PlayerCameraManager + 0x630, &POV, sizeof(MinimalViewInfo));
// }
// 模块头 + 0xf1e8158
// 模块头 + 0xf27fd50

void ReadGame::ReadGameData() {
    uintptr_t tmp = read<uintptr_t>(GameData.UE4 + 0xf1fb900);
    tmp = read<uintptr_t>(tmp + 0x810);
    GameData.UWorld = read<uintptr_t>(tmp + 0x78);

    auto PlayerController = read<uintptr_t>(
        read<uintptr_t>(
            read<uintptr_t>(GameData.UWorld + 0x38) + 0x78
        ) + 0x30
    );

    auto PlayerCameraManager = read<uintptr_t>(PlayerController + 0x548);

    GetAddress(PlayerCameraManager + 0x530, &POV, sizeof(MinimalViewInfo));
    POV.FOV = read<float>(PlayerCameraManager + 0x554);
}