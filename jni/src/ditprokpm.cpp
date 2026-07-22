#include <iostream>
#include <vector>
#include <initializer_list>
#include <unistd.h>
#include <stdio.h>

class proKpm {
private:
struct Ditpro_uct_kpm
{
	int pid;
	uintptr_t addr;
	void *buffer;
	size_t size;
	uint32_t mode;
};

struct Ditpro_uct_list
{
    int pid;
    uintptr_t addr[10];
    size_t addr_count;
    void *buffer;
    size_t buffer_size;
    uint32_t mode;
};

struct Ditpro_uct_base {
    pid_t pid;
    char* name;
    uintptr_t base;
};

enum {
__NR_syscall_ = 18,
__FLAGS = 616,
__READMEM = 0x400,
__READMEMLIST = 0x401,
__WRITEMEM = 0x200,
__MODULEBASE = 0x100, //未完成
__PROCPID = 0x50,
__UNINSTALL = 0x10, 
__KERNELMODE = 1, //1正常缓存映射 2正常缓存只读，写不可用 3无缓存映射，直接访问硬件

__CALLFUNC_1 = 0x900, //过触摸检测
};

public:
int pid = -1;
float mode = 1.f;

//读取整个链条
bool read(void *buffer,size_t buffer_size,std::initializer_list<uint64_t> args) {
	struct Ditpro_uct_list ptr;
	ptr.pid = pid;
	uintptr_t *p = ptr.addr;
	ptr.buffer = buffer;
	ptr.addr_count = args.size();
	if(ptr.addr_count > 10) return false;
    for (const auto &arg : args) {
    	*p = arg;
        p ++;
    }
    ptr.buffer_size = buffer_size;
    ptr.mode = (uint32_t)this->mode;
    if(syscall(__NR_syscall_, __FLAGS, &ptr, __READMEMLIST))
        return false;
    return true;
}

bool read(uintptr_t addr, void *buffer, size_t size){
    struct Ditpro_uct_kpm ptr;
    ptr.addr = addr;
    ptr.buffer = buffer;
    ptr.pid = this->pid;
    ptr.size = size;
    ptr.mode = (uint32_t)this->mode;
    if(syscall(__NR_syscall_, __FLAGS, &ptr, __READMEM))
        return false;
    return true;
}

template <typename T>
T read(uintptr_t addr)
{
	T res;
	if (this->read(addr, &res, sizeof(T)))
		return res;
	return {};
}

//失败返回 -1
// int get_pid(const char *name){
    // return syscall(__NR_syscall_, __FLAGS, name, __PROCPID);
// }
int get_pid(const char *name) {
    printf("[ditKPM调试] 开始获取PID，进程名: %s\n", name);
    
    // 尝试不同的参数组合
    int result = -1;
    
    // 组合1: 原始顺序
    result = syscall(__NR_syscall_, __FLAGS, name, __PROCPID);
    printf("[组合1] 结果: %d, 错误: %s\n", result, strerror(errno));
    if (result > 0) return result;
    
    // 组合2: 交换FLAGS和PROCPID
    result = syscall(__NR_syscall_, __PROCPID, name, __FLAGS);
    printf("[组合2] 结果: %d, 错误: %s\n", result, strerror(errno));
    if (result > 0) return result;
    
    // 组合3: 只传两个参数
    result = syscall(__NR_syscall_, __FLAGS, name);
    printf("[组合3] 结果: %d, 错误: %s\n", result, strerror(errno));
    if (result > 0) return result;
    
    // 组合4: 尝试不同的操作码
    int opcodes[] = {0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0, 0x100, 0x200, 0};
    for (int i = 0; opcodes[i] != 0; i++) {
        result = syscall(__NR_syscall_, __FLAGS, name, opcodes[i]);
        if (result > 0) {
            printf("[+] 找到有效操作码: 0x%x, PID: %d\n", opcodes[i], result);
            return result;
        }
    }
    
    return -1;
}

//务必调用
void uninstall(){
    syscall(__NR_syscall_, __FLAGS, 0, __UNINSTALL);
}

~proKpm() { uninstall(); }

//在getpid或者getbase之后调用即可
int tscape_input(const char *name){
    return syscall(__NR_syscall_, __FLAGS, name, __CALLFUNC_1);
}

uintptr_t get_module_base(int pid, const char *module_name) {
	FILE *fp; 
	long addr = 0; 
	char *pch; 
	char filename[64]; 
	char line[1024]; 
	snprintf(filename, sizeof(filename), "/proc/%d/maps", pid); 
	fp = fopen(filename, "r"); 
	if (fp != NULL) { 
		while (fgets(line, sizeof(line), fp)) { 
			if (strstr(line, module_name)) { 
				pch = strtok(line, "-"); 
				addr = strtoul(pch, NULL, 16); 
				if (addr == 0x8000) addr = 0; 
					break; 
			} 
		} fclose(fp); 
	} 
    return addr; 
}
};

// proKpm dpk;

// int main(){
	// dpk.pid = dpk.get_pid("com.YourCompany.FPSTrain");
	// uintptr_t base = dpk.get_module_base(dpk.pid, "libUE4.so");
	// printf("pid %d  %p\n",dpk.pid,base);
	// base += 0xA6F2000;
    
    // //填包名就可以
    // dpk.tscape_input("com.YourCompany.FPSTrain");
    // int value;
    // //正常
    // value = dpk.read<int>(base);
    // printf("%d\n",value);
    
    // //例子：通过链条直接读取数量 
    // dpk.read(&value,sizeof(value),{base+0x295050,0x30,0xa0});
    // printf("%d\n",value);
    
    // uint64_t Matrix = 0x0;
    
    // //原始链条getZZ_64(getZZ_64(libbase + 0x2656A8) + 0x20) + 0x280;
    // dpk.read(&Matrix, sizeof(Matrix), {base + 0x2656A8, 0x20});
    // Matrix += 280;
    // printf("%p\n",Matrix);
    
    // float pos[3];   
    // //例子：通过链条直接读取坐标 如：热门游戏的 278+1B0
    // dpk.read(&pos,sizeof(pos),{base+0x295050,0x30,0xa0});
    // printf("%.1f %.1f %.1f\n",pos[0],pos[1],pos[2]);
    
    
    
    // return 0;
// }