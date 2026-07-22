/* 
 * by币子
 */

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "图片调用.h"
#include "物资ID.h"
#include "辅助类.h"

#include "Utils/PtraceUtils.h"

#include <memory>
#include <mutex>
#include <random>
#include "gryo/gryo.h"  // 陀螺仪头文件
// 在文件顶部添加
bool clickRegion(bool isClick, Rect clickRect);

int decrypt_zero_x() {
    return 0; // 暂时返回0，可能需要根据实际情况修改
}

int decrypt_zero_y() {
    return 0; // 暂时返回0，可能需要根据实际情况修改
}
// 在全局变量区域添加
FGyro *陀螺仪 = nullptr;
bool 陀螺仪已初始化 = false;

// 示例：在头文件（如 Utils.h）中声明
bool LineOfSightTo(const D3DVector& target);

// 在源文件（如 Utils.cpp）中定义
bool LineOfSightTo(const D3DVector& target) {
    // 实现视线检测逻辑
    return true; // 示例
}
extern 绘制 绘制;
char filename[64];
const char* MEMReadandwrite = "/storage/emulated/0/Download/Memory";
int MountFile;
std::mutex mtx;
unordered_map<int, ImColor> colorMap;
std::mutex printMutex;
std::map<std::string, int> Antitankgrenade;  // 给手雷一个机会,不然让你飞起来

void 绘制::readmaps_v(struct map_node** head) {
    char lj[32], buff[256];
    FILE* fp;
    sprintf(lj, "/proc/%d/maps", pid);
    fp = fopen(lj, "r");
    if (fp == NULL) {
        return;
    }
    while (fgets(buff, sizeof(buff), fp) != NULL) {
        if (strstr(buff, "rw-p 00000000 00:00 0") != NULL) {
            unsigned long start_addr, end_addr;
            sscanf(buff, "%lx-%lx", &start_addr, &end_addr);
            long int count = (end_addr - start_addr) / 0x4;
            if (count > 解密模式)
                continue;
            struct map_node* node = (struct map_node*)malloc(sizeof(struct map_node));
            node->start_addr = start_addr;
            node->end_addr = end_addr;
            node->next = NULL;
            if (*head == NULL) {
                *head = node;
            } else {
                struct map_node* cur = *head;
                while (cur->next != NULL) {
                    cur = cur->next;
                }
                cur->next = node;
            }
        }
    }
    fclose(fp);
}

void 绘制::free_maps(struct map_node* head) {
    struct map_node* cur = head;
    while (cur != NULL) {
        struct map_node* temp = cur;
        cur = cur->next;
        free(temp);
    }
}

void 绘制::print_maps(struct map_node* head) {
    struct map_node* cur = head;
    long int Objaddr, 循环次数 = 0;
    long int Arrayheader, Arrayheader_is = 0;
    int array_is, flagso = 0;
    bool jie_start = false;
    while (cur != NULL) {
        Objaddr = 0;
        循环次数 = 0;
        while (Objaddr <= cur->end_addr) {
            Objaddr = cur->start_addr + 0x4 * 循环次数;
            if (读写.getPtr64(Objaddr) == 地址.自身地址) {
                array_is = 0;
                flagso = 0;
                Arrayheader_is = Objaddr - 0x4;
                for (int i = 0; i < 100; i++) {
                    Arrayheader = Arrayheader_is - (0x8 * i);
                    int Array_size = 读写.getDword(Arrayheader);
                    if (Array_size == 0) {
                        flagso++;
                    }
                    if (Array_size >= 100 && Array_size <= 150) {
                        array_is++;
                    }
                    if (Array_size == 0 && flagso == 1 && array_is == 48) {
                        jie_start = true;
                        解密数组 = Arrayheader + 0x4;
                        break;
                    }
                }
            }
            if (jie_start)
                break;
            循环次数++;
        }
        if (jie_start)
            break;
        cur = cur->next;
    }
}

void 绘制::InitMaps() {
    struct map_node* head = NULL;
    readmaps_v(&head);
    print_maps(head);
    free_maps(head);
}
/*
string exec(string command) {
    char buffer[128];
    string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "popen failed!";
    }
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }
    pclose(pipe);
    return result;
}
*/
void MemInitialization() {
    std::string mountCmd;

    mountCmd = "umount ";
    mountCmd += MEMReadandwrite;
//    exec(mountCmd);

    mountCmd = "mount ";
    mountCmd += filename;
    mountCmd += " ";
    mountCmd += MEMReadandwrite;
   // exec(mountCmd);
}

bool writev(uintptr_t addr, void* buffer, size_t size) {
    return pwrite(MountFile, buffer, size, addr) != -1;
}

void writedword(unsigned long addr, int data) {
    writev(addr, &data, 4);
}

void writeptr(unsigned long addr, uintptr_t data) {
    writev(addr, &data, 8);
}

//------漏打东西
static FRotator ToRotator(const D3DVector& local, const D3DVector& target) {
    D3DVector rotation;
    rotation.X = local.X - target.X;
    rotation.Y = local.Y - target.Y;
    rotation.Z = local.Z - target.Z;
    float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);
    FRotator newViewAngle;
    newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float)3.14159265358979323846);
    newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float)3.14159265358979323846);
    newViewAngle.Roll = (float)0.f;
    if (rotation.X >= 0.f)
        newViewAngle.Yaw += 180.0f;
    return {newViewAngle.Yaw, newViewAngle.Pitch, 0};
}
std::random_device rand_device;
std::mt19937 engine(rand_device());
std::uniform_int_distribution<int> dist(0, 100);

uintptr_t CodePtr = 0;
uintptr_t infdata;
uintptr_t Controller;
long Arm64Index = 0;

#define TidOffset 0x40
#define CodeSize 24

int NewShoot[15] = {0, 0x00000000, 0x00000000, 0x18FFFFE8, 0x34000068, 0x1CFFFF83, 0x1CFFFF44, 0xB9436348, 0x2A0203F3, 0x2A0103F4, 0xAA0003F5, 0x58000051, 0xD61F0220, 0x00000000, 0x00000000};
int ArmHook[4] = {0x58000051, 0xD61F0220, 0x58695051, 0x58035151};
long rot = 0;
bool IsShoot = false;

int index = 1;
int bak = 0;
uintptr_t* getanonymousaddr() {
    bak = 0;
    uintptr_t* ret;
    ret = (uintptr_t*)calloc(2, sizeof(uintptr_t));
    uintptr_t start, end = 0;
    FILE* fp;
    uintptr_t addr = 0;
    char* pch;
    char filename[64];
    char line[1024];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", 绘制.pid);
    fp = fopen(filename, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "r-xp 00000000 00:00 0") && strlen(line) == 45) {
                bak++;
                if (bak == index) {
                    sscanf(line, "%lx-%lx", &start, &end);
                    fclose(fp);
                    ret[0] = start;
                    ret[1] = end;
                    return ret;
                }
            }
        }
        fclose(fp);
    }
    return ret;  // 没找到
}
int GetIndex() {
    index = 99;
    getanonymousaddr();
    return bak;
}
uintptr_t findptr() {
    index = 0;
重新计算:
    if (index >= GetIndex()) {
        return 0;
    }
    uintptr_t* ModuleBase = getanonymousaddr();
    int ModuldeSize = ModuleBase[1] - ModuleBase[0];
    for (int i = 0; i < ModuldeSize / 8; i++) {
        uintptr_t Valid = 绘制.读写.getPtr64(ModuleBase[0] + i * 8);
        if (Valid == infdata && infdata != 0)
            return ModuleBase[0] + i * 8;
    }
    index += 1;
    goto 重新计算;
}

uintptr_t IsValid(int add, int size) {
    uintptr_t ret = 0;

    uintptr_t* ModuleBase = getanonymousaddr();
    uintptr_t addr = ModuleBase[0] + add;
    int ModuldeSize = ModuleBase[1] - addr;
    for (int i = 0; i < ModuldeSize / 4; i++) {
        int Valid = 绘制.读写.getDword(addr + i * 4);
        if (Valid != 0) {
            addr += 4;
            i = 0;
            continue;
        }
        if (Valid == 0 && addr <= ModuleBase[1]) {
            ret = addr + 4;
            break;
        }
        if (addr > ModuleBase[1])
            break;
    }
    return ret;
}
long GetValidSize(uintptr_t address) {
    long ret = -1;
    for (long i = 0;; i++) {
        int Valid = 绘制.读写.getDword(address + i * 4);
        if (Valid == 0)
            ret++;

        if (Valid != 0) {
            break;
        }
    }
    return ret;
}
uintptr_t IsValid(int size) {
    uintptr_t ret = 0;
    srand(time(NULL));

    index = rand() % GetIndex() + 1;
    uintptr_t* ModuleBase = getanonymousaddr();
    ret = IsValid(0, ModuleBase[1] - ModuleBase[0]);
    if (ret != 0)
        return ret;
    return 0;
}

// struct pt_regs currentRegs, originalRegs;
void* pthreadAddr;
int callRemoteMmap() {
    long parameters[6];
    void* mmapAddr = getRemoteFuncAddr(绘制.pid, libcPath, (void*)mmap);
    LOGI("Mmap Function Address: 0x%lx\n", (uintptr_t)mmapAddr);
    // void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offsize);
    parameters[0] = 0;  // Not needed
    parameters[1] = 0x1000;
    parameters[2] = PROT_READ | PROT_WRITE;
    parameters[3] = MAP_ANONYMOUS | MAP_PRIVATE;
    parameters[4] = 0;  // Not needed
    parameters[5] = 0;  // Not needed
    // Call the mmap function of the target process
    if (ptrace_call(绘制.pid, (uintptr_t)mmapAddr, parameters, 6, &currentRegs)) {
        return -1;
    }
    return 0;
}
int callpthread_create(uintptr_t FunctionAddr, struct pt_regs* regs) {
    long parameters[6];
    parameters[0] = infdata + TidOffset;
    parameters[1] = 0x0;
    parameters[2] = FunctionAddr;
    parameters[3] = 0x0;
    if (ptrace_call(绘制.pid, (uintptr_t)pthreadAddr, parameters, 4, &currentRegs)) {
        return -1;
    }
    return 0;
}
int Initpthread = false;
bool callpthread() {
    callpthread_create(CodePtr + 0x8, &currentRegs);
    return true;
}

void 绘制::InitShoot() {
    fopen(MEMReadandwrite, "wb+");
    sprintf(filename, "/proc/%d/mem", this->pid);
    MemInitialization();  // 初始化
    MountFile = open(MEMReadandwrite, O_RDWR);

    CodePtr = IsValid(CodeSize);
    long ShootAddr = 地址.libue4 + 0x5634114;

    if (CodePtr > 0xFFFFFF && !IsShoot) {
        Arm64Index = GetValidSize(CodePtr);
        srand(time(NULL));
        long suji = rand() % (Arm64Index - CodeSize);
        CodePtr += (suji) * 4;
        rot = CodePtr;
        writev(CodePtr, &NewShoot, 4 * 15);
        writeptr(CodePtr + 0x34, ShootAddr + 0x10);
        writev(ShootAddr, &ArmHook, 4 * 4);
        writeptr(ShootAddr + 0x8, CodePtr + 0xC);
        IsShoot = true;
   //     exec("setenforce 1");
        // printf("%p\n",CodePtr);
    }
}

//-----漏打东西

ImColor RandomColor() {
    int R, G, B, A = 255;
    R = (random() % 255);
    G = (random() % 255);
    B = (random() % 255);
    return ImColor(R, G, B, A);
}

ImColor GetRandomColorById(int id) {
    if (colorMap.find(id) != colorMap.end()) {
        return colorMap[id];
    } else {
        ImColor color = RandomColor();
        colorMap[id] = color;
        return color;
    }
}

void 绘制::保存配置() {
    std::string 配置;
    配置 += "自瞄范围:" + std::to_string(自瞄.自瞄范围) + ";\n";
    配置 += "触摸范围:" + std::to_string(自瞄.触摸范围) + ";\n";
    配置 += "自瞄速度:" + std::to_string(自瞄.自瞄速度) + ";\n";
    配置 += "压枪力度:" + std::to_string(自瞄.压枪力度) + ";\n";
    配置 += "预判力度:" + std::to_string(自瞄.预判力度) + ";\n";
    配置 += "雷达X:" + std::to_string(按钮.雷达X) + ";\n";
    配置 += "雷达Y:" + std::to_string(按钮.雷达Y) + ";\n";
    配置 += "趴下位置调节:" + std::to_string(自瞄.趴下位置调节) + ";\n";
    配置 += "触摸采样率:" + std::to_string(自瞄.触摸采样率) + ";\n";
    配置 += "喷子距离限制:" + std::to_string(自瞄.喷子距离限制) + ";\n";
    配置 += "掉血自瞄数率:" + std::to_string(自瞄.掉血自瞄数率) + ";\n";

    配置 += "腰射距离限制:" + std::to_string(自瞄.腰射距离限制) + ";\n";
    配置 += "自瞄距离限制:" + std::to_string(自瞄.自瞄距离限制) + ";\n";
    配置 += "近战距离限制:" + std::to_string(自瞄.近战限制距离) + ";\n";
    
    配置 += "触摸范围X:" + std::to_string(自瞄.触摸范围X) + ";\n";
    配置 += "触摸范围Y:" + std::to_string(自瞄.触摸范围Y) + ";\n";

    配置 += "方框粗细:" + std::to_string(按钮.方框粗细) + ";\n";
    配置 += "手持绘图:" + std::to_string(按钮.手持绘图) + ";\n";
    配置 += "射线粗细:" + std::to_string(按钮.射线粗细) + ";\n";
    配置 += "骨骼粗细:" + std::to_string(按钮.骨骼粗细) + ";\n";

    配置 += "速度值:" + std::to_string(按钮.速度值) + ";\n";
    配置 += "第三人称:" + std::to_string(按钮.第三人称) + ";\n";
    
配置 += "不开镜灵敏度:" + std::to_string(按钮.不开镜灵敏度) + ";\n";
配置 += "机瞄红点灵敏度:" + std::to_string(按钮.机瞄红点灵敏度) + ";\n";
配置 += "二倍灵敏度:" + std::to_string(按钮.二倍灵敏度) + ";\n";
配置 += "三倍灵敏度:" + std::to_string(按钮.三倍灵敏度) + ";\n";
配置 += "四倍灵敏度:" + std::to_string(按钮.四倍灵敏度) + ";\n";
配置 += "六倍灵敏度:" + std::to_string(按钮.六倍灵敏度) + ";\n";
配置 += "八倍灵敏度:" + std::to_string(按钮.八倍灵敏度) + ";\n";

    // 配置 += "第一人称:"+ std::to_string(按钮.第一人称) + ";\n";
    // 配置 += "开镜广角:"+ std::to_string(按钮.开镜广角) + ";\n";

    // 配置 += "追踪概率:"+ std::to_string(按钮.追踪概率) + ";\n";

    配置 += "自瞄条件:" + std::to_string(自瞄.自瞄条件) + ";\n";
    配置 += "充电口方向:" + std::to_string(自瞄.充电口方向) + ";\n";

    配置 += "当前帧率:" + std::to_string(按钮.当前帧率) + ";\n";
    配置 += "帧率选项:" + std::to_string(按钮.帧率选项) + ";\n";

    配置 += "骨骼距离限制:" + std::to_string(骨骼距离限制) + ";\n";
    
    配置 += "压枪系数50_100:" + std::to_string(自瞄.压枪系数_50_100) + ";\n";
    配置 += "压枪系数100_150:" + std::to_string(自瞄.压枪系数_100_150) + ";\n";
    配置 += "压枪系数150_200:" + std::to_string(自瞄.压枪系数_150_200) + ";\n";
    配置 += "压枪系数200_250:" + std::to_string(自瞄.压枪系数_200_250) + ";\n";
    配置 += "压枪系数250_300:" + std::to_string(自瞄.压枪系数_250_300) + ";\n";    

    // 步枪压枪
    配置 += "M416压枪:" + std::to_string(自瞄.m416) + ";\n";
    配置 += "SCAR_L压枪:" + std::to_string(自瞄.scar_l) + ";\n";
    配置 += "AUG压枪:" + std::to_string(自瞄.aug) + ";\n";
    配置 += "FAMAS压枪:" + std::to_string(自瞄.famas) + ";\n";
    配置 += "G36C压枪:" + std::to_string(自瞄.g36c) + ";\n";
    配置 += "M249压枪:" + std::to_string(自瞄.m249) + ";\n";
    配置 += "AKM压枪:" + std::to_string(自瞄.akm) + ";\n";
    配置 += "M762压枪:" + std::to_string(自瞄.m762) + ";\n";
    配置 += "蜜獾压枪:" + std::to_string(自瞄.蜜獾) + ";\n";
    配置 += "PKM压枪:" + std::to_string(自瞄.pkm) + ";\n";
    配置 += "MG3压枪:" + std::to_string(自瞄.mg3) + ";\n";
    配置 += "MG_36压枪:" + std::to_string(自瞄.mg_36) + ";\n";
    
    // 冲锋枪压枪
    配置 += "P90压枪:" + std::to_string(自瞄.p90) + ";\n";
    配置 += "UZI压枪:" + std::to_string(自瞄.uzi) + ";\n";
    配置 += "UMP45压枪:" + std::to_string(自瞄.ump45) + ";\n";
    配置 += "VECTOR压枪:" + std::to_string(自瞄.vector) + ";\n";
    配置 += "汤姆逊压枪:" + std::to_string(自瞄.汤姆逊) + ";\n";
    配置 += "野牛压枪:" + std::to_string(自瞄.野牛) + ";\n";
    
    // 射手步枪
    配置 += "MINI14压枪:" + std::to_string(自瞄.mini14) + ";\n";
    配置 += "SKS压枪:" + std::to_string(自瞄.sks) + ";\n";
    配置 += "M417压枪:" + std::to_string(自瞄.m417) + ";\n";
    配置 += "MK20_H压枪:" + std::to_string(自瞄.mk20_h) + ";\n";
    配置 += "MK12压枪:" + std::to_string(自瞄.mk12) + ";\n";
    配置 += "VSS压枪:" + std::to_string(自瞄.vss) + ";\n";
    
    // 握把压枪
    配置 += "轻型握把压枪:" + std::to_string(轻型压枪力度) + ";\n";
    配置 += "垂直握把压枪:" + std::to_string(垂直压枪力度) + ";\n";
    配置 += "拇指握把压枪:" + std::to_string(拇指压枪力度) + ";\n";
    配置 += "直角握把压枪:" + std::to_string(直角压枪力度) + ";\n";

    std::ofstream outputFile("/data/币子按钮配置", ios::out | ios::trunc);
    outputFile << 配置;
    outputFile.close();

    string SaveFile = "/data/币子颜色配置";
    numSave = fopen(SaveFile.c_str(), "wb+");
    fseek(numSave, 0, SEEK_SET);
    fwrite(Colorset, sizeof(ColorTable), 2, numSave);
    fwrite(握把, sizeof(float), 100, numSave);
    fwrite(Colorset, sizeof(ColorTable), 2, numSave);
    fwrite(static_cast<void*>(&压枪力), sizeof(压枪力), 1, numSave);
    fwrite(static_cast<void*>(&预判度), sizeof(预判度), 1, numSave);
    fwrite(static_cast<void*>(&手持颜色), sizeof(手持颜色), 1, numSave);
    fwrite(static_cast<void*>(&物资颜色), sizeof(物资颜色), 1, numSave);
    fwrite(static_cast<void*>(&车辆颜色), sizeof(车辆颜色), 1, numSave);

    fwrite(static_cast<void*>(&按钮.绘制), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.忽略人机), sizeof(bool), 1, numSave);
    // fwrite(static_cast<void *>(&按钮.手持2), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.被瞄预警), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.人物加速), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.物资总开关), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.绘制信号枪), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.绘制金插), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.绘制宝箱), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.绘制药箱), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.绘制武器箱), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.头甲包显示), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.盒子物资), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.头甲包显示2), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.隐藏已开启), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.初始化), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.隐藏自瞄圈), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.随机触摸点), sizeof(bool), 1, numSave);

    fwrite(static_cast<void*>(&按钮.人数), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.隐藏名字背景), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.方框), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.血量), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.手持), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.盒子图片), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.盒子文字), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.背敌预警), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.距离), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.射线), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.名字), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.骨骼), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.车辆), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.雷达), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.手雷预警), sizeof(bool), 1, numSave);

    fwrite(static_cast<void*>(&自瞄.触摸位置), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.近战限制开关), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.烟雾不触发自瞄), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.动态自瞄), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.准星射线), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.倒地不瞄), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.掉血自瞄), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.自瞄控件), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.喷子自瞄), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.狙击自瞄), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.人机不瞄), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&自瞄.框内自瞄), sizeof(bool), 1, numSave);

    fwrite(static_cast<void*>(&按钮.其他物资), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.手枪枪械), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示步枪), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.散弹枪械), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.冲锋枪械), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.狙击枪械), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示子弹), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示倍镜), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示扩容), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示配件), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示药品), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示防具), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.投掷物品), sizeof(bool), 1, numSave);

    fwrite(static_cast<void*>(&按钮.显示医疗箱), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示急救包), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示绷带), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示可乐), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示肾上腺素), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示止痛药), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示三级甲), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示三级头), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示三级包), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.其他物资), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.狙击枪械), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示步枪), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.冲锋枪械), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.散弹枪械), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示762子弹), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示556子弹), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示9mm子弹), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示霰弹), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示45mm子弹), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示信号弹), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示箭矢), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示倍镜), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示子弹袋), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示箭袋), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示激光瞄准器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示轻型握把), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示半截握把), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示UZI枪托), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示狙击枪托), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示步枪枪托), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示狙击枪补偿器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示狙击枪消焰器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示狙击枪消音器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示步枪消音器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示步枪补偿器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示步枪消焰器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示冲锋枪消音器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示冲锋枪消焰器), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示拇指握把), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示垂直握把), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示直角握把), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示撞火枪托), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示霰弹快速), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示鸭嘴枪口), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示霰弹收束), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.显示扩容), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.投掷物品), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.载具血量), sizeof(bool), 1, numSave);
    fwrite(static_cast<void*>(&按钮.载具油量), sizeof(bool), 1, numSave);

    fflush(numSave);
    fsync(fileno(numSave));
    fclose(numSave);
}

