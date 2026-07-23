#include <unistd.h>
#include <android/log.h>
#include "ReadGame.h"
#include "NaiveRaycaster.h"
#include "PhysX.h"

#define TAG "physx_native_exe"

int main(int argc, char** argv) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "physx_native_exe start");
    // 初始化：读取 target pid / 模块基址 / 启动后台加载线程
    ReadGame::Initialization();

    // 主循环（示例：每 300ms 更新一次）
    for (int loop = 0; loop < 100000; ++loop) {
        ReadGame::ReadGameData(); // 更新 POV 和一些内存数据
        // 这里做一次简单射线示例：从相机向前投射一条射线
        Ray r;
        r.origin = physx::PxVec3(POV.Location.X, POV.Location.Y, POV.Location.Z);
        // forward vector from rotation
        float yaw = POV.Rotation.Yaw * M_PI / 180.0f;
        float pitch = POV.Rotation.Pitch * M_PI / 180.0f;
        r.dir = physx::PxVec3(cosf(yaw) * cosf(pitch), sinf(yaw) * cosf(pitch), sinf(pitch));
        // Normalise dir
        r.dir = r.dir.getNormalized();

        // 使用已有的 VisibleScene / DynamicLoadScene 等结构进行 Raycast（如果它们已初始化）
        bool hit = LineTrace::LineTraceSingle(
            FVector(r.origin.x, r.origin.y, r.origin.z),
            FVector(r.origin.x + r.dir.x * 20000.0f, r.origin.y + r.dir.y * 20000.0f, r.origin.z + r.dir.z * 20000.0f)
        );
        if (hit) {
            __android_log_print(ANDROID_LOG_INFO, TAG, "Ray hit!");
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, TAG, "Ray miss");
        }
        usleep(300000); // 300ms
    }

    StopWorkers();
    __android_log_print(ANDROID_LOG_INFO, TAG, "physx_native_exe exit");
    return 0;
}