void 绘制::读取配置() {
    std::ifstream inputFile("/data/币子按钮配置");
    std::string line;
    while (std::getline(inputFile, line)) {
        if (line.find("自瞄范围:") != std::string::npos) {
            自瞄.自瞄范围 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("触摸范围:") != std::string::npos) {
            自瞄.触摸范围 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("自瞄速度:") != std::string::npos) {
            自瞄.自瞄速度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("压枪力度:") != std::string::npos) {
            自瞄.压枪力度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("预判力度:") != std::string::npos) {
            自瞄.预判力度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("雷达X:") != std::string::npos) {
            按钮.雷达X = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("雷达Y:") != std::string::npos) {
            按钮.雷达Y = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("趴下位置调节:") != std::string::npos) {
            自瞄.趴下位置调节 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("触摸采样率:") != std::string::npos) {
            自瞄.触摸采样率 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("喷子距离限制:") != std::string::npos) {
            自瞄.喷子距离限制 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("掉血自瞄数率:") != std::string::npos) {
            自瞄.掉血自瞄数率 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("方框粗细:") != std::string::npos) {
            按钮.方框粗细 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("射线粗细:") != std::string::npos) {
            按钮.射线粗细 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("骨骼粗细:") != std::string::npos) {
            按钮.骨骼粗细 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("触摸范围X:") != std::string::npos) {
            自瞄.触摸范围X = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("触摸范围Y:") != std::string::npos) {
            自瞄.触摸范围Y = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("自瞄距离限制:") != std::string::npos) {
            自瞄.自瞄距离限制 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("腰射距离限制:") != std::string::npos) {
            自瞄.腰射距离限制 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("速度值:") != std::string::npos) {
            按钮.速度值 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("第三人称:") != std::string::npos) {
            按钮.第三人称 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("不开镜灵敏度:") != std::string::npos) {
            按钮.不开镜灵敏度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("机瞄红点灵敏度:") != std::string::npos) {
            按钮.机瞄红点灵敏度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("二倍灵敏度:") != std::string::npos) {
            按钮.二倍灵敏度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("三倍灵敏度:") != std::string::npos) {
            按钮.三倍灵敏度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("四倍灵敏度:") != std::string::npos) {
            按钮.四倍灵敏度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("六倍灵敏度:") != std::string::npos) {
            按钮.六倍灵敏度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("八倍灵敏度:") != std::string::npos) {
            按钮.八倍灵敏度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        
            /*}else if (line.find("第一人称:") != std::string::npos) {
              按钮.第一人称 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
            }else if (line.find("开镜广角:") != std::string::npos) {
              按钮.开镜广角 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
            }else if (line.find("追踪概率:") != std::string::npos) {
              按钮.追踪概率 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
           */
        } else if (line.find("充电口方向:") != std::string::npos) {
            自瞄.充电口方向 = std::stoi(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("手持绘图:") != std::string::npos) {
            按钮.手持绘图 = std::stoi(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("自瞄条件:") != std::string::npos) {
            自瞄.自瞄条件 = std::stoi(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("帧率选项:") != std::string::npos) {
            按钮.帧率选项 = std::stoi(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("当前帧率:") != std::string::npos) {
            按钮.当前帧率 = std::stoi(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("骨骼距离限制:") != std::string::npos) {
            骨骼距离限制 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("mk20-h:") != std::string::npos) {
            mk20 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        } else if (line.find("m417:") != std::string::npos) {
            m417 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("压枪系数50_100:") != std::string::npos) {
            自瞄.压枪系数_50_100 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("压枪系数100_150:") != std::string::npos) {
            自瞄.压枪系数_100_150 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("压枪系数150_200:") != std::string::npos) {
            自瞄.压枪系数_150_200 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("压枪系数200_250:") != std::string::npos) {
            自瞄.压枪系数_200_250 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("压枪系数250_300:") != std::string::npos) {
            自瞄.压枪系数_250_300 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("M416压枪:") != std::string::npos) {
            自瞄.m416 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("SCAR_L压枪:") != std::string::npos) {
            自瞄.scar_l = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("AUG压枪:") != std::string::npos) {
            自瞄.aug = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("FAMAS压枪:") != std::string::npos) {
            自瞄.famas = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("P90压枪:") != std::string::npos) {
            自瞄.p90 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("UZI压枪:") != std::string::npos) {
            自瞄.uzi = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("UMP45压枪:") != std::string::npos) {
            自瞄.ump45 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("MINI14压枪:") != std::string::npos) {
            自瞄.mini14 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("SKS压枪:") != std::string::npos) {
            自瞄.sks = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("M417压枪:") != std::string::npos) {
            自瞄.m417 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("轻型握把压枪:") != std::string::npos) {
            轻型压枪力度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("垂直握把压枪:") != std::string::npos) {
            垂直压枪力度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("拇指握把压枪:") != std::string::npos) {
            拇指压枪力度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("直角握把压枪:") != std::string::npos) {
            直角压枪力度 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("近战限制距离:") != std::string::npos) {
        自瞄.近战限制距离 = std::stof(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }else if (line.find("烟雾不触发自瞄:") != std::string::npos) {
        自瞄.烟雾不触发自瞄 = std::stoi(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
        }   
}

    string SaveFile = "/data";
    SaveFile += "/";
    SaveFile += "币子颜色配置";
    numSave = fopen(SaveFile.c_str(), "rb+");
    if (numSave == nullptr) {
        return;
    }
    fseek(numSave, 0, SEEK_SET);
    fread(Colorset, sizeof(ColorTable), 2, numSave);
    fread(握把, sizeof(float), 100, numSave);
    fread(Colorset, sizeof(ColorTable), 2, numSave);
    fread(static_cast<void*>(&压枪力), sizeof(压枪力), 1, numSave);
    fread(static_cast<void*>(&预判度), sizeof(预判度), 1, numSave);
    fread(static_cast<void*>(&手持颜色), sizeof(手持颜色), 1, numSave);
    fread(static_cast<void*>(&物资颜色), sizeof(物资颜色), 1, numSave);
    fread(static_cast<void*>(&车辆颜色), sizeof(车辆颜色), 1, numSave);

    fread(static_cast<void*>(&按钮.绘制), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.忽略人机), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.被瞄预警), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.人物加速), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.物资总开关), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.绘制信号枪), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.绘制金插), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.绘制宝箱), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.绘制药箱), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.绘制武器箱), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.头甲包显示), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.盒子物资), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.头甲包显示2), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.隐藏已开启), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.初始化), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.隐藏自瞄圈), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.随机触摸点), sizeof(bool), 1, numSave);

    fread(static_cast<void*>(&按钮.人数), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.隐藏名字背景), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.方框), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.血量), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.手持), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.盒子文字), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.盒子图片), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.背敌预警), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.距离), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.射线), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.名字), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.骨骼), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.车辆), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.雷达), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.手雷预警), sizeof(bool), 1, numSave);

    fread(static_cast<void*>(&自瞄.触摸位置), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.近战限制开关), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.动态自瞄), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.准星射线), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.倒地不瞄), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.掉血自瞄), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.自瞄控件), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.喷子自瞄), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.狙击自瞄), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.人机不瞄), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&自瞄.框内自瞄), sizeof(bool), 1, numSave);

    fread(static_cast<void*>(&按钮.其他物资), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.手枪枪械), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示步枪), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.冲锋枪械), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.狙击枪械), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示子弹), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示倍镜), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示扩容), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示配件), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示药品), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示防具), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.投掷物品), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.散弹枪械), sizeof(bool), 1, numSave);

    fread(static_cast<void*>(&按钮.显示医疗箱), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示急救包), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示绷带), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示可乐), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示肾上腺素), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示止痛药), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示三级甲), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示三级头), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示三级包), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.其他物资), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.狙击枪械), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示步枪), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.冲锋枪械), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.散弹枪械), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示762子弹), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示556子弹), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示9mm子弹), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示霰弹), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示45mm子弹), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示信号弹), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示箭矢), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示倍镜), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示子弹袋), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示箭袋), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示激光瞄准器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示轻型握把), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示半截握把), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示UZI枪托), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示狙击枪托), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示步枪枪托), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示狙击枪补偿器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示狙击枪消焰器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示狙击枪消音器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示步枪消音器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示步枪补偿器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示步枪消焰器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示冲锋枪消音器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示冲锋枪消焰器), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示拇指握把), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示垂直握把), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示直角握把), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示撞火枪托), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示霰弹快速), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示鸭嘴枪口), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示霰弹收束), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.显示扩容), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.绘制空投), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.投掷物品), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.载具血量), sizeof(bool), 1, numSave);
    fread(static_cast<void*>(&按钮.载具油量), sizeof(bool), 1, numSave);
    fclose(numSave);

    inputFile.close();
    
}


void 绘制::OffScreen(ImDrawList* ImDraw, D4DVector Obj, float camear, ImU32 color, float Radius, float 距离) {
    ImRect screen_rect = {0.0f, 0.0f, 真实PX, 真实PY};
    if (Obj.Z > 0 && screen_rect.Contains({Obj.X, Obj.Y}))
        return;
    auto screen_center = screen_rect.GetCenter();
    auto angle = atan2(screen_center.y - Obj.Y, screen_center.x - Obj.X);
    angle += camear > 0 ? M_PI : 0.0f;
    D2DVector arrow_center{
        screen_center.x + Radius * cosf(angle),
        screen_center.y + Radius * sinf(angle)};
    /*
    std::array<ImVec2, 4> points {
    ImVec2(-30.8f, -12.04f),
    ImVec2(0.0f, 0.0f),
    ImVec2(-38.8f, 12.4f),
    ImVec2(-25.2f, 0.0f)
    };
    */
    std::array<ImVec2, 4> points{
        ImVec2(-22.0f, -8.6f),
        ImVec2(0.0f, 0.0f),
        ImVec2(-22.0f, 8.6f),
        ImVec2(-18.0f, 0.0f)};
    D2DVector tpoint;
    for (auto& point : points) {
        auto x = point.x * 1.155f;
        auto y = point.y * 1.155f;
        point.x = arrow_center.X + x * cosf(angle) - y * sinf(angle);
        point.y = arrow_center.Y + x * sinf(angle) + y * cosf(angle);
        tpoint.X = point.x;
        tpoint.Y = point.y;
    }
    float alpha = 1.0f;
    if (camear > 0) {
        constexpr float nearThreshold = 200 * 200;
        ImVec2 screen_outer_diff = {
            Obj.X < 0 ? abs(Obj.X) : (Obj.X > screen_rect.Max.x ? Obj.X - screen_rect.Max.x : 0.0f),
            Obj.Y < 0 ? abs(Obj.Y) : (Obj.Y > screen_rect.Max.y ? Obj.Y - screen_rect.Max.y : 0.0f),
        };
        float distance = static_cast<float>(pow(screen_outer_diff.x, 2) + pow(screen_outer_diff.y, 2));
        alpha = camear < 0 ? 1.0f : (distance / nearThreshold);
    }
    ImColor arrowColor = color;
    // arrowColor.Value.w = std::min(alpha, 1.0f);
    arrowColor.Value.w = (alpha < 1.0f) ? alpha : 1.0f;
    ImDraw->AddTriangleFilled(points[0], points[1], points[3], arrowColor);
    ImDraw->AddTriangleFilled(points[2], points[1], points[3], arrowColor);
    ImDraw->AddQuad(points[0], points[1], points[2], points[3], ImColor(0.0f, 0.0f, 0.0f, alpha), 1.335f);

    float radius = 30.0f;
    ImColor textColor = (arrowColor.Value.x == 0.0f) ? ImColor(0.0f, 1.0f, 0.0f, alpha) : ImColor(1.0f, 0.0f, 0.0f, alpha);
    ImDraw->AddCircle({tpoint.X, tpoint.Y}, radius, textColor, 12, 2.0f);

    string tmp = to_string((int)距离) + "M";
    auto textSize = ImGui::CalcTextSize(tmp.c_str(), 0, 28);
    ImDraw->AddText(NULL, 28, {tpoint.X - (textSize.x / 2), tpoint.Y}, ImColor(1.0f, 1.0f, 1.0f, alpha), tmp.c_str());
}

void 绘制::初始化绘制(string 包名, int 真实X, int 真实Y) {
    int pid1 = 读写.getPID(包名.c_str());
    this->pid = pid1;
    读写.初始化读写(pid1);
    绘图.初始化绘图(真实X, 真实Y);
    this->真实PX = 真实X;
    this->真实PY = 真实Y;
    骨骼 = new class 骨骼(&读写);
    if (真实Y < 真实X) {
        this->PX = 真实X / 2;
        this->PY = 真实Y / 2;
    } else {
        this->PX = 真实Y / 2;
        this->PY = 真实X / 2;
    }
    读写.init_key("bawanglong");
    地址.libue4 = 读写.get_module_base((char*)"libUE4.so");
    if (读写.选择值 == 0 || 读写.选择值 ==1 || 读写.选择值 == 2 || 读写.选择值 ==3) {
        读写.initialize(pid); // 确保驱动正确初始化
}
printf("[-] 模块地址：0x%lx\n", 地址.libue4);
        if(自瞄模式 == 1){
    陀螺仪 = FGyro::GetInstance();
      if (陀螺仪) {
        陀螺仪已初始化 = 陀螺仪->Init();
        if (陀螺仪已初始化) {
          printf("[+] 陀螺仪初始化成功\n");
          // 测试发送事件（清零）
          陀螺仪->Send_Event(0, 0);
          usleep(50000);
          陀螺仪->Unlock_Event();
        } else {
          printf("[-] 陀螺仪初始化失败\n");
          陀螺仪已初始化 = false;
        }
      } else {
        printf("[-] 获取陀螺仪实例失败\n");
        陀螺仪已初始化 = false;
      }
      }
    // 地址.libue4 = 读写.get_module_base(pid1,(char*)"libUE4.so");
}

int 绘制::findminat() {
    float DistanceMin = 450.0f;
    float min = 自瞄.自瞄范围;
    int minAt = 999;
    for (int i = 0; i < 自瞄.瞄准总数量; i++) {
        switch ((int)自瞄.瞄准优先) {
            case 0:
                if (自瞄函数[i].准心距离 < min && 自瞄函数[i].准心距离 != 0) {
                    min = 自瞄函数[i].准心距离;
                    minAt = i;
                }
                break;
            case 1:
                if (自瞄函数[i].准心距离 < 自瞄.自瞄范围) {
                    if (自瞄函数[i].距离 < DistanceMin) {
                        DistanceMin = 自瞄函数[i].距离;
                        minAt = i;
                    }
                }
                break;
        }
    }
    if (minAt == 999) {
        自瞄.瞄准目标 = -1;
        return -1;
    }
    自瞄.瞄准目标 = minAt;
    return minAt;
}

void 绘制::GetTouch() {
    std::thread* 触摸位置线程 = new std::thread([&] {
    for(;;)
    {
      usleep(1000000 / 120);
      ImGuiIO& iooi = ImGui::GetIO();
      if (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
      {
        usleep(30000);
        if (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
        {
          while (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
          {
            自瞄.触摸范围X = iooi.MousePos.x;
            自瞄.触摸范围Y = displayInfo.height - iooi.MousePos.y;
            usleep(500);
          }
        }
      }
    } });
    触摸位置线程->detach();
}
std::map<uintptr_t, std::pair<D3DVector, float>> activeSmokes; // 烟雾弹地址:(位置,生成时间)
bool 绘制::自瞄触发(float 距离) {
    
    if (自瞄.烟雾不触发自瞄) {
    for (const auto& smoke : activeSmokes) {
        // 计算敌人到烟雾中心的距离
        float distToSmoke = 计算.计算距离(自瞄函数[自瞄.瞄准目标].瞄准坐标, smoke.second.first);
        if (distToSmoke < 4.0f) { // 4米范围内
            自瞄.瞄准目标 = -1;
            break;
        }
    }
}
    
    if (自瞄.近战限制开关 && 
        自身数据.开镜 != 0 && 
        距离 <= 自瞄.近战限制距离) {
        return false; // 开镜且距离在限制范围内时不触发
    }
        
    // if (自瞄.喷子自瞄) {
        // if (自身数据.手持 == 104003 or 自身数据.手持 == 104005 or 自身数据.手持 == 104100 or 自身数据.手持 == 104004) {
            // if (距离 < 自瞄.喷子距离限制) {
                // if (自瞄.喷子自瞄条件 == 0 or (自瞄.喷子自瞄条件 == 1 && 自身数据.开镜 != 0)) {
                    // return true;
                // }
            // }
        // }
    // }
            if (自瞄.龙息弹) {
        if (自身数据.手持 == 106001 or 自身数据.手持 == 106002 or 自身数据.手持 == 106003 or 自身数据.手持 == 106004 or 自身数据.手持 == 106005 or 自身数据.手持 == 106010 or 自身数据.手持 == 106008 or 自身数据.手持 == 106011 or 自身数据.手持 == 106007 or 自身数据.手持 == 106009) {
            // if (距离 < 自瞄.龙息弹距离限制) {
                // if (自瞄.龙息弹自瞄条件 == 0) {
                    return true;
                // }
            // }
        }
    }

            if(自瞄.喷子自瞄) {
                if (自身数据.手持 == 104003 or 自身数据.手持 == 104005 or 自身数据.手持 == 104100 or 自身数据.手持 == 104004 || 自身数据.手持 == 9810041 || 自身数据.手持 == 9810032 || 自身数据.手持 == 9810025 || 自身数据.手持 == 9810015) {
                    if (距离 < 自瞄.喷子距离限制) {
                        if(自瞄.喷子自瞄条件 == 0 or (自瞄.喷子自瞄条件 == 1 && 自身数据.开镜 != 0) or (自瞄.喷子自瞄条件 == 2 /*&& 自身数据.喷子开火 != 0*/)) {
                            // if (自瞄.喷子连点)
                            // {
                                // clickRegion(true,自瞄.连点位置);//连点
                            // }
                            return true;
                        }
                    }
                }
    //连狙连点自瞄
                if (自身数据.手持 == 103009 or
                自身数据.手持 == 103005 or
                自身数据.手持 == 103006 or
                自身数据.手持 == 103010 or
                自身数据.手持 == 103004 or
                自身数据.手持 == 103013 or
        自身数据.手持 == 103017 or  //加兰德 
                自身数据.手持 == 9815100 or  //加兰德
        自身数据.手持 == 103100 or  //mk12
        自身数据.手持 == 9815089 or  //mk12
                自身数据.手持 == 103014)
                {
                    // if (自瞄.喷子连点)
                    // {
                        // clickRegion(true,自瞄.连点位置);
                    // }
                    return true;
                }


                if (自身数据.手持 == 103011 or 自身数据.手持 == 103001 or 自身数据.手持 == 103003 or 自身数据.手持 == 103015 or 自身数据.手持 == 103012 or 自身数据.手持 == 103002) {
            // if (自身数据.开镜 != 0) {
                    return true;
            // }
                }
            }

        // 改为分状态判断距离
    if (自身数据.开镜) { // 开镜状态使用自瞄距离
        if (自瞄函数[自瞄.瞄准目标].距离 > 自瞄.自瞄距离限制) 
            return false;
    } 
    else { // 腰射状态使用腰射距离
        if (自瞄函数[自瞄.瞄准目标].距离 > 自瞄.腰射距离限制 && 
            自身数据.开火 != 0 && 
            自身数据.Fov > 75) 
            return false;
    }

    if (自瞄.狙击自瞄) {
        if (自身数据.手持 == 103011 or 自身数据.手持 == 103001 or 自身数据.手持 == 103003 or 自身数据.手持 == 103015 or 自身数据.手持 == 103012 or 自身数据.手持 == 103002) {
            if (自身数据.开镜 != 0) {
                return true;
            }
        }
    }
    switch ((int)自瞄.自瞄条件) {
        case 0:
            if (自身数据.开火 != 1) {
                return false;
            }
            break;
        case 1:
            if (自身数据.开镜 != 1) {
                return false;
            }
            break;
        case 2:
            if (自身数据.开火 == 0 && 自身数据.开镜 == 0) {
                return false;
            }
            break;
    }
    return true;
}

void 绘制::掩体线程() {
    std::thread*掩体线程 = new std::thread([&]{
        if (漏打开关 && 自瞄.初始化 && 自瞄.瞄准目标 != -1 && 自瞄函数[自瞄.瞄准目标].准心距离 <= 自瞄.自瞄范围) {
            for (int i = 0; i < 14; i++) {
                Shelter[i] = LineOfSightTo(自瞄函数[自瞄.瞄准目标].骨骼坐标[i]);
                if (Shelter[i]) {
                  break;
                }
                usleep(500);
            }
        }
    });
    掩体线程->detach();
}
void 绘制::自瞄主线程() {
    std::thread* 自瞄线程 = new std::thread([&] {
    bool isDown = false;    
    float halfSize = 自瞄.触摸范围 / 2;
    double RandomnumberX = 自瞄.触摸范围Y,RandomnumberY = 自瞄.触摸范围X;
    double tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
    
    if (自瞄.随机触摸点) {
      RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
      RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
      tx = RandomnumberX, ty = RandomnumberY;    
    }
    
    double ScreenX,ScreenY;
    if(displayInfo.orientation == 1 || displayInfo.orientation == 3){
       ScreenX = displayInfo.height; ScreenY = displayInfo.width;
    }else {
       ScreenX = displayInfo.width; ScreenY = displayInfo.height;
    }
    //double ScreenX = displayInfo.width, ScreenY = displayInfo.height;
    double ScrXH = ScreenX / 2.0f;
    double ScrYH = ScreenY / 2.0f;
    static float TargetX = 0;
    static float TargetY = 0;
    D3DVector obj;
    float NowCoor[3];
    float zm_x,zm_y;
    
    int 目标血量 = 100;
    string 目标名字;
    bool 自瞄测试 = false;
    int 数率 = 0;
    
    
    timer AimFPS;
    AimFPS.SetFps(120);
    AimFPS.AotuFPS_init();
    AimFPS.setAffinity();
    
    while (1)
    {
      if (!自瞄.随机触摸点) {
        RandomnumberX = 自瞄.触摸范围Y,RandomnumberY = 自瞄.触摸范围X;
      }
      if (!自瞄.初始化)
      {
        if (isDown == true)
        {
          tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
          if (自瞄.随机触摸点) {
            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
            tx = RandomnumberX, ty = RandomnumberY;
          }
          // 恢复变量
          Touch_Up(8);
          // 抬起
          isDown = false;
        }
        usleep(自瞄.自瞄速度 * 500);
        continue;
      }
      findminat();
      if (自瞄.瞄准目标 == -1)
      {
        if (isDown == true)
        {
          tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
          if (自瞄.随机触摸点) {
            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
            tx = RandomnumberX, ty = RandomnumberY;
          }
          // 恢复变量
          Touch_Up(8);
          // 抬起
          isDown = false;
        }
        usleep(自瞄.自瞄速度 * 500);
        continue;
      }
      float ToReticleDistance = 自瞄函数[自瞄.瞄准目标].准心距离;
      float BulletFlightTime = 自瞄函数[自瞄.瞄准目标].距离 / 自身数据.子弹速度;
      float FlyTime;
      float 预判力度a = 自瞄.预判力度;
      

      

      if(对象信息.敌人信息.乘坐载具) {
        预判力度a = 预判度.扫车;
      }else{
      // 5.56
      if (自身数据.手持id == 101004 && 预判度.m416 != 0)
        预判力度a = 预判度.m416;
      else if (自身数据.手持id == 101003 && 预判度.scar_l != 0)
         预判力度a = 预判度.scar_l;
      else if (自身数据.手持id == 101006 && 预判度.aug != 0)
         预判力度a = 预判度.aug;
      else if (自身数据.手持id == 101013 && 预判度.famas != 0)
         预判力度a = 预判度.famas;
      else if (自身数据.手持id == 101010 && 预判度.g36c != 0)
        预判力度a = 预判度.g36c;
      else if (自身数据.手持id == 105001 && 预判度.m249 != 0)
        预判力度a = 预判度.m249;
      // 7.62
      else if (自身数据.手持id == 101001 && 预判度.akm != 0)
        预判力度a = 预判度.akm;
      else if (自身数据.手持id == 101008 && 预判度.m762 != 0)
        预判力度a = 预判度.m762;
      else if (自身数据.手持id == 101012 && 预判度.蜜獾 != 0)
        预判力度a = 预判度.蜜獾;
      else if (自身数据.手持id == 105012 && 预判度.pkm != 0)
        预判力度a = 预判度.pkm;
      else if (自身数据.手持id == 105010 && 预判度.mg3!= 0)
        预判力度a = 预判度.mg3;
      else if (自身数据.手持id == 105013 && 预判度.mg_36 != 0)
        预判力度a = 预判度.mg_36;
      //冲锋枪
      else if (自身数据.手持id == 102105 && 预判度.p90 != 0)
        预判力度a = 预判度.p90;
      else if (自身数据.手持id == 102001 && 预判度.uzi != 0)
        预判力度a = 预判度.uzi;
      else if (自身数据.手持id == 102002 && 预判度.ump45 != 0)
        预判力度a = 预判度.ump45;
      else if (自身数据.手持id == 102003 && 预判度.vector != 0)
        预判力度a = 预判度.vector;
      else if (自身数据.手持id == 102004 && 预判度.汤姆逊 != 0)
        预判力度a = 预判度.汤姆逊;
      else if (自身数据.手持id == 102005 && 预判度.野牛 != 0)
        预判力度a = 预判度.野牛;
      //射手步枪
      else if (自身数据.手持id == 103006 && 预判度.mini14 != 0)
        预判力度a = 预判度.mini14;
      else if (自身数据.手持id == 103004 && 预判度.sks != 0)
        预判力度a = 预判度.sks;
      else if (自身数据.手持id == 103013 && 预判度.m417 != 0)
        预判力度a = 预判度.m417;
      else if (自身数据.手持id == 103014 && 预判度.mk20_h != 0)
        预判力度a = 预判度.mk20_h;
      else if (自身数据.手持id == 103100&& 预判度.mk12 != 0)
        预判力度a = 预判度.mk12;
      else if (自身数据.手持id == 103005&& 预判度.vss != 0)
        预判力度a = 预判度.vss;
      }

      if (自瞄函数[自瞄.瞄准目标].距离 >= 40) {
        FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.01f) * 预判力度a;
      }else {
        FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.0055f) * 预判力度a;
      }
      float DropM = 540.0f * BulletFlightTime * BulletFlightTime;
      float 压枪力度 = 自瞄.压枪力度;
      // 5.56
      if (自身数据.手持id == 101004 && 压枪力.m416 != 0)
        压枪力度 = 压枪力度 * 压枪力.m416;
      else if (自身数据.手持id == 101003 && 压枪力.scar_l != 0)
         压枪力度 = 压枪力度 * 压枪力.scar_l;
      else if (自身数据.手持id == 101006 && 压枪力.aug != 0)
         压枪力度 = 压枪力度 * 压枪力.aug;
      else if (自身数据.手持id == 101013 && 压枪力.famas != 0)
         压枪力度 = 压枪力度 * 压枪力.famas;
      else if (自身数据.手持id == 101010 && 压枪力.g36c != 0)
        压枪力度 = 压枪力度 * 压枪力.g36c;
      else if (自身数据.手持id == 105001 && 压枪力.m249 != 0)
        压枪力度 = 压枪力度 * 压枪力.m249;
      // 7.62
      else if (自身数据.手持id == 101001 && 压枪力.akm != 0)
        压枪力度 = 压枪力度 * 压枪力.akm;
      else if (自身数据.手持id == 101008 && 压枪力.m762 != 0)
        压枪力度 = 压枪力度 * 压枪力.m762;
      else if (自身数据.手持id == 101012 && 压枪力.蜜獾 != 0)
        压枪力度 = 压枪力度 * 压枪力.蜜獾;
      else if (自身数据.手持id == 105012 && 压枪力.pkm != 0)
        压枪力度 = 压枪力度 * 压枪力.pkm;
      else if (自身数据.手持id == 105010 && 压枪力.mg3 != 0)
        压枪力度 = 压枪力度 * 压枪力.mg3;
      else if (自身数据.手持id == 105013 && 压枪力.mg_36 != 0)
        压枪力度 = 压枪力度 * 压枪力.mg_36;
      //冲锋枪
      else if (自身数据.手持id == 102105 && 压枪力.p90 != 0)
        压枪力度 = 压枪力度 * 压枪力.p90;
      else if (自身数据.手持id == 102001 && 压枪力.uzi != 0)
        压枪力度 = 压枪力度 * 压枪力.uzi;
      else if (自身数据.手持id == 102002 && 压枪力.ump45 != 0)
        压枪力度 = 压枪力度 * 压枪力.ump45;
      else if (自身数据.手持id == 102003 && 压枪力.vector != 0)
        压枪力度 = 压枪力度 * 压枪力.vector;
      else if (自身数据.手持id == 102004 && 压枪力.汤姆逊 != 0)
        压枪力度 = 压枪力度 * 压枪力.汤姆逊;
      else if (自身数据.手持id == 102005 && 压枪力.野牛 != 0)
        压枪力度 = 压枪力度 * 压枪力.野牛;
      //射手步枪
      else if (自身数据.手持id == 103006 && 压枪力.mini14 != 0)
        压枪力度 = 压枪力度 * 压枪力.mini14;
      else if (自身数据.手持id == 103004 && 压枪力.sks != 0)
        压枪力度 = 压枪力度 * 压枪力.sks;
      else if (自身数据.手持id == 103013 && 压枪力.m417 != 0)
        压枪力度 = 压枪力度 * 压枪力.m417;
      else if (自身数据.手持id == 103014 && 压枪力.mk20_h != 0)
        压枪力度 = 压枪力度 * 压枪力.mk20_h;
      else if (自身数据.手持id == 103100 && 压枪力.mk12 != 0)
        压枪力度 = 压枪力度 * 压枪力.mk12;
      else if (自身数据.手持id == 103005 && 压枪力.vss != 0)
        压枪力度 = 压枪力度 * 压枪力.vss;
     
       if (自身数据.人物高度 == 120.0f) {
        压枪力度 = 自瞄.压枪力度-(Recoil(自身数据.手持)*自瞄.趴下位置调节);
      }


      NowCoor[0] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.X;
      NowCoor[1] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Y;
      NowCoor[2] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Z;
      obj.X = NowCoor[0] + (自瞄函数[自瞄.瞄准目标].人物向量.X * FlyTime);
      obj.Y = NowCoor[1] + (自瞄函数[自瞄.瞄准目标].人物向量.Y * FlyTime);
      obj.Z = NowCoor[2] + (自瞄函数[自瞄.瞄准目标].人物向量.Z * FlyTime) + DropM;
      if (自身数据.开火 == 1) {
        obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 压枪力度 * GetWeaponId(自身数据.手持);
      }

      // int 手持武器握把 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x3F60) + 0xA30) + 0x19d8);

      if (自身数据.手持握把 == 1082130432)
      {
        obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 轻型压枪力度 * GetWeaponId(自身数据.手持);
      }else if(自身数据.手持握把 == 1084227584){
        obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 拇指压枪力度 * GetWeaponId(自身数据.手持);
      }else if(自身数据.手持握把 == 1065353216){
        obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 垂直压枪力度 * GetWeaponId(自身数据.手持);
      }else if(自身数据.手持握把 == 1073741824){
        obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 直角压枪力度 * GetWeaponId(自身数据.手持);
      }

      D2DVector vpvp = 计算.计算屏幕坐标2(自身数据.矩阵, obj, PX, PY);

      float AimDs = sqrt(pow(PX - vpvp.X, 2) + pow(PY - vpvp.Y, 2));
      
      if(自瞄.动态自瞄 && (自身数据.开火==1 || 自身数据.开镜 == 1))
      {
        自瞄.动态范围 = AimDs;
      }else{
        自瞄.动态范围 = 自瞄.自瞄范围;
      }      
      zm_y = vpvp.X;
      zm_x = ScreenX - vpvp.Y;
      if (zm_x <= 0 || zm_x >= ScreenX || zm_y <= 0 || zm_y >= ScreenY)
      {
        if (isDown == true)
        {
          tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
          if (自瞄.随机触摸点) {
            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
            tx = RandomnumberX, ty = RandomnumberY;
          }
          // 恢复变量
          Touch_Up(8);
          // 抬起
          isDown = false;
        }
        usleep(自瞄.自瞄速度 * 500);
        continue;
      }
      if (ToReticleDistance <= 自瞄.自瞄范围)
      {
        
        if (!自瞄触发(自瞄函数[自瞄.瞄准目标].距离))
        {
          if (isDown == true)
          {
            tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
            if (自瞄.随机触摸点) {
              RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
              RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
              tx = RandomnumberX, ty = RandomnumberY;
            }
            // 恢复变量
            Touch_Up(8);
            isDown = false;
          }
          usleep(自瞄.自瞄速度 * 500);
          continue;
        }

        if (自瞄.框内自瞄) {
          if ((自瞄函数[自瞄.瞄准目标].人物向量.X == 0 and AimDs > 35) or (自瞄函数[自瞄.瞄准目标].人物向量.X != 0 and AimDs > 65))
          {
            if (isDown == true)
            {
              tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
              if (自瞄.随机触摸点) {
                RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                tx = RandomnumberX, ty = RandomnumberY;
              }
              // 恢复变量
              Touch_Up(8);
              isDown = false;
            }
            usleep(自瞄.自瞄速度 * 500);
            continue;
          }
        }
        
        if (自瞄.掉血自瞄) {
          if (数率 > 0) {
            数率++;
            if (数率 == (int)自瞄.掉血自瞄数率) {
              数率 = 0;
            }
          }else {
            if (自瞄函数[自瞄.瞄准目标].名字 == 目标名字) {
              if (自瞄函数[自瞄.瞄准目标].血量 >= 目标血量) {
                目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                continue;
              }else {
                数率++;
                目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
              }
            }else {
              目标名字 = 自瞄函数[自瞄.瞄准目标].名字;
              目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
              continue;
            }
          }
        }
        
        if (isDown == false)
        {
          if (自瞄.充电口方向 == 0)
            Touch_Down(8,(int)tx, (int)ty);
          else
            Touch_Down(8,displayInfo.height - (int)tx, displayInfo.width - (int)ty);
          isDown = true;
        }
        
        float Acc = getScopeAcc((int)(90 / 自身数据.Fov));        
        if (zm_x > ScrXH) {
          TargetX = -(ScrXH - zm_x) / 自瞄.自瞄速度 * Acc;
          if (TargetX + ScrXH > ScrXH * 2) {
            TargetX = 0;
          }
        }
        else if (zm_x < ScrXH) {
          TargetX = (zm_x - ScrXH) / 自瞄.自瞄速度 * Acc;
          if (TargetX + ScrXH < 0) {
            TargetX = 0;
          }
        }
        
        if (zm_y > ScrYH) {
          TargetY = -(ScrYH - zm_y) / 自瞄.自瞄速度 * Acc;
          if (TargetY + ScrYH > ScrYH * 2) {
            TargetY = 0;
          }
        }
        else if (zm_y < ScrYH) {
          TargetY = (zm_y - ScrYH) / 自瞄.自瞄速度 * Acc;
          if (TargetY + ScrYH < 0) {
            TargetY = 0;
          }
        }
        if (TargetY >= 35 || TargetX >= 35 || TargetY <= -35 || TargetX <= -35)
        {
          if (isDown == true)
          {
            tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
            if (自瞄.随机触摸点) {
              RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
              RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
              tx = RandomnumberX, ty = RandomnumberY;
            }
            // 恢复变量
            Touch_Up(8);
            isDown = false;
          }
          usleep(自瞄.自瞄速度 * 500);
          continue;
        }
        
        tx += TargetX;
        ty += TargetY;
        if (tx >= RandomnumberX + 自瞄.触摸范围 || tx <= RandomnumberX - 自瞄.触摸范围
        || ty >= RandomnumberY + 自瞄.触摸范围 || ty <= RandomnumberY - 自瞄.触摸范围)
        {
          tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
          if (自瞄.随机触摸点) {
            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
            tx = RandomnumberX, ty = RandomnumberY;
          }
          // 恢复变量
          Touch_Up(8);
          // 抬起
          
          if (自瞄.充电口方向 == 0)
            Touch_Down(8,(int)tx, (int)ty);
          else
            Touch_Down(8,displayInfo.height - (int)tx, displayInfo.width - (int)ty);         
          
          isDown = true;
        }
        if (自瞄.充电口方向 == 0)
          Touch_Move(8,(int)tx, (int)ty);
        else
          Touch_Move(8,displayInfo.height - (int)tx, displayInfo.width - (int)ty);
        isDown = true;
      }
      else
      {
        if (isDown == true)
        {
          tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
          if (自瞄.随机触摸点) {
            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
            tx = RandomnumberX, ty = RandomnumberY;
          }
          // 恢复变量
          Touch_Up(8);
          // 抬起
          isDown = false;
        }
      }
      usleep(自瞄.自瞄速度 * 500);
      AimFPS.SetFps(按钮.当前帧率);
      AimFPS.AotuFPS();
    } });
    自瞄线程->detach();
}
float 绘制::陀螺仪计算(float fov)
        {
            float 币子の陀螺仪预设 = 250.0f;
            if (fov > 75 && fov <= 130)
            {
                return 按钮.不开镜灵敏度 / 币子の陀螺仪预设;
            } else if (fov == 70 || fov == 75)
            {
                return 按钮.机瞄红点灵敏度 / 币子の陀螺仪预设;
            } else if (fov == 55 || fov == 60)
            {
                return 按钮.机瞄红点灵敏度 / 币子の陀螺仪预设;
            } else if ((int)fov == 44)
            {
                return 按钮.二倍灵敏度 / 币子の陀螺仪预设;
            } else if ((int)fov == 26)
            {
                return 按钮.三倍灵敏度 / 币子の陀螺仪预设;
            } else if ((int)fov == 20)
            {
                return 按钮.四倍灵敏度 / 币子の陀螺仪预设;
            } else if ((int)fov == 13)
            {
                return 按钮.六倍灵敏度 / 币子の陀螺仪预设;
            } else if ((int)fov == 11)
            {
                return 按钮.八倍灵敏度 / 币子の陀螺仪预设;
            }

            return 按钮.不开镜灵敏度 / 币子の陀螺仪预设;
        }

void 绘制::陀螺仪自瞄主线程() {
    std::thread *自瞄线程 = new std::thread([&]
                                            {
    
    
    while (!陀螺仪已初始化) {
        usleep(100000); 
    }
    
    printf("[+] 陀螺仪已就绪，自瞄功能启动\n");
    
    
    double ScreenX, ScreenY;
    if(displayInfo.orientation == 1 || displayInfo.orientation == 3){
       ScreenX = displayInfo.height; 
       ScreenY = displayInfo.width;
    } else {
       ScreenX = displayInfo.width; 
       ScreenY = displayInfo.height;
    }
    double ScrXH = ScreenX / 2.0f;
    double ScrYH = ScreenY / 2.0f;
    
    D3DVector obj;
    float NowCoor[3];
    float zm_x, zm_y;
    
    int 目标血量 = 100;
    string 目标名字;
    bool 自瞄测试 = false;
    int 数率 = 0;
    
    timer AimFPS;
    AimFPS.SetFps(120);
    AimFPS.AotuFPS_init();
    AimFPS.setAffinity();
    while (true)
    {
if (!自瞄.初始化) {
    
    陀螺仪->Send_Event(0, 0);
    陀螺仪->Unlock_Event();
    usleep(自瞄.自瞄速度 * 500);
    continue;  
}
if (自瞄.无目标压枪开关 && 自瞄.初始化)  {  
    if(自身数据.开火){
        
         
         float Acc = 自瞄.压枪力度;
        float 最终压枪力度 = (自瞄.基础下压力度 * -Acc * 0.1f);
        陀螺仪->Send_Event(最终压枪力度 ,0);
        usleep(20000);
        陀螺仪->Unlock_Event();  
        
        
        
        
    } else {
        
        陀螺仪->Send_Event(0, 0);
        陀螺仪->Unlock_Event();
    }
}
        findminat();
        if (自瞄.瞄准目标 == -1)
        {
            usleep(自瞄.自瞄速度 * 500);
            continue;
        }
        
        float ToReticleDistance = 自瞄函数[自瞄.瞄准目标].准心距离;
        float BulletFlightTime = 自瞄函数[自瞄.瞄准目标].距离 / 自身数据.子弹速度;
        float FlyTime;
        float 预判力度a = 自瞄.预判力度;
        
        if(对象信息.敌人信息.乘坐载具) {
            预判力度a = 预判度.扫车;
        } else {
            
            
            if (自身数据.手持id == 101004 && 预判度.m416 != 0)
                预判力度a = 预判度.m416;
            else if (自身数据.手持id == 101003 && 预判度.scar_l != 0)
                预判力度a = 预判度.scar_l;
            else if (自身数据.手持id == 101006 && 预判度.aug != 0)
                预判力度a = 预判度.aug;
            else if (自身数据.手持id == 101013 && 预判度.famas != 0)
                预判力度a = 预判度.famas;
            else if (自身数据.手持id == 101010 && 预判度.g36c != 0)
                预判力度a = 预判度.g36c;
            else if (自身数据.手持id == 105001 && 预判度.m249 != 0)
                预判力度a = 预判度.m249;
            
            else if (自身数据.手持id == 101001 && 预判度.akm != 0)
                预判力度a = 预判度.akm;
            else if (自身数据.手持id == 101008 && 预判度.m762 != 0)
                预判力度a = 预判度.m762;
            else if (自身数据.手持id == 101012 && 预判度.蜜獾 != 0)
                预判力度a = 预判度.蜜獾;
            else if (自身数据.手持id == 105012 && 预判度.pkm != 0)
                预判力度a = 预判度.pkm;
            else if (自身数据.手持id == 105010 && 预判度.mg3 != 0)
                预判力度a = 预判度.mg3;
            else if (自身数据.手持id == 105013 && 预判度.mg_36 != 0)
                预判力度a = 预判度.mg_36;
            
            else if (自身数据.手持id == 102105 && 预判度.p90 != 0)
                预判力度a = 预判度.p90;
            else if (自身数据.手持id == 102001 && 预判度.uzi != 0)
                预判力度a = 预判度.uzi;
            else if (自身数据.手持id == 102002 && 预判度.ump45 != 0)
                预判力度a = 预判度.ump45;
            else if (自身数据.手持id == 102003 && 预判度.vector != 0)
                预判力度a = 预判度.vector;
            else if (自身数据.手持id == 102004 && 预判度.汤姆逊 != 0)
                预判力度a = 预判度.汤姆逊;
            else if (自身数据.手持id == 102005 && 预判度.野牛 != 0)
                预判力度a = 预判度.野牛;
            
            else if (自身数据.手持id == 103006 && 预判度.mini14 != 0)
                预判力度a = 预判度.mini14;
            else if (自身数据.手持id == 103004 && 预判度.sks != 0)
                预判力度a = 预判度.sks;
            else if (自身数据.手持id == 103013 && 预判度.m417 != 0)
                预判力度a = 预判度.m417;
            else if (自身数据.手持id == 103014 && 预判度.mk20_h != 0)
                预判力度a = 预判度.mk20_h;
            else if (自身数据.手持id == 103100 && 预判度.mk12 != 0)
                预判力度a = 预判度.mk12;
            else if (自身数据.手持id == 103005 && 预判度.vss != 0)
                预判力度a = 预判度.vss;
        }
        
        if (自瞄函数[自瞄.瞄准目标].距离 >= 40) {
            FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.01f) * 预判力度a;
        } else {
            FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.0055f) * 预判力度a;
        }
        
        float DropM = 540.0f * BulletFlightTime * BulletFlightTime;
        float 压枪力度 = 自瞄.压枪力度;
        
        
        
        if (自身数据.手持id == 101004 && 压枪力.m416 != 0)
            压枪力度 = 压枪力度 * 压枪力.m416;
        else if (自身数据.手持id == 101003 && 压枪力.scar_l != 0)
            压枪力度 = 压枪力度 * 压枪力.scar_l;
        else if (自身数据.手持id == 101006 && 压枪力.aug != 0)
            压枪力度 = 压枪力度 * 压枪力.aug;
        else if (自身数据.手持id == 101013 && 压枪力.famas != 0)
            压枪力度 = 压枪力度 * 压枪力.famas;
        else if (自身数据.手持id == 101010 && 压枪力.g36c != 0)
            压枪力度 = 压枪力度 * 压枪力.g36c;
        else if (自身数据.手持id == 105001 && 压枪力.m249 != 0)
            压枪力度 = 压枪力度 * 压枪力.m249;
        
        else if (自身数据.手持id == 101001 && 压枪力.akm != 0)
            压枪力度 = 压枪力度 * 压枪力.akm;
        else if (自身数据.手持id == 101008 && 压枪力.m762 != 0)
            压枪力度 = 压枪力度 * 压枪力.m762;
        else if (自身数据.手持id == 101012 && 压枪力.蜜獾 != 0)
            压枪力度 = 压枪力度 * 压枪力.蜜獾;
        else if (自身数据.手持id == 105012 && 压枪力.pkm != 0)
            压枪力度 = 压枪力度 * 压枪力.pkm;
        else if (自身数据.手持id == 105010 && 压枪力.mg3 != 0)
            压枪力度 = 压枪力度 * 压枪力.mg3;
        else if (自身数据.手持id == 105013 && 压枪力.mg_36 != 0)
            压枪力度 = 压枪力度 * 压枪力.mg_36;
        
        else if (自身数据.手持id == 102105 && 压枪力.p90 != 0)
            压枪力度 = 压枪力度 * 压枪力.p90;
        else if (自身数据.手持id == 102001 && 压枪力.uzi != 0)
            压枪力度 = 压枪力度 * 压枪力.uzi;
        else if (自身数据.手持id == 102002 && 压枪力.ump45 != 0)
            压枪力度 = 压枪力度 * 压枪力.ump45;
        else if (自身数据.手持id == 102003 && 压枪力.vector != 0)
            压枪力度 = 压枪力度 * 压枪力.vector;
        else if (自身数据.手持id == 102004 && 压枪力.汤姆逊 != 0)
            压枪力度 = 压枪力度 * 压枪力.汤姆逊;
        else if (自身数据.手持id == 102005 && 压枪力.野牛 != 0)
            压枪力度 = 压枪力度 * 压枪力.野牛;
        
        else if (自身数据.手持id == 103006 && 压枪力.mini14 != 0)
            压枪力度 = 压枪力度 * 压枪力.mini14;
        else if (自身数据.手持id == 103004 && 压枪力.sks != 0)
            压枪力度 = 压枪力度 * 压枪力.sks;
        else if (自身数据.手持id == 103013 && 压枪力.m417 != 0)
            压枪力度 = 压枪力度 * 压枪力.m417;
        else if (自身数据.手持id == 103014 && 压枪力.mk20_h != 0)
            压枪力度 = 压枪力度 * 压枪力.mk20_h;
        else if (自身数据.手持id == 103100 && 压枪力.mk12 != 0)
            压枪力度 = 压枪力度 * 压枪力.mk12;
        else if (自身数据.手持id == 103005 && 压枪力.vss != 0)
            压枪力度 = 压枪力度 * 压枪力.vss;
        
        if (自身数据.人物高度 == 120.0f) {
            压枪力度 = 自瞄.压枪力度 - (Recoil(自身数据.手持) * 自瞄.趴下位置调节);
        }
        
        NowCoor[0] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.X;
        NowCoor[1] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Y;
        NowCoor[2] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Z;
        obj.X = NowCoor[0] + (自瞄函数[自瞄.瞄准目标].人物向量.X * FlyTime);
        obj.Y = NowCoor[1] + (自瞄函数[自瞄.瞄准目标].人物向量.Y * FlyTime);
        obj.Z = NowCoor[2] + (自瞄函数[自瞄.瞄准目标].人物向量.Z * FlyTime) + DropM;
        
        
        if (自身数据.开火 == 1) {
            obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 压枪力度 * GetWeaponId(自身数据.手持);
        }
        
        if (自身数据.手持握把 == 1082130432) 
        {
            obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 轻型压枪力度 * GetWeaponId(自身数据.手持);
        } else if(自身数据.手持握把 == 1084227584) { 
            obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 拇指压枪力度 * GetWeaponId(自身数据.手持);
        } else if(自身数据.手持握把 == 1065353216) { 
            obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 垂直压枪力度 * GetWeaponId(自身数据.手持);
        } else if(自身数据.手持握把 == 1073741824) { 
            obj.Z += 自瞄函数[自瞄.瞄准目标].距离 * 直角压枪力度 * GetWeaponId(自身数据.手持);
        }
        
        D2DVector vpvp = 计算.计算屏幕坐标2(自身数据.矩阵, obj, PX, PY);
        float AimDs = sqrt(pow(PX - vpvp.X, 2) + pow(PY - vpvp.Y, 2));
        
        if(自瞄.动态自瞄 && (自身数据.开火==1 || 自身数据.开镜 == 1))
        {
            自瞄.动态范围 = AimDs;
        } else {
            自瞄.动态范围 = 自瞄.自瞄范围;
        }
        
        if (vpvp.X < 0 || vpvp.X > ScreenY ||
            vpvp.Y < 0 || vpvp.Y > ScreenX)
        {
            usleep(自瞄.自瞄速度 * 500);
            continue;
        }
        
        if (ToReticleDistance <= 自瞄.自瞄范围 && 陀螺仪已初始化)
        {
            if (!自瞄触发(自瞄函数[自瞄.瞄准目标].距离))
            {
                usleep(自瞄.自瞄速度 * 500);
                continue;
            }
            
            if (自瞄.掉血自瞄) {
                if (数率 > 0) {
                    数率++;
                    if (数率 == (int)自瞄.掉血自瞄数率) {
                        数率 = 0;
                    }
                } else {
                    if (自瞄函数[自瞄.瞄准目标].名字 == 目标名字) {
                        if (自瞄函数[自瞄.瞄准目标].血量 >= 目标血量) {
                            目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                            continue;
                        } else {
                            数率++;
                            目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                        }
                    } else {
                        目标名字 = 自瞄函数[自瞄.瞄准目标].名字;
                        目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                        continue;
                    }
                }
            }
            
                            
                        float Acc = 陀螺仪计算(自身数据.Fov);
                        float TargetX = 0.0f;
                        float TargetY = 0.0f;
                        if (zm_x > ScrXH) {
                            TargetX = -(ScrXH - zm_x) / 自瞄.自瞄速度 * Acc;
                        } else if (zm_x < ScrXH) {
                            TargetX = (zm_x - ScrXH) / 自瞄.自瞄速度 * Acc;
                        }
                        if (zm_y > ScrYH) {
                            TargetY = -(ScrYH - zm_y) / 自瞄.自瞄速度 * Acc;
                        } else if (zm_y < ScrYH) {
                            TargetY = (zm_y - ScrYH) / 自瞄.自瞄速度 * Acc;
                        }
            
            float xxx = vpvp.X - (ScreenY / 2);
            float yyy = vpvp.Y - (ScreenX / 2);
            xxx *= Acc;
            yyy *= Acc;
            xxx *= 自瞄.自瞄速度;
            yyy *= 自瞄.自瞄速度;
            
            if (!自瞄.充电口方向)
            {
                xxx = -xxx;
                yyy = -yyy;
            }
            if (陀螺仪 && 自瞄.初始化)
            {
                
                float 陀螺仪X = -(xxx * 0.002);
                float 陀螺仪Y = yyy * 0.002;
                
                陀螺仪->Send_Event(陀螺仪Y, 陀螺仪X);
                
                
                usleep(20000); 
                
                
                陀螺仪->Unlock_Event();
                
                
                static int 调试计数 = 0;
                // if (调试计数++ % 50 == 0) {
                    // gyro_infor 陀螺仪信息 = 陀螺仪->GetGyroInfo();
                    // printf("[陀螺仪] X=%.3f, Y=%.3f, 启用=%d\n", 
                           // 陀螺仪信息.x, 陀螺仪信息.y, 陀螺仪信息.enable);
                // }
            }
}
else
{
    
    if (陀螺仪 && 陀螺仪已初始化)
    {
        陀螺仪->Send_Event(0, 0);  
        usleep(20000);
        陀螺仪->Unlock_Event();
    }
}
        
        usleep(自瞄.自瞄速度 * 500);
        AimFPS.SetFps(按钮.当前帧率);
        AimFPS.AotuFPS();
    } });
    自瞄线程->detach();
}
int 绘制::Acquisitionsite() {
    for (int i = 0; i < 14; i++) {
        if (Shelter[i]) {
            return i;
        }
    }
    return 15;
}
D3DVector PovLocation;  
FRotator PovRotator;

void 绘制::更新地址数据() {
地址.自身地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4+0xf1fb900) + 0x810) + 0x78) + 0x38) + 0x78) + 0x30) + 0x28c8);
// 9D0矩阵: 0xf1d5f70)+ 0xC0) + 0x9D0;
// 590矩阵: 0xf1d5f70)+ 0xC0) + 0x590;
地址.矩阵地址 = 读写.getPtr64(读写.getPtr64(地址.libue4+0xf1d5f70) + 0xC0) + 0x590;
地址.矩阵地址_Tol = 读写.getPtr64(读写.getPtr64(地址.libue4+0x16AF5510) + 0x98) + 0x10440;
  if (按钮.解密)
	{
            地址.世界地址 = 读写.getPtr64(读写.getPtr64(地址.libue4+0xf1d5f70) + 0xb0); 

		地址.数组地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0xecf15c0) + 0xF8) + 0x190) + 0xF0);
		世界数量 = 读写.getDword(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0xf1d5f70) + 0xf8) + 0x190) + 0xf8);
	}
	else
	{
	    地址.世界地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0xf1fb900) + 0x810) + 0x78);
	    uintptr_t Uleve = 读写.getPtr64(地址.世界地址 + 0x30);
        地址.数组地址 = 读写.getPtr64(Uleve + 0xA0);
        世界数量 = 读写.getDword(Uleve + 0xA8);
	}
    if (解密数组) {
        地址.数组地址 = 解密数组;
        世界数量 = 读写.getDword(地址.世界地址 + 0xB8);
    }
    
    地址.类地址 = 读写.getPtr64(读写.getPtr64(地址.libue4 + 0xec73720) + 0x110);
读写.read(地址.矩阵地址_Tol - 0x208, &自身数据.坐标, sizeof(自身数据.坐标));
读写.read(读写.getPtr64(地址.自身地址 + 0x208) + 0x1c8, &自身数据.坐标, sizeof(自身数据.坐标));
  读写.read(地址.矩阵地址, &自身数据.矩阵, sizeof(自身数据.矩阵));             
  读写.read(地址.矩阵地址_Tol, &自身数据.矩阵, sizeof(自身数据.矩阵));             
自身数据.自身队伍 = 读写.getDword(地址.自身地址+0x998);
自身数据.自身状态 = 读写.getDword(读写.getPtr64(地址.自身地址+0x1058));/**/
  
自身数据.开镜 = 读写.getDword(地址.自身地址+0x1134);/**/
  
自身数据.开火 = 读写.getDword(地址.自身地址+0x1830);/**/
  
自身数据.手持id =读写.getDword(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x2608) + 0x5d8) + 0x1370) + 0x1e0);

  自身数据.手持 = heldconversion(自身数据.手持id);

  自身数据.Fov = 读写.getFloat(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x4b18) + 0x548) + 0x544);  //554
  自身数据.子弹速度 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x2608) + 0x5d8) + 0x1370) + 0x560);
  自身数据.后坐力数据 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x2608) + 0x5d8) + 0x1370) + 0xbd0);
自身数据.手持握把 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x1158) + 0xc78) + 0x5040);/**/
    
    
    
自身数据.头 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.自身地址+0x408)+0x50)+0x210)+0x5e8)+0x18)+0x58);
  
自身数据.甲 = 读写.getFloat(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.自身地址+0x408)+0x50)+0x210)+0x5e8)+0x18)-0x58);  
  
自身数据.准星Y = 读写.getFloat(读写.getPtr64(地址.自身地址 + 0x4b18) + 0x4e8) - 90; 
  自身数据.人物高度 = 读写.getFloat(地址.自身地址 + 0x116c);  

地址.全图人数 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.libue4 + 0xf1d5f70) +0x428) + 0xbb4);
 地址.真实玩家 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.libue4 + 0xf1d5f70) +0x428) + 0xaa8);
 地址.队伍数 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.libue4 + 0xf1d5f70) +0x428) + 0xbb8);
    
    读写.read(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x4b18) + 0x548) + 0x520 + 0x10, &PovLocation, sizeof(PovLocation));//视角位置
    读写.read(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x4b18) + 0x548) + 0x554, &PovRotator, sizeof(PovRotator));//视角旋转器
/*模型漏打*/
    
}
ImColor 绘制::floatArrToImColor(float arr[4]) {
    return ImColor(arr[0] * 255, arr[1] * 255, arr[2] * 255, arr[3] * 255);
}

void 绘制::更新对象数据() {
    if (自瞄.触摸位置)
        绘图.绘制自瞄触摸范围(自瞄.触摸范围, 自瞄.触摸范围X, 自瞄.触摸范围Y);
    if (!自瞄.动态自瞄) {
        自瞄.动态范围 = 自瞄.自瞄范围;
    }
    if (自瞄.初始化 && !自瞄.隐藏自瞄圈) {
        ImGui::GetForegroundDrawList()->AddCircle({PX, PY}, 自瞄.动态范围, ImColor(255, 255, 255, 255), 0, 1.0f);
    }
if (按钮.雷达)
{
    ImVec2 币子の雷达坐标 = {按钮.雷达X, 按钮.雷达Y};
    float 币子の雷达大小 = 按钮.雷达大小 *100;
    
    // 使用自身视角的Yaw值作为雷达旋转角度（转换为弧度）
    float 币子の雷达旋转角度 = IM_PI * 自身数据.准星Y / 180.0f;
    
    ImGui::GetForegroundDrawList()->AddCircleFilled(币子の雷达坐标, 币子の雷达大小, ImColor(0, 0, 0, 55));
    ImGui::GetForegroundDrawList()->AddCircle(币子の雷达坐标, 币子の雷达大小, ImColor(255, 255, 255, 255));
    
    // 绘制十字线（根据视角旋转）
    float lineLength = 币子の雷达大小;
    
    // 计算旋转后的十字线端点
    ImVec2 crossRight = {
        币子の雷达坐标.x + lineLength * cosf(币子の雷达旋转角度),
        币子の雷达坐标.y + lineLength * sinf(币子の雷达旋转角度)
    };
    ImVec2 crossLeft = {
        币子の雷达坐标.x - lineLength * cosf(币子の雷达旋转角度),
        币子の雷达坐标.y - lineLength * sinf(币子の雷达旋转角度)
    };
    ImVec2 crossUp = {
        币子の雷达坐标.x + lineLength * cosf(币子の雷达旋转角度 + IM_PI/2),
        币子の雷达坐标.y + lineLength * sinf(币子の雷达旋转角度 + IM_PI/2)
    };
    ImVec2 crossDown = {
        币子の雷达坐标.x - lineLength * cosf(币子の雷达旋转角度 + IM_PI/2),
        币子の雷达坐标.y - lineLength * sinf(币子の雷达旋转角度 + IM_PI/2)
    };
    
    ImGui::GetForegroundDrawList()->AddLine(crossLeft, crossRight, ImColor(255, 255, 255, 128), 2);
    ImGui::GetForegroundDrawList()->AddLine(crossUp, crossDown, ImColor(255, 255, 255, 128), 2);
    
    // 绘制45度斜线（可选）
    float lineAngle1 = 币子の雷达旋转角度 + 0.25f * IM_PI;
    float lineAngle2 = 币子の雷达旋转角度 - 0.25f * IM_PI;
    ImVec2 line1Start(币子の雷达坐标.x + 币子の雷达大小 * cos(lineAngle1), 
                    币子の雷达坐标.y + 币子の雷达大小 * sin(lineAngle1));
    ImVec2 line1End(币子の雷达坐标.x - 币子の雷达大小 * cos(lineAngle1), 
                  币子の雷达坐标.y - 币子の雷达大小 * sin(lineAngle1));
    ImVec2 line2Start(币子の雷达坐标.x + 币子の雷达大小 * cos(lineAngle2), 
                    币子の雷达坐标.y + 币子の雷达大小 * sin(lineAngle2));
    ImVec2 line2End(币子の雷达坐标.x - 币子の雷达大小 * cos(lineAngle2), 
                  币子の雷达坐标.y - 币子の雷达大小 * sin(lineAngle2));
    ImGui::GetForegroundDrawList()->AddLine(line1Start, line1End, ImColor(255, 255, 255, 128), 2);
    ImGui::GetForegroundDrawList()->AddLine(line2Start, line2End, ImColor(255, 255, 255, 128), 2);
    
    // 绘制方向指示器（根据视角）
    ImVec2 directionIndicator = ImVec2(币子の雷达坐标.x + 币子の雷达大小 * cos(币子の雷达旋转角度), 
                                   币子の雷达坐标.y + 币子の雷达大小 * sin(币子の雷达旋转角度));
    ImGui::GetForegroundDrawList()->AddLine(币子の雷达坐标, directionIndicator, ImColor(255, 255, 255, 255), 3);
    
    // 绘制方向文字（根据视角旋转）
    const float 币子の雷达大小Set = 币子の雷达大小 + 20.0f;
    const ImVec4 币子の雷达字体颜色 = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    const float 币子の雷达字体大小 = 22.0f;
    ImFont* font = ImGui::GetFont();
    font->Scale = 币子の雷达字体大小 / font->FontSize;
    
    const char* 币子の雷达方向文字[] = {"北", "东北", "东", "东南", "南", "西南", "西", "西北"};
    
    for (int i = 0; i < 8; i++)
    {
        // 计算每个方向的角度（相对于北方）
        float directionAngle = IM_PI * 2.0f * (float)i / 8.0f - IM_PI/2.0f;
        
        // 应用自身视角旋转
        float finalAngle = directionAngle + 币子の雷达旋转角度;
        
        ImVec2 pos = {
            币子の雷达坐标.x + cosf(finalAngle) * 币子の雷达大小Set, 
            币子の雷达坐标.y + sinf(finalAngle) * 币子の雷达大小Set
        };
        
        ImVec2 edge_pos = {
            币子の雷达坐标.x + cosf(finalAngle) * 币子の雷达大小, 
            币子の雷达坐标.y + sinf(finalAngle) * 币子の雷达大小
        };
        
        ImVec2 币子の雷达整体文字坐标 = font->CalcTextSizeA(币子の雷达字体大小, FLT_MAX, 0.0f, 币子の雷达方向文字[i]);
        pos.x -= 币子の雷达整体文字坐标.x * 0.5f;
        pos.y -= 币子の雷达整体文字坐标.y * 0.5f;
        
        ImGui::GetForegroundDrawList()->AddLine(
            {pos.x + 币子の雷达整体文字坐标.x * 0.5f, pos.y + 币子の雷达整体文字坐标.y * 0.5f}, 
            edge_pos, ImColor(255, 255, 255, 128), 1);
        
        ImGui::GetForegroundDrawList()->AddText(font, 币子の雷达字体大小, pos, ImColor(币子の雷达字体颜色), 币子の雷达方向文字[i]);
    }
    font->Scale = 1.0f;
    
    // 绘制敌人位置（需要根据视角旋转调整敌人坐标）
    if (对象信息.敌人信息.距离 <= 按钮.雷达视距) 
    {
        string 距离文本 = to_string((int)对象信息.敌人信息.距离);
        
        // 将敌人相对坐标根据视角旋转
        float rotatedX = 对象信息.敌人信息.雷达.X * cosf(币子の雷达旋转角度) - 对象信息.敌人信息.雷达.Y * sinf(币子の雷达旋转角度);
        float rotatedY = 对象信息.敌人信息.雷达.X * sinf(币子の雷达旋转角度) + 对象信息.敌人信息.雷达.Y * cosf(币子の雷达旋转角度);
        
        ImVec2 敌人位置 = {币子の雷达坐标.x + rotatedX, 币子の雷达坐标.y + rotatedY};
        
        int 队伍编号 = 对象信息.敌人信息.队伍 % 100;
        float 色相 = (队伍编号 * 3.6f) / 360.0f;        
        ImColor 填充颜色 = ImColor::HSV(色相, 0.8f, 1.0f);
        ImColor 描边颜色 = ImColor(0, 0, 0, 255);
        
        if (对象信息.敌人信息.isboot == 1) 
        {
            填充颜色 = ImColor(255, 255, 255, 255);
        }
        
        ImGui::GetForegroundDrawList()->AddCircle(敌人位置, 9, 描边颜色, 0, 1.0f);
        ImGui::GetForegroundDrawList()->AddCircleFilled(敌人位置, 8, 填充颜色);
        
        ImVec2 文本尺寸 = ImGui::CalcTextSize(距离文本.c_str());
        ImVec2 文字位置 = {敌人位置.x - 文本尺寸.x / 2, (敌人位置.y - 文本尺寸.y / 2) + 15};
        ImColor 文本颜色 = (填充颜色.Value.x + 填充颜色.Value.y + 填充颜色.Value.z > 1.5f) ? 
                         ImColor(255, 0, 0, 255) : ImColor(255, 255, 255, 255);
        
        绘图.绘制字体描边(14, 文字位置.x, 文字位置.y, 文本颜色, 距离文本.c_str());
    }
}
/*雷达第一部分*/
    int 绘制人机 = 0, 绘制真人 = 0;
    被瞄准对象数量 = 0;
    自瞄.瞄准对象数量 = 0;
    if (按钮.显示全图信息) {
// 地址.全图人数 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x1373A758) + 0x9b0) + 0x11cc);
// 地址.真实玩家 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x1373A758) + 0x9b0) + 0x11d0);
// 地址.队伍数 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x1373A758) + 0x9b0) + 0x123c);
    // 创建绘制列表引用
                    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

    // 设置文本位置和颜色
                    ImVec2 text_pos = ImVec2(10 + 1050, 200);
    ImU32 text_color = IM_COL32(255, 255, 255, 255);  // 白色文字

    // 直接绘制文本到屏幕
                    draw_list->AddText(text_pos, text_color,
                    ("存活真实玩家: " + std::to_string(地址.真实玩家)).c_str());

                    draw_list->AddText(ImVec2(text_pos.x, text_pos.y + ImGui::GetTextLineHeight()),
                    text_color,
                    ("存活队伍数: " + std::to_string(地址.队伍数)).c_str());

                    draw_list->AddText(ImVec2(text_pos.x, text_pos.y + ImGui::GetTextLineHeight() * 2),
                    text_color,
                    ("全图人数(人机 + 真人): " + std::to_string(地址.全图人数)).c_str());
                }
    for (int a = 0; a < 世界数量; a++) {
        对象地址.敌人地址 = 读写.getPtr64(地址.数组地址 + a * 8);
// 读写.read(读写.getPtr64(对象地址.敌人地址 + 0x260) + 0x200, &对象信息.敌人信息.坐标, sizeof(对象信息.敌人信息.坐标));
           bool 闪框解密 = 按钮.防闪框;
        if(闪框解密){
    
读写.read(读写.getPtr64(对象地址.敌人地址 + 0x260) + 0x600, &对象信息.敌人信息.坐标, sizeof(对象信息.敌人信息.坐标));
       } else {
读写.read(读写.getPtr64(对象地址.敌人地址 + 0x208) + 0x1c8, &对象信息.敌人信息.坐标, sizeof(对象信息.敌人信息.坐标));
        }

        对象信息.敌人信息.距离 = 计算.计算距离(自身数据.坐标, 对象信息.敌人信息.坐标);                                          // 距离

        float camear_r = 自身数据.矩阵[3] * 对象信息.敌人信息.坐标.X + 自身数据.矩阵[7] * 对象信息.敌人信息.坐标.Y + 自身数据.矩阵[11] * 对象信息.敌人信息.坐标.Z + 自身数据.矩阵[15];
        float r_x = PX + (自身数据.矩阵[0] * 对象信息.敌人信息.坐标.X + 自身数据.矩阵[4] * 对象信息.敌人信息.坐标.Y + 自身数据.矩阵[8] * 对象信息.敌人信息.坐标.Z + 自身数据.矩阵[12]) / camear_r * PX;
        float r_y = PY - (自身数据.矩阵[1] * 对象信息.敌人信息.坐标.X + 自身数据.矩阵[5] * 对象信息.敌人信息.坐标.Y + 自身数据.矩阵[9] * (对象信息.敌人信息.坐标.Z - 5) + 自身数据.矩阵[13]) / camear_r * PY;
        float r_z = PY - (自身数据.矩阵[1] * 对象信息.敌人信息.坐标.X + 自身数据.矩阵[5] * 对象信息.敌人信息.坐标.Y + 自身数据.矩阵[9] * (对象信息.敌人信息.坐标.Z + 205) + 自身数据.矩阵[13]) / camear_r * PY;
        D4DVector t_屏幕坐标 = {r_x - (r_y - r_z) / 4, r_y, (r_y - r_z) / 2, r_y - r_z};

        float X = r_x - (r_y - r_z) / 4;
        float Y = r_y;
        float W = (r_y - r_z) / 2;
        float MIDDLE = X + W / 2;
        float BOTTOM = Y + W;
        float TOP = Y - W;
                    char 计算地址[256] = "我是帅哥";
                    sprintf(计算地址, "%lx", 对象地址.敌人地址);
                    if (按钮.手雷预警)
                    {
                        int 手雷ID = 读写.getDword(对象地址.敌人地址 + 0x56c); 
                        const char *投掷物信息 = Getagrenade(手雷ID);
                        if (手雷ID == 602004 or 手雷ID == 9825004)
                        {
                            if (!计时器.hasTimer(计算地址))
                            {
                                手雷类.add(计算地址, 对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y);
                            }
                            else
                            {
                                手雷类.remove(计算地址);
                            }
                        }
                        if (投掷物信息 != nullptr)
                        {
                            std::string name = 投掷物信息;
                            if (t_屏幕坐标.W > 0)
                            {
                            // int 手雷状态 = 读写.getDword(对象地址.敌人地址 + 0x5F4);
// if (手雷状态 == 1) continue;  // 爆炸后跳过绘制
                                float 计算_max = 7 - (计时器.getTimerSeconds(计算地址) / 7 * (60 / ImGui::GetIO().Framerate * 0.115));
                                绘图::VecTor3 坐标 = {对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y, 对象信息.敌人信息.坐标.Z} ;
                                if (计算_max >= 0)
                                {
                                    name += "[" + std::to_string((int)对象信息.敌人信息.距离) + "米]";
                                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 35);
                                    if (手雷ID != 602004 && 手雷ID != 9825004)
                                    {
                                        ImGui::GetForegroundDrawList()->AddText(NULL, 22, {r_x - (textSize.x / 2), r_y + 30} , ImColor(255, 0, 0, 255), name.c_str());
                                    }
                                    if (手雷ID == 602004 or 手雷ID == 9825004)
                                    {
                                        float 计时 = 计算_max / 7 * 100;
                                        float aa = 计时 * 3.6;
                                        ImGui::GetForegroundDrawList()->AddCircleArc( {r_x, r_y} , 30, {0, 100 * 3.6} , ImColor(255, 255, 255, 100), 0, 5);
                                        ImGui::GetForegroundDrawList()->AddCircleArc( {r_x, r_y} , 30, {0, aa} , ImColor(0, 255, 0, 255), 0, 5);

                                        string 计算 = std::to_string((int)计算_max);
                                        计算 += "s";
                                        textSize = ImGui::CalcTextSize(计算.c_str(), 0, 22);
                                        ImGui::GetForegroundDrawList()->AddText(NULL, 22, {r_x - (textSize.x / 2), r_y - 22} , ImColor(255, 0, 0, 255), 计算.c_str());

                                        string 距离 = std::to_string((int)对象信息.敌人信息.距离);
                                        距离 += "米";
                                        textSize = ImGui::CalcTextSize(距离.c_str(), 0, 22);
                                        ImGui::GetForegroundDrawList()->AddText(NULL, 22, {r_x - (textSize.x / 2), r_y + 20} , ImColor(0, 255, 0, 255), 距离.c_str());

                                        绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(255, 0, 0), 210, 1.5f, 自身数据.矩阵);
                                        绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(255, 255, 0), 400, 1.5f, 自身数据.矩阵);
                                        绘图.Parabola(坐标, 自身数据.矩阵);
                                    }
                                    if (手雷ID == 602001 or 手雷ID == 9825001)
                                    {
                                        string 距离 = std::to_string((int)对象信息.敌人信息.距离);
                                        距离 += "米";
                                        textSize = ImGui::CalcTextSize(距离.c_str(), 0, 22);
                                        ImGui::GetForegroundDrawList()->AddText(NULL, 22, {r_x - (textSize.x / 2), r_y + 20} , ImColor(0, 255, 0, 255), 距离.c_str());

                                        绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(255, 255, 255), 180, 1.5f, 自身数据.矩阵);
                                        绘图.Parabola(坐标, 自身数据.矩阵);
                                    }
                                    if (手雷ID == 602003 or 手雷ID == 9825003)
                                    {
                                        string 距离 = std::to_string((int)对象信息.敌人信息.距离);
                                        距离 += "米";
                                        textSize = ImGui::CalcTextSize(距离.c_str(), 0, 22);
                                        ImGui::GetForegroundDrawList()->AddText(NULL, 22, {r_x - (textSize.x / 2), r_y + 20} , ImColor(0, 255, 0, 255), 距离.c_str());

                                        绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(255, 0, 255), 485, 1.5f, 自身数据.矩阵);
                                        绘图.Parabola(坐标, 自身数据.矩阵);
                                    }
                                    if (手雷ID == 602075)
                                    {
                                        string 距离 = std::to_string((int)对象信息.敌人信息.距离);
                                        距离 += "米";
                                        textSize = ImGui::CalcTextSize(距离.c_str(), 0, 22);
                                        ImGui::GetForegroundDrawList()->AddText(NULL, 22, {r_x - (textSize.x / 2), r_y + 20} , ImColor(0, 255, 0, 255), 距离.c_str());

                                        绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(128, 0, 128), 80, 1.5f, 自身数据.矩阵);
                                        绘图.Parabola(坐标, 自身数据.矩阵);
                                    }
                                }
                            }
                        }
                    }
        char ClassName[64] = "";
        char 对象信息_max[200] = "";
        int ClassID = 读写.getPtr64(对象地址.敌人地址 + 24);
        long int FNameEntry;

        if (t_屏幕坐标.W > 0) {  // 车辆物资区域
            FNameEntry = 读写.getPtr64(读写.getPtr64(地址.类地址 + (ClassID / 0x4000) * 0x8) + (ClassID % 0x4000) * 0x8);
            读写.read(FNameEntry + 0xC, ClassName, 64);
            if (按钮.Debug) {
                if (按钮.Debug模式 == 0) {
                    auto textSize = ImGui::CalcTextSize(ClassName, 0, 30);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 255, 255), ClassName);
                } else {
                    auto textSize = ImGui::CalcTextSize(计算地址, 0, 30);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 255, 255), 计算地址);
                }
            }
            if (按钮.车辆) {
    long long VehicleData = 读写.getPtr64(对象地址.敌人地址 + 0xeb0); 
    float 载具血量 = 读写.getFloat(VehicleData + 0x350) / 读写.getFloat(VehicleData + 0x354) * 100; 
    float 载具油量 = 读写.getFloat(VehicleData + 0x438) / 读写.getFloat(VehicleData + 0x43c) * 100;				 
    // if ((int)载具血量 != 0 && (int)载具油量 != 0 && 载具油量 <= 100 && 载具血量 <= 100 && 载具油量 >= 0 && 载具血量 >= 0) {
        std::string name = getMaterialName(ClassName);
        if (name != "Error" && 读写.getPtr64(地址.自身地址 + 0x1B8) != 对象地址.敌人地址 && 对象信息.敌人信息.距离 > 5) {
            name += std::to_string((int)对象信息.敌人信息.距离) + "米";
            auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 20);
            ImColor color = ImColor(static_cast<int>(车辆颜色[0] * 255 + 0.5), static_cast<int>(车辆颜色[1] * 255 + 0.5), static_cast<int>(车辆颜色[2] * 255 + 0.5), static_cast<int>(车辆颜色[3] * 255 + 0.5));
            ImGui::GetForegroundDrawList()->AddText(NULL, 20, {r_x - (textSize.x / 2), r_y}, color, name.c_str());

            // 绘制载具血量油量
            if (按钮.载具血量) {
                // 血条背景框
                ImGui::GetForegroundDrawList()->AddRect({r_x - 60, r_y + 25 + 2}, {r_x - 60 + 60, r_y + 25 - 2}, ImColor(0, 0, 0, 255), 0.0f, 0, 2.0f);

                // 血量填充（绿色）
                ImGui::GetForegroundDrawList()->AddRectFilled({r_x - 60, r_y + 25 + 2}, {r_x - 60 + 载具血量 * 0.6, r_y + 25 - 2}, ImColor(0, 255, 0, 255), 0.0f);

                // 血条分割线（5条黑色分割线）
                for (int i = 1; i <= 5; ++i) {
                    float lineX = r_x - 60 + i * 12;
                    ImGui::GetForegroundDrawList()->AddLine({lineX, r_y + 25 + 2}, {lineX, r_y + 25 - 2}, ImColor(0, 0, 0, 255), 1.0f);
                }
            }

            if (按钮.载具油量) {
                // 油条背景框
                ImGui::GetForegroundDrawList()->AddRect({r_x - 60, r_y + 37 + 2}, {r_x - 60 + 60, r_y + 37 - 2}, ImColor(0, 0, 0, 255), 0.0f, 0, 2.0f);

                // 油量填充（白色）
                ImGui::GetForegroundDrawList()->AddRectFilled({r_x - 60, r_y + 37 + 2}, {r_x - 60 + 载具油量 * 0.6, r_y + 37 - 2}, ImColor(255, 255, 255, 255), 0.0f);
            }
        // }
    }
}
            // if (按钮.物资总开关)
            // {
                int MaterialID = 读写.getDword(对象地址.敌人地址 + 0x5fc); 
                std::string name = getBoxName(MaterialID);
                if (name != "Error") {
                    // if (对象信息.敌人信息.距离 < 200)
                    {
                        name += "[";
                        name += std::to_string((int)对象信息.敌人信息.距离);
                        name += "米]";
                        auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                        ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, 绘制::floatArrToImColor(绘制::物资颜色), name.c_str());
                    }
                }
            // }

            if (按钮.爆炸猎弓 && (strstr(ClassName, "BP_Other_HuntingBow_Wrapper_C") != 0)) {
                std::string name = "爆炸猎弓[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }

            if (按钮.超级物资箱 && (strstr(ClassName, "MilitarySupplyBoxBase_Baltic_Theme_C") != 0 or strstr(ClassName, "EscopeBox_SpeEffect_C") != 0)) {
                std::string name = "超级物资箱[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }
            if (按钮.手雷预警 && (strstr(ClassName, "AttachActor_Ring_BP_C") != 0 or strstr(ClassName, "AttachActor_Ring_BP_C") != 0)) {
                std::string name = "正在手持手雷!![";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }

            if (按钮.绘制信号枪 && (strstr(ClassName, "Pistol_Flaregun_") != 0 or strstr(ClassName, "Pistol_RevivalFlaregun_Wrapper") != 0)) {
                std::string name = "信号枪[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }
            if (按钮.绘制信号枪 && strstr(ClassName, "Ammo_Flare") != 0) {
                std::string name = "信号枪子弹[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }
            if (按钮.绘制金插 && strstr(ClassName, "PeopleSkill") != 0) {
                std::string name = "金色插件[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 30);
                ImGui::GetForegroundDrawList()->AddText(NULL, 30, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 0, 255), name.c_str());
            }
if (按钮.盒子图片 && (strstr(ClassName, "CharacterDeadInventoryBox_C") != 0 || 
                      strstr(ClassName, "EscapePlayerTombBox_C") != 0 || 
                      strstr(ClassName, "aracterDeadInventoryBox_C") != 0 || 
                      strstr(ClassName, "SuperPeopleDeadBox_C") != 0 || 
                      strstr(ClassName, "_TrainingBoxListWrapper_C") != 0 || 
                      strstr(ClassName, "PickUpListWrapperActor") != 0)) {
    // 只显示盒子图片
    ImGui::GetBackgroundDrawList()->AddImage(手持图片[12].textureId, 
                                           ImVec2(t_屏幕坐标.X + 14, t_屏幕坐标.Y - 40), 
                                           ImVec2(t_屏幕坐标.X + 54, t_屏幕坐标.Y));
}
if (按钮.盒子文字 && (strstr(ClassName, "CharacterDeadInventoryBox_C") != 0 || 
                     strstr(ClassName, "EscapePlayerTombBox_C") != 0 || 
                     strstr(ClassName, "aracterDeadInventoryBox_C") != 0 || 
                     strstr(ClassName, "SuperPeopleDeadBox_C") != 0 || 
                     strstr(ClassName, "_TrainingBoxListWrapper_C") != 0 || 
                     strstr(ClassName, "PickUpListWrapperActor") != 0)) {
    std::string name = "盒子[";
    name += std::to_string((int)对象信息.敌人信息.距离);
    name += "M]";
    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y}, ImColor(255, 255, 0, 255), name.c_str());
}
            if (按钮.绘制空投 && (strstr(ClassName, "AirDropBox") != 0 or strstr(ClassName, "AirDropBox") != 0)) {
                std::string name = "空投[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "M]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y}, ImColor(255, 255, 0, 255), name.c_str());
            }
int 开启状态 = 读写.getDword(对象地址.敌人地址 + 0x270);
            if (按钮.绘制宝箱 && (strstr(ClassName, "EscapeBox_SupplyBox_") != 0 or strstr(ClassName, "EscapeBoxHight_SupplyBox_") != 0)) {
                if (按钮.隐藏已开启 && 开启状态 == 1) {
                } else {
                    std::string name = "宝箱 ";
                    name += Level(ClassName);
                    if (开启状态 == 1) {
                        name += " 已开启";
                    } else {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y}, ImColor(255, 255, 0, 255), name.c_str());
                }
            }
            if (按钮.绘制药箱 && (strstr(ClassName, "EscapeBox_Medical_") != 0 or strstr(ClassName, "EscapeBoxHight_Medical_") != 0)) {
                if (按钮.隐藏已开启 && 开启状态 == 1) {
                } else {
                    std::string name = "药箱 ";
                    name += Level(ClassName);
                    if (开启状态 == 1) {
                        name += " 已开启";
                    } else {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y}, ImColor(255, 255, 0, 255), name.c_str());
                }
            }
            if (按钮.绘制武器箱 && (strstr(ClassName, "EscapeBox_Weapon_") != 0 or strstr(ClassName, "EscapeBoxHight_Weapon_") != 0)) {
                if (按钮.隐藏已开启 && 开启状态 == 1) {
                } else {
                    std::string name = "武器箱 ";
                    name += Level(ClassName);
                    if (开启状态 == 1) {
                        name += " 已开启";
                    } else {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {t_屏幕坐标.X - (textSize.x / 2) + 50, t_屏幕坐标.Y}, ImColor(255, 255, 0, 255), name.c_str());
                }
            }
        }

        if (strstr(ClassName, "BPPawn_Escape_Raven") != 0 or strstr(ClassName, "BPPawn_Escape_UAV_C") != 0) {
            continue;
        }
        if (读写.getDword(地址.自身地址) == 0 && 对象信息.敌人信息.距离 <= 5) {
            continue;
        }

        bool isboss = 骨骼->isBoss(*ClassName);

if(读写.getFloat(对象地址.敌人地址+0x2b78)==479.5 or strstr(ClassName, "BPPawn_Escape_") != 0){
      D4DVector 屏外预警坐标(r_x,r_y,r_y-r_z,(r_y-r_z)/2);
对象信息.敌人信息.队伍 = 读写.getDword(对象地址.敌人地址+0x998);
对象信息.敌人信息.isboot = (对象信息.敌人信息.队伍 == -1) ? 1 : 读写.getDword(对象地址.敌人地址+0xa59);//人机0xAb4
      
      int botFlag = 读写.getDword(对象地址.敌人地址 + 0xa59);
    int 队伍ID = 对象信息.敌人信息.队伍;
    int 人机判断 = 0;
    if (队伍ID)
    {
        if (botFlag == 16842753 || botFlag == 16843009 || botFlag == 16843008)
        {
            人机判断 = 1;
        }
    }
    对象信息.敌人信息.isboot = 人机判断;

      if (按钮.忽略人机 && 对象信息.敌人信息.isboot == 1) {
        continue;
      }
对象信息.敌人信息.状态 = 读写.getDword(对象地址.敌人地址+0x1058);/**/
      对象信息.敌人信息.雷达 = 计算.rotateCoord(自身数据.准星Y, (自身数据.坐标.X - 对象信息.敌人信息.坐标.X) / 200, (自身数据.坐标.Y - 对象信息.敌人信息.坐标.Y) / 200);
      
      if (读写.getPtr64(对象地址.敌人地址 + 0x2c0)) {
         读写.read(读写.getPtr64(对象地址.敌人地址 + 0x2c0) + 0x1b8, &对象信息.敌人信息.向量, sizeof(对象信息.敌人信息.向量)); 
      } else {
          读写.read(对象地址.敌人地址 + 0x2c0, &对象信息.敌人信息.向量, sizeof(对象信息.敌人信息.向量)); 
      }
            
//读写.read(对象地址.敌人地址 +0x2c0, &对象信息.敌人信息.向量, sizeof(对象信息.敌人信息.向量));
      
      
对象信息.敌人信息.Rotator = 读写.getFloat(对象地址.敌人地址+0x138); 
      
      对象信息.敌人信息.当前血量 = 读写.getFloat(对象地址.敌人地址+0xe60); 
      对象信息.敌人信息.最大血量 = 读写.getFloat(对象地址.敌人地址+0xe64); 
      
     
      对象信息.敌人信息.手持 = 读写.getDword(读写.getPtr64(读写.getPtr64(读写.getPtr64(对象地址.敌人地址 + 0x2608) + 0x5d8) + 0x1370) + 0x1e0);

            对象信息.敌人信息.子弹数量 = 读写.getDword(读写.getPtr64(读写.getPtr64(读写.getPtr64(对象地址.敌人地址 + 0x2608) + 0x5d8) + 0x1370) + 0x1018);
      对象信息.敌人信息.子弹最大数量 = 读写.getDword(读写.getPtr64(读写.getPtr64(读写.getPtr64(对象地址.敌人地址 + 0x2608) + 0x5d8) + 0x1370) + 0x1090);
对象信息.敌人信息.敌人头 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x5040)+0x1fc);
对象信息.敌人信息.敌人甲 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x5040)+0x21c);
对象信息.敌人信息.敌人包 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x5040)+0x1cc);
         
long int MeshOffset = 读写.getPtr64(对象地址.敌人地址 + 0x510);
int Bonecount = 读写.getDword(MeshOffset + 0x9a8);  // 改为 getDword，读4字节
D3DVector tempBones[17];
骨骼->更新骨骼数据(
    MeshOffset + 0x210,
    读写.getPtr64(MeshOffset + 0x9a8) + 0x30,
    tempBones,
    Bonecount,
    对象信息.敌人信息.队伍,
    ClassName,
    闪框解密,    // 使用闪框解密标志
    对象信息.敌人信息.坐标,
    对象信息.敌人信息.最大血量);  // 传入 MaxHealth
                for (int i = 0; i < 15; i++)
            {
                对象信息.敌人信息.骨骼坐标[i].X = tempBones[i].X;
                对象信息.敌人信息.骨骼坐标[i].Y = tempBones[i].Y;
                对象信息.敌人信息.骨骼坐标[i].Z = tempBones[i].Z;
            }

      char temp[64];
读写.getUTF8(temp,读写.getPtr64(对象地址.敌人地址+0x960));
      对象信息.敌人信息.名字 = temp;
      bool 是否掐雷 = false;
if (按钮.手雷预警) {
    // 获取当前敌人状态对应的动作描述
    const char* actionDesc = GetHol(对象信息.敌人信息.状态);
    
    // 判断是否为“正在掐雷”
    bool isPullingPin = (actionDesc != nullptr && strcmp(actionDesc, "投掷") == 0);
    
    if (isPullingPin) {
        // 开始计时（如果尚未开始）
        if (!计时器.hasTimer(计算地址)) {
            计时器.addTimer(计算地址, 0);
            是否掐雷 = true;
        }
    } else {
        // 如果之前有计时器，则移除（或按原逻辑处理）
        if (计时器.hasTimer(计算地址)) {
            // 可以沿用原代码中的移除逻辑（手雷类、Antitankgrenade 等）
            // 例如：
            string 手雷递送 = 手雷类.calculateKey(对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y);
            if (手雷递送 == "") {
                Antitankgrenade[计算地址] = Antitankgrenade[计算地址] + 1;
                if (Antitankgrenade[计算地址] == 15) {
                    计时器.removeTimer(计算地址);
                    Antitankgrenade.erase(计算地址);
                }
            } else {
                计时器.renameTimer(计算地址, 手雷递送);
                手雷类.remove(手雷递送);
            }
        }
    }
}

            // 手雷倒计时计算

            if (对象信息.敌人信息.队伍 == 自身数据.自身队伍)
                continue;
if (对象信息.敌人信息.队伍 == 按钮.观透队伍) continue;

            bool LineOfSightTo1 = true;
            
    // if (按钮.雷达)
    // {
        // if (对象信息.敌人信息.距离 <= 500) 
        // {
            // string 距离文本 = to_string((int)对象信息.敌人信息.距离);
            // ImVec2 敌人位置 = {按钮.雷达X + 对象信息.敌人信息.雷达.X, 按钮.雷达Y + 对象信息.敌人信息.雷达.Y};        
            // int 队伍编号 = 对象信息.敌人信息.队伍 % 100;
            // float 色相 = (队伍编号 * 3.6f) / 360.0f;        
            // ImColor 填充颜色 = ImColor::HSV(色相, 0.8f, 1.0f);
            // ImColor 描边颜色 = ImColor(0, 0, 0, 255);                    
            
            // if (对象信息.敌人信息.isboot == 1) 
            // {
                // 填充颜色 = ImColor(255, 255, 255, 255);
            // }
            
            // ImGui::GetForegroundDrawList()->AddCircle(敌人位置, 9, 描边颜色, 0, 1.0f);
            // ImGui::GetForegroundDrawList()->AddCircleFilled(敌人位置, 8, 填充颜色);
            
            // ImVec2 文本尺寸 = ImGui::CalcTextSize(距离文本.c_str());
            // ImVec2 文字位置 = {敌人位置.x - 文本尺寸.x / 2, (敌人位置.y - 文本尺寸.y / 2) + 15};
            // ImColor 文本颜色 = (填充颜色.Value.x + 填充颜色.Value.y + 填充颜色.Value.z > 1.5f) ? 
                             // ImColor(255, 0, 0, 255) : ImColor(255, 255, 255, 255);       
            
            // 绘图.绘制字体描边(14, 文字位置.x, 文字位置.y, 文本颜色, 距离文本.c_str());
        // }
    // }
    /*雷达第二部分*/
if (按钮.雷达)
{
    // if (对象信息.敌人信息.距离 <= 500) 
    if (对象信息.敌人信息.距离 <= 按钮.雷达视距)
    {
        // 检查是否开启敌人圆点显示
        // {
            string 距离文本 = to_string((int)对象信息.敌人信息.距离);
            ImVec2 敌人位置 = {按钮.雷达X + 对象信息.敌人信息.雷达.X, 按钮.雷达Y + 对象信息.敌人信息.雷达.Y};        
            int 队伍编号 = 对象信息.敌人信息.队伍 % 100;
            float 色相 = (队伍编号 * 3.6f) / 360.0f;        
            ImColor 填充颜色 = ImColor::HSV(色相, 0.8f, 1.0f);
            ImColor 描边颜色 = ImColor(0, 0, 0, 255);                    
            
            if (对象信息.敌人信息.isboot == 1) 
            {
                填充颜色 = ImColor(255, 255, 255, 255);
            }
            float 敌人圆点大小;
            if (按钮.显示敌人圆点){
            // 使用可调节的圆点大小
            敌人圆点大小 = 按钮.圆点大小; // 可调节的圆点大小值
            } else {
            敌人圆点大小 = 0.0f; // 可调节的圆点大小值
            }
            // 绘制圆点（外圈描边 + 内圈填充）
            ImGui::GetForegroundDrawList()->AddCircle(敌人位置, 敌人圆点大小, 描边颜色, 0, 1.0f);
            ImGui::GetForegroundDrawList()->AddCircleFilled(敌人位置, 敌人圆点大小 - 1, 填充颜色);
           if(按钮.显示距离) {
            // 计算距离文本位置（根据圆点大小调整偏移）
            ImVec2 文本尺寸 = ImGui::CalcTextSize(距离文本.c_str());
            ImVec2 文字位置 = {敌人位置.x - 文本尺寸.x / 2, (敌人位置.y - 文本尺寸.y / 2) + 敌人圆点大小 + 6};
            ImColor 文本颜色 = (填充颜色.Value.x + 填充颜色.Value.y + 填充颜色.Value.z > 1.5f) ? 
                             ImColor(255, 0, 0, 255) : ImColor(255, 255, 255, 255);       
            
            绘图.绘制字体描边(14, 文字位置.x, 文字位置.y, 文本颜色, 距离文本.c_str());
            }
        // }
    }
}
        if(对象信息.敌人信息.状态==1048592||对象信息.敌人信息.状态==1048576)
                continue;
            if (对象信息.敌人信息.isboot == 1) {
                绘制人机++;
            } else {
                绘制真人++;
            }
            /*模型漏打*/
/*模型漏打*/
                       static bool isVisible[15];
if (按钮.手动开启 == 1) {
           for(int i = 0; i < 15; i++) {
                Vector3 locVec(PovLocation.X, PovLocation.Y, PovLocation.Z);
                auto& bone = 对象信息.敌人信息.骨骼坐标[i];
                Vector3 boneVec(bone.X, bone.Y, bone.Z);
                isVisible[i] = CustomLineTrace(locVec, boneVec);
           }
            } else {
            // for(int i = 0; i < 15; i++) {//追踪不开掩体没效果改这个
        isVisible[15] = true;
    // }
}
            // 下面开始绘制
            骨骼数据 t_骨骼数据 = 计算.计算骨骼(自身数据.矩阵, 对象信息.敌人信息.骨骼坐标, PX, PY);
            绘图.初始化坐标(t_屏幕坐标, t_骨骼数据);
            if (t_屏幕坐标.W >= 0) {
                if (自瞄.倒地不瞄 && 对象信息.敌人信息.当前血量 <= 0) {
                } else if (自瞄.人机不瞄 && 对象信息.敌人信息.isboot == 1) {
                } else {
                    bool isTargetVisible = false;
    
    
    if (自瞄.瞄准部位 == 0) { 
        isTargetVisible = isVisible[0];  
    } else if (自瞄.瞄准部位 == 1) { 
        isTargetVisible = isVisible[1];  
    } else if (自瞄.瞄准部位 == 2) { 
        isTargetVisible = isVisible[2];  
    } else if (自瞄.瞄准部位 == 3) { 
        
        if (对象信息.敌人信息.距离 <= 50.0f) {
            
            isTargetVisible = isVisible[0] || isVisible[1] || isVisible[2];
        } else {
            
            isTargetVisible = isVisible[0] || isVisible[1];
        }
    }
    
bool canAim;
if (按钮.手动开启 == 1) {
    
    canAim = isTargetVisible || 对象信息.敌人信息.距离 <= 5.0f;
} else {
    canAim = true;
}
    if (canAim ) {//漏打逻辑

                    自瞄函数[自瞄.瞄准对象数量].距离 = 对象信息.敌人信息.距离;
                    自瞄函数[自瞄.瞄准对象数量].人物向量 = 对象信息.敌人信息.向量;
                    自瞄函数[自瞄.瞄准对象数量].血量 = 对象信息.敌人信息.当前血量;
                    自瞄函数[自瞄.瞄准对象数量].Bone = 读写.getPtr64(MeshOffset + 0x9a8) + 0x30; 
                    自瞄函数[自瞄.瞄准对象数量].Human = MeshOffset + 0x210; 
                    自瞄函数[自瞄.瞄准对象数量].名字 = 对象信息.敌人信息.名字;
                    自瞄函数[自瞄.瞄准对象数量].阵营 = 对象信息.敌人信息.队伍;
                    自瞄函数[自瞄.瞄准对象数量].头 = 对象信息.敌人信息.头;
                    自瞄函数[自瞄.瞄准对象数量].甲 = 对象信息.敌人信息.甲;
                    自瞄函数[自瞄.瞄准对象数量].头甲包地址 = 对象信息.敌人信息.头甲包地址;
                    自瞄函数[自瞄.瞄准对象数量].可见 = isTargetVisible; 
                    memcpy(自瞄函数[自瞄.瞄准对象数量].骨骼坐标, 对象信息.敌人信息.骨骼坐标, sizeof(对象信息.敌人信息.骨骼坐标));
                    if (自瞄.瞄准部位 == 0) {
                        自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[0];
                        自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - t_骨骼数据.Head.X, 2) + pow(PY - t_骨骼数据.Head.Y, 2));
                        自瞄函数[自瞄.瞄准对象数量].对象骨骼 = t_骨骼数据.Head;
                    } else if (自瞄.瞄准部位 == 1) {
                        自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[1];
                        自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - t_骨骼数据.Chest.X, 2) + pow(PY - t_骨骼数据.Chest.Y, 2));
                        自瞄函数[自瞄.瞄准对象数量].对象骨骼 = t_骨骼数据.Chest;
                    } else if (自瞄.瞄准部位 == 2) {
                        自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[2];
                        自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - t_骨骼数据.Pelvis.X, 2) + pow(PY - t_骨骼数据.Pelvis.Y, 2));
                        自瞄函数[自瞄.瞄准对象数量].对象骨骼 = t_骨骼数据.Pelvis;
                    } else if (自瞄.瞄准部位 == 3) {
                            static int gmin=1;
                            float dmin=666;
                            static float 与准距[6];
                            与准距[1] = sqrt(pow(PX - t_骨骼数据.Head.X, 2) + pow(PY - t_骨骼数据.Head.Y, 2));
                            与准距[2] = sqrt(pow(PX - t_骨骼数据.Right_Elbow.X, 2) + pow(PY - t_骨骼数据.Right_Elbow.Y, 2));
                            与准距[3] = sqrt(pow(PX - t_骨骼数据.Left_Elbow.X, 2) + pow(PY - t_骨骼数据.Left_Elbow.Y, 2));
                            与准距[4] = sqrt(pow(PX - t_骨骼数据.Right_Knee.X, 2) + pow(PY - t_骨骼数据.Right_Knee.Y, 2));
                            与准距[5] = sqrt(pow(PX - t_骨骼数据.Left_Knee.X, 2) + pow(PY - t_骨骼数据.Left_Knee.Y, 2));
                            
                            for(int i=1;i < 6; i++)
                            {
                                if(与准距[i]<dmin)
                                {
                                    gmin=i;
                                    dmin=与准距[i];
                                }
                            }
                            switch (gmin) 
                            {
                                case 1:
                                    自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[0];
                                    自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - t_骨骼数据.Head.X, 2) + pow(PY - t_骨骼数据.Head.Y, 2));
                                    自瞄函数[自瞄.瞄准对象数量].对象骨骼 = t_骨骼数据.Head;
                                    break;
                                case 2:
                                    自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[6];
                                    自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - t_骨骼数据.Right_Elbow.X, 2) + pow(PY - t_骨骼数据.Right_Elbow.Y, 2));
                                    自瞄函数[自瞄.瞄准对象数量].对象骨骼 = t_骨骼数据.Right_Elbow;
                                    break;
                                case 3:
                                    自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[5];
                                    自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - t_骨骼数据.Left_Elbow.X, 2) + pow(PY - t_骨骼数据.Left_Elbow.Y, 2));
                                    自瞄函数[自瞄.瞄准对象数量].对象骨骼 = t_骨骼数据.Left_Elbow;
                                    break;
                                case 4:
                                    自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[12];
                                    自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - t_骨骼数据.Right_Knee.X, 2) + pow(PY - t_骨骼数据.Right_Knee.Y, 2));
                                    自瞄函数[自瞄.瞄准对象数量].对象骨骼 = t_骨骼数据.Right_Knee;
                                    break;
                                case 5:
                                    自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[11];
                                    自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - t_骨骼数据.Left_Knee.X, 2) + pow(PY - t_骨骼数据.Left_Knee.Y, 2));
                                    自瞄函数[自瞄.瞄准对象数量].对象骨骼 = t_骨骼数据.Left_Knee;
                                    break;
                            }
                            }
                    自瞄.瞄准对象数量++;
                }
            }
}
            /* if (按钮.被瞄预警)
            {
              auto aimAngle = 计算.rotateCoord(对象信息.敌人信息.骨骼坐标[1], 自身数据.坐标);
              auto aimMZ = FRotator(0, 对象信息.敌人信息.Rotator, 0);
              aimMZ.Clamp();
              float AimX = abs(aimAngle.X - aimMZ.Yaw);

              if (AimX <= 4)
              {
                被瞄信息[被瞄准对象数量].距离 = 对象信息.敌人信息.距离;
                被瞄信息[被瞄准对象数量].名字 = 对象信息.敌人信息.名字;
                被瞄信息[被瞄准对象数量].瞄准武器 = GetHolGunItem(对象信息.敌人信息.手持);
                被瞄准对象数量++;
              }
            } */

            if (按钮.背敌预警) {
                /*
                if (对象信息.敌人信息.isboot == 1){
                  OffScreen(ImGui::GetForegroundDrawList(), 屏外预警坐标, camear_r, ImColor(0, 255, 0, 255), 250, 对象信息.敌人信息.距离);
                }else {
                  OffScreen(ImGui::GetForegroundDrawList(), 屏外预警坐标, camear_r, ImColor(255, 0, 0, 255), 250, 对象信息.敌人信息.距离);
                }
                */

                                char 背敌距离[250];
                                sprintf(背敌距离, "%d米", ((int)对象信息.敌人信息.距离));
                                auto textSize = ImGui::CalcTextSize(背敌距离, 0, 25);
                                if (X + W / 2 < 0) {
                        // 修改圆形为正方形，边长=半径*2=120，中心坐标(0, Head.Y)
                                    ImGui::GetForegroundDrawList()->AddRectFilled(
                            {0 - 60, t_骨骼数据.Head.Y - 60},  // 左上角坐标
                            {0 + 60, t_骨骼数据.Head.Y + 60},  // 右下角坐标
                                    ImColor(135, 206, 235, 170)
                                    );
                                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {5, t_骨骼数据.Head.Y - 20} , ImColor(0, 0, 0, 255), 背敌距离);
                                } else if (W > 0 && X > PX * 2) {
                        // 中心坐标(PX*2, Head.Y)
                                    ImGui::GetForegroundDrawList()->AddRectFilled(
                                    {PX * 2 - 60, t_骨骼数据.Head.Y - 60} ,
                                    {PX * 2 + 60, t_骨骼数据.Head.Y + 60} ,
                                    ImColor(135, 206, 235, 170)
                                    );
                                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {PX * 2 - 45, t_骨骼数据.Head.Y - 20} , ImColor(0, 0, 0, 255), 背敌距离);
                                } else if (W > 0 && Y + W < 0) {
                        // 中心坐标(Head.X, 0)
                                    ImGui::GetForegroundDrawList()->AddRectFilled(
                                    {t_骨骼数据.Head.X - 60, 0 - 60} ,
                                    {t_骨骼数据.Head.X + 60, 0 + 60} ,
                                    ImColor(135, 206, 235, 170)
                                    );
                                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {t_骨骼数据.Head.X - 25, 10} , ImColor(0, 0, 0, 255), 背敌距离);
                                } else if (W < 0) {
                        // 中心坐标(PX*2 - Head.X, PY*2)
                                    ImGui::GetForegroundDrawList()->AddRectFilled(
                                    {PX * 2 - t_骨骼数据.Head.X - 60, PY * 2 - 60} ,
                                    {PX * 2 - t_骨骼数据.Head.X + 60, PY * 2 + 60} ,
                                    ImColor(135, 206, 235, 170)
                                    );
                                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {PX * 2 - t_骨骼数据.Head.X - 25, PY * 2 - 30} , ImColor(0, 0, 0, 255), 背敌距离);
                                }
            }

            if (t_屏幕坐标.W >= 0) {
                if (按钮.方框)
                    绘图.绘制方框(对象信息.敌人信息.isboot);
                if (按钮.射线)
                    绘图.绘制射线(t_骨骼数据);
                if (按钮.名字) {
                    绘图.绘制名字(对象信息.敌人信息.名字, 对象信息.敌人信息.isboot, 0, false, ClassName, 对象信息.敌人信息.队伍, Bonecount);
                }
                if (按钮.距离)
                    绘图.绘制距离(对象信息.敌人信息.距离, 对象信息.敌人信息.队伍);
            //    if (按钮.血量)
        if(按钮.血量)
         绘图.绘制血量(对象信息.敌人信息.最大血量,对象信息.敌人信息.当前血量,t_骨骼数据);

                if (按钮.骨骼) {
                    // 绘图.绘制骨骼(t_骨骼数据, t_屏幕坐标, LineOfSightTo1, 对象信息.敌人信息.距离);
                绘图.绘制骨骼(t_骨骼数据, t_屏幕坐标, isVisible, 对象信息.敌人信息.距离);/*模型漏打*/
                }
                if (按钮.头甲包显示)
                            绘图.头甲包显示(t_骨骼数据, 对象信息.敌人信息.敌人头, 对象信息.敌人信息.敌人甲, 对象信息.敌人信息.敌人包, t_屏幕坐标);
                if (按钮.手持)
                    绘图.绘制手持(对象信息.敌人信息.手持, 对象信息.敌人信息.状态, 对象信息.敌人信息.子弹数量, 对象信息.敌人信息.子弹最大数量);
            }
        }
    }
    if (自瞄.初始化 && 自瞄.准星射线 && 自瞄.瞄准目标 != -1 && 自瞄函数[自瞄.瞄准目标].准心距离 <= 自瞄.自瞄范围) {
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(PX, PY), ImVec2(自瞄函数[自瞄.瞄准目标].对象骨骼.X, 自瞄函数[自瞄.瞄准目标].对象骨骼.Y), ImColor(255, 255, 255, 255), 2.1);
    }
    /* if (按钮.被瞄预警)
      绘图.绘制瞄准信息(); */
    if (按钮.人数)
        绘图.绘制人数(绘制人机, 绘制真人);
    自瞄.瞄准总数量 = 自瞄.瞄准对象数量;
}

void 绘制::运行绘制() {
    更新地址数据();
    更新对象数据();

    // 掩体线程();
    if (!按钮.自瞄选项 && 自瞄.初始化) {
        // 驱动自瞄主线程();
        自瞄主线程();
    }

    if (按钮.手雷预警) {
        计时器.updateTimers();
        计时器.checkAndRemoveTimers();
    }
}

const char* 绘制::Level(char* name) {
    if (strstr(name, "Lv1") != 0) {
        return "Lv1";
    } else if (strstr(name, "Lv2") != 0) {
        return "Lv2";
    } else if (strstr(name, "Lv3") != 0) {
        return "Lv3";
    } else if (strstr(name, "Lv4") != 0) {
        return "Lv4";
    } else if (strstr(name, "Lv5") != 0) {
        return "Lv5";
    } else if (strstr(name, "Lv6") != 0) {
        return "Lv6";
    } else if (strstr(name, "Lv7") != 0) {
        return "Lv7";
    }
    return "未知";
}

const char* 绘制::getMaterialName(char* name) {
    static char dealname[32];
    memset(dealname, '\0', sizeof(dealname));
    if (strstr(name, "VH_MotorcycleCart") != 0) {
        strcpy(dealname, "三轮摩托车");
        按钮.VehicleID = 43;
    } else if (strstr(name, "VH_Scooter") != 0) {
        strcpy(dealname, "小绵羊车");
        按钮.VehicleID = 45;
    } else if (strstr(name, "VH_Motorcycle") != 0) {
        strcpy(dealname, "摩托车");
        按钮.VehicleID = 44;
    } else if (strstr(name, "VH_Tuk") != 0) {
        strcpy(dealname, "三轮摩托车");
        按钮.VehicleID = 48;
    } else if (strstr(name, "Buggy") != 0) {
        strcpy(dealname, "蹦蹦");
        按钮.VehicleID = 30;
    } else if (strstr(name, "Mirado") != 0) {
        strcpy(dealname, "敞篷跑车");
        按钮.VehicleID = 35;
    } else if (strstr(name, "CoupeRB") != 0) {
        strcpy(dealname, "跑车");
        按钮.VehicleID = 39;
    } else if (strstr(name, "Dacia") != 0) {
        strcpy(dealname, "轿车");
        按钮.VehicleID = 31;
    } else if (strstr(name, "PickUp_02_C") != 0) {
        strcpy(dealname, "皮卡");
        按钮.VehicleID = 33;
    } else if (strstr(name, "Rony") != 0) {
        strcpy(dealname, "皮卡");
        按钮.VehicleID = 38;
    } else if (strstr(name, "_StationWagon_C") != 0) {
        strcpy(dealname, "旅行车");
        按钮.VehicleID = 40;
    } else if (strstr(name, "UAZ") != 0) {
        strcpy(dealname, "吉普");
        按钮.VehicleID = 32;
    } else if (strstr(name, "PG117") != 0) {
        strcpy(dealname, "快艇");
        按钮.VehicleID = 49;
    } else if (strstr(name, "AquaRail") != 0) {
        strcpy(dealname, "冲锋艇");
        按钮.VehicleID = 50;
    } else if (strstr(name, "MiniBus") != 0) {
        strcpy(dealname, "面包车");
        按钮.VehicleID = 37;
    } else if (strstr(name, "VH_BRDM") != 0) {
        strcpy(dealname, "两栖装甲车");
        按钮.VehicleID = 0;
    } else if (strstr(name, "LadaNiva") != 0) {
        strcpy(dealname, "雪地越野车");
        按钮.VehicleID = 36;
    } else if (strstr(name, "_4SportCar_C") != 0) {
        strcpy(dealname, "敞篷跑车");
        按钮.VehicleID = 41;
    } else if (strstr(name, "_Bigfoot_C") != 0) {
        strcpy(dealname, "大脚车");
        按钮.VehicleID = 53;
    } else if (strstr(name, "_Training_C") != 0) {
        strcpy(dealname, "自行车");
        按钮.VehicleID = 0;
    } else if (strstr(name, "ATV1_C") != 0) {
        strcpy(dealname, "越野摩托车");
        按钮.VehicleID = 0;
    } else if (strstr(name, "_Horse") != 0) {
        strcpy(dealname, "马");
        按钮.VehicleID = 52;
    } else if (strstr(name, "_02_C") != 0) {
        strcpy(dealname, "雪地摩托车");
        按钮.VehicleID = 46;
    } else if (strstr(name, "Snowmobile_C") != 0) {
        strcpy(dealname, "雪地摩托车");
        按钮.VehicleID = 47;
    } else if (strstr(name, "_Motorglider_C") != 0) {
        strcpy(dealname, "滑翔机");
        按钮.VehicleID = 51;
    } else if (strstr(name, "_DesertCar_C") != 0) {
        strcpy(dealname, "大脚越野车");
        按钮.VehicleID = 0;
    } else if (strstr(name, "_New_C") != 0) {
        strcpy(dealname, "赛车");
        按钮.VehicleID = 0;
    } else if (strstr(name, "AttachActor_Ring_BP_C") != 0) {
//        strcpy(dealname, "正在掐雷");
        return "正在掐雷";
    } else {
        strcpy(dealname, "Error");
    }
    return dealname;
}

string 绘制::getBoxName(int id) {
    if (按钮.显示医疗箱 && id == 601006)
        return "[药品]医疗箱";
    if (按钮.显示急救包 && id == 601005)
        return "[药品]急救包";
    if (按钮.显示绷带 && id == 601004)
        return "[药品]绷带";
    if (按钮.显示可乐 && id == 601001)
        return "[药品]可乐";
    if (按钮.显示肾上腺素 && id == 601002)
        return "[药品]肾上腺素";
    if (按钮.显示止痛药 && id == 601003)
        return "[药品]止痛药";

    if (按钮.显示三级甲 && id == 503003)
        return "[防具]三级防弹衣";
    if (按钮.显示三级头 && id == 502003)
        return "[防具]三级头盔";
    if (按钮.显示三级包 && id == 501003)
        return "[背包]三级包";

    if (按钮.其他物资) {
        if (id == 107010)
            return "[特殊]突击盾牌";
        if (id == 603001)
            return "[特殊]汽油";
        if (id == 403990)
            return "[特殊]吉利服";
        if (id == 403187)
            return "[特殊]吉利服";
        if (id == 106007)
            return "[特殊]信号枪";
        if (id == 106094)
            return "[特殊]召回信号枪";
            }
        
    
    if (按钮.狙击枪械) {
        if (id == 107001)
            return "[武器]十字弩";
        if (id == 103003)
            return "[武器]AWM";
        if (id == 103001)
            return "[武器]Kar98K";
        if (id == 103002)
            return "[武器]M24";
        if (id == 103006)
            return "[武器]Mini14";
        if (id == 103008)
            return "[武器]Win94";
        if (id == 103009)
            return "[武器]SLR";
        if (id == 103012)
            return "[武器]AMR狙击枪";
        if (id == 103013)
            return "[武器]M417";
        if (id == 103014)
            return "[武器]MK20-H";
        if (id == 103100)
            return "[武器]MK12";
        if (id == 103010)
            return "[武器]QBU";
        if (id == 103004)
            return "[武器]SKS";
        if (id == 103007)
            return "[武器]MK14";
        if (id == 103015)
            return "[武器]M200";
        if (id == 103011)
            return "[武器]莫辛狙击枪";
    }
    if (按钮.显示步枪) {
        if (id == 101012)
            return "[武器]蜜獾";
        if (id == 101008)
            return "[武器]M762";
        if (id == 101011)
            return "[武器]VC-VAL突击步枪";
        if (id == 101010)
            return "[武器]G36C步枪";
        if (id == 101013)
            return "[武器]Famas突击步枪";
        if (id == 105002)
            return "[武器]DP28";
        if (id == 101005)
            return "[武器]狗砸";
        if (id == 101001)
            return "[武器]AKM";
        if (id == 101007)
            return "[武器]QBZ";
        if (id == 101009)
            return "[武器]MK47";
        if (id == 101006)
            return "[武器]AUG";
        if (id == 101002)
            return "[武器]M16A4";
        if (id == 101003)
            return "[武器]SCAR";
        if (id == 101004)
            return "[武器]M416";
        if (id == 105001)
            return "[武器]M249";
        if (id == 105012)
            return "[武器]PKM";
        if (id == 105010)
            return "[武器]MG3";
    }
    if (按钮.冲锋枪械) {
        if (id == 102001)
            return "[武器]UZI";
        if (id == 102004)
            return "[武器]汤姆逊冲锋枪";
        if (id == 102003)
            return "[武器]维克托";
        if (id == 102002)
            return "[武器]UMP9";
        if (id == 103005)
            return "[武器]VSS射手步枪";
        if (id == 102005)
            return "[武器]野牛冲锋枪";
        if (id == 102007)
            return "[武器]MP5K";
        if (id == 102105)
            return "[武器]P90冲锋枪";
    }
    if (按钮.散弹枪械) {
        if (id == 106003)
            return "[武器]R1895";
        if (id == 106006)
            return "[武器]短管散弹枪";
        if (id == 104003)
            return "[武器]S12K";
        if (id == 104002)
            return "[武器]S1897";
        if (id == 104001)
            return "[武器]双管猎枪";
        if (id == 104004)
            return "[武器]DBS";
        if (id == 104100)
            return "[武器]SPAS-12";
        if (id == 104005)
            return "[武器]AA12-G";
    }
    if (按钮.手枪枪械) {
        if (id == 106001)
            return "[武器]P92手枪";
        if (id == 106002)
            return "[武器]P1911手枪";
        if (id == 106003)
            return "[武器]R1895手枪";
        if (id == 106004)
            return "[武器]P18C手枪";
        if (id == 106005)
            return "[武器]R45手枪";
        if (id == 106006)
            return "[武器]短管散弹枪";
        if (id == 106008)
            return "[武器]蝎式手枪";
        if (id == 106009)
            return "[武器]医疗手枪";
        if (id == 106010)
            return "[武器]沙漠之鹰";
        if (id == 106011)
            return "[武器]TMP-9手枪";
        if (id == 106012)
            return "[武器]双持巨蟒手枪";
        if (id == 106093)
            return "[武器]手持激光钻";
        if (id == 106095)
            return "[武器]获胜礼花枪";
        if (id == 106096)
            return "[武器]元宵节礼花枪";
        if (id == 106099)
            return "[武器]新年礼花枪";
        if (id == 106100)
            return "[武器]冰霜手枪";
        if (id == 106101)
            return "[武器]红包能量枪";
        if (id == 106102)
            return "[武器]红包能量枪";
        if (id == 106103)
            return "[武器]电流枪";
        if (id == 106104)
            return "[武器]雪球枪";
    }
    
    if (按钮.显示762子弹 && id == 302001)
        return "[子弹]7.62MM";
    if (按钮.显示556子弹 && id == 303001)
        return "[子弹]5.56MM";
    if (按钮.显示9mm子弹 && id == 301001)
        return "[子弹]9MM";
    if (按钮.显示霰弹 && id == 304001)
        return "[子弹]12口径";
    if (按钮.显示45mm子弹 && id == 305001)
        return "[子弹]45口径";
    if (按钮.显示箭矢 && id == 307001)
        return "[子弹]箭矢";
    if (按钮.显示信号弹 && id == 308001)
        return "[特殊]信号弹";
    if (按钮.显示信号弹 && id == 308005)
        return "[特殊]召回信号弹";
    if (按钮.显示倍镜) {
        if (id == 203001)
            return "[倍镜]红点";
        if (id == 203014)
            return "[倍镜]3倍镜";
        if (id == 203015)
            return "[倍镜]6倍镜";
        if (id == 203005)
            return "[倍镜]8倍镜";
    }

    if (按钮.显示子弹袋 && (id == 204014 || id == 204010))
        return "[配件]子弹袋";
    if (按钮.显示箭袋 && id == 205004)
        return "[配件]箭袋";
    if (按钮.显示激光瞄准器 && id == 202007)
        return "[配件]激光瞄准器";
    if (按钮.显示轻型握把 && id == 202004)
        return "[配件]轻型握把";
    if (按钮.显示半截握把 && id == 202005)
        return "[配件]半截握把";
    if (按钮.显示UZI枪托 && id == 205001)
        return "[配件]UZI枪托";
    if (按钮.显示狙击枪托 && id == 205003)
        return "[配件]狙击枪托";
    if (按钮.显示步枪枪托 && id == 205002)
        return "[配件]步枪枪托";
    if (按钮.显示狙击枪补偿器 && id == 201003)
        return "[配件]狙击枪补偿器";
    if (按钮.显示狙击枪消焰器 && id == 201005)
        return "[配件]狙击枪消焰器";
    if (按钮.显示狙击枪消音器 && id == 201007)
        return "[配件]狙击枪消音器";
    if (按钮.显示步枪消音器 && id == 201011)
        return "[配件]步枪消音器";
    if (按钮.显示步枪补偿器 && id == 201009)
        return "[配件]步枪补偿器";
    if (按钮.显示步枪消焰器 && id == 201010)
        return "[配件]步枪消焰器";
    if (按钮.显示冲锋枪消音器 && id == 201006)
        return "[配件]冲锋枪消音器";
    if (按钮.显示冲锋枪消焰器 && (id == 201004 || id == 201002))
        return "[配件]冲锋枪消焰器";
    if (按钮.显示拇指握把 && id == 202006)
        return "[配件]拇指握把";
    if (按钮.显示垂直握把 && id == 202002)
        return "[配件]垂直握把";
    if (按钮.显示直角握把 && id == 202001)
        return "[配件]直角握把";
    if (按钮.显示撞火枪托 && id == 205011)
        return "[配件]撞火枪托";
    if (按钮.显示霰弹快速 && id == 204017)
        return "[配件]霰弹快速";
    if (按钮.显示鸭嘴枪口 && id == 201012)
        return "[配件]鸭嘴枪口";
    if (按钮.显示霰弹收束 && id == 201001)
        return "[配件]霰弹收束";

    if (按钮.显示扩容) {
        if (id == 204009)
            return "[配件]狙击枪快扩";
        if (id == 204007)
            return "[配件]狙击枪扩容";
        if (id == 204008)
            return "[配件]狙击枪快速";
        if (id == 204013)
            return "[配件]步枪快扩";
        if (id == 204011)
            return "[配件]步枪扩容";
        if (id == 204012)
            return "[配件]步枪快速";
        if (id == 204006)
            return "[配件]冲锋枪快扩";
        if (id == 204004)
            return "[配件]冲锋枪扩容";
        if (id == 204005)
            return "[配件]冲锋枪快速";
        if (id == 204003)
            return "[配件]手枪快扩";
        if (id == 204002)
            return "[配件]手枪快速";
        if (id == 204001)
            return "[配件]手枪扩容";
    }
    if (按钮.投掷物品) {
        if (id == 602003)
            return "[投掷]燃烧瓶";
        if (id == 602002)
            return "[投掷]烟雾弹";
        if (id == 602001)
            return "[投掷]震爆弹";
        if (id == 602004)
            return "[物品]手榴弹";
    }
    return "Error";
}
