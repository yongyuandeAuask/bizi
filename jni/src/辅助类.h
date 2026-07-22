#ifndef 辅助类_H
#define 辅助类_H
// 系统调用和内核相关
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include "ReadGame.h"/*模型漏打*/

// 网络编程
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/netlink.h>

// 标准C库
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <regex.h>

// C++标准库
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <regex>
#include <sstream>

// 线程支持
#include <pthread.h>

#include "./CPUaffinity/timer.h"
#include "obfuscate.h"
#define PI 3.141592653589793238
#define MAX_PAYLOAD 1048
#define DEVICE_NAME "/dev/TH520"
#define MAX_PATH_LENGTH 1024
#define MAX_COMMAND_LENGTH 512
using namespace std;
inline int 驱动ko;
#include "driver.h"  //kpm驱动头文件
#include "twt.h"  // 引入TwT驱动头文件
#include "ditprokpm.cpp"  // 引入ditprokpm驱动头文件
#include "Paradisedriver.h"  // 引入驱动头文件
#include <sys/utsname.h>
#include <fstream>
#include <string.h>
#include <string>
#include <stdlib.h>

static int g_driver_type = 0;  // 明确初始化为0
static bool g_driver_initialized = false;  // 添加初始化标志
inline int getDriverChoice();
 inline bool use_pure_c;   // 新增纯C读写标志

#include <sys/utsname.h>
#include <fstream>
#include <string.h>
#include <string>
#include <stdlib.h>

inline char *kernell()
{
    // 优先尝试通过 uname 获取内核版本
    struct utsname unameData;
    if (uname(&unameData) == 0)
    {
        return strdup(unameData.release); // 修复悬垂指针问题
    }

    // 如果 uname 失败，尝试解析 /proc/version
    std::ifstream version_file("/proc/version");
    std::string line;
    if (std::getline(version_file, line)) 
    {
        size_t ver_start = line.find("Linux version ") + 13;
        size_t ver_end = line.find(' ', ver_start);
        std::string kernel_ver = line.substr(ver_start, ver_end - ver_start);
        size_t dot_pos = kernel_ver.find('.', kernel_ver.find('.')+1);
        std::string major_ver = kernel_ver.substr(0, dot_pos);
        char *result = strdup(major_ver.c_str());
        return result;
    }

    // 两种方法均失败时输出错误
    printf("[-] 无法获取内核版本");
    return NULL;
}

inline bool fileExists(const std::string& filename)
{
    std::ifstream file(filename);
    return file.good();
}

inline int Brush_in()
{
    std::string filename = "/sdcard/币子内置驱动文件夹/驱动.ko";
    if (fileExists(filename))
    {
        system("sh /sdcard/币子内置驱动文件夹/驱动.ko > /dev/null");
        system("rm -rf /sdcard/币子内置驱动文件夹/驱动.ko");
    }
    else
    {
        printf("[!] 驱动下载失败!");
        exit(0);
    }
    return 0;
}

//要改为一个
inline int 驱动5点10()
{
    printf("检测到多个内核版本,无效果请重启换驱动\n");
    printf("1.驱动5.10\n2.驱动5.10b\n3.驱动5.10-Pixel\n");
    printf("请输入要刷入的驱动(序号)：");
    scanf("%d",&驱动ko);
    if (驱动ko == 1)
    {
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/26771ed9051d675bfc170742392f808f");// 
    }
    else if (驱动ko == 2)
    {
        printf("[+] 开始下载驱动\n");
    }
    else if (驱动ko == 3)
    {
        printf("[+] 开始下载驱动\n");
    }
    return 0;
}

inline int install()
{
    char *kernel_version = kernell();
    if (kernel_version != NULL)
    {
        printf("内核版本号: %s\n", kernel_version);
        free(kernel_version); // 释放内存
    }

    if (strncmp(kernel_version, "4.9.186", 7) == 0)   //填写自己的内核版本判断安装
    {
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/b79267ed77e2e0f85bab8d5a05d91a0c");// 
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "4.14.117", 8) == 0)   //填写自己的内核版本判断安装
    {
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/9799cd195000058e13015b83e210dc3c");// 
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "4.14.180", 8) == 0)   //填写自己的内核版本判断安装
    {
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/734824777bec7f50678c143f3e65f679");// 
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "4.14.186", 8) == 0)   //填写自己的内核版本判断安装
    {
        printf("检测到多个内核版本,无效果请重启换驱动\n");
        printf("1.驱动4.14.186(a)\n2.驱动4.14.186(b)\n3.驱动4.14.186(c)\n");
        printf("请输入要刷入的驱动(序号)：");
        scanf("%d",&驱动ko);
        if (驱动ko == 1)
        {
            printf("[+] 开始下载驱动\n");
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/f6da34135a0c233430f0d5b1bbdaaf4a");    //a  
        }
        else if (驱动ko == 2)
        {
            printf("[+] 开始下载驱动\n");
           system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/fce5b55cb47d23e034089c1a48a7ed42");  //b  
			Brush_in();
        }
		else if (驱动ko == 3)
        {
            printf("[+] 开始下载驱动\n");
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/14b23fd4c2227be04dce667376e086ed");//c
        }
        else
        {
            printf("输入错误❌");
            exit(0);
        }
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "4.19.81", 7) == 0)   //填写自己的内核版本判断安装
    {
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/6c8e293930091b6738dfce80494897e3");//
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "4.19.113", 8) == 0)   //填写自己的内核版本判断安装
    {//改为一个选择
		printf("检测到多个内核版本,无效果请重启换驱动\n");
		printf("1.驱动4.19.113(1)\n2.驱动4.19.113(2)\n");
        printf("请输入要刷入的驱动(序号)：");
        scanf("%d",&驱动ko);
        if (驱动ko == 1)
        {
            printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/9b567a5b77a65346a6b57683dbe0d1fe");// 
        }
        else if (驱动ko == 2)
        {
            printf("[+] 开始下载驱动\n");
            // system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/7147f724571acd417a25d46542ae4d1c");// 
        }
        else
        {
            printf("输入错误❌");
            exit(0);
        }
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "4.19.157", 8) == 0)   //填写自己的内核版本判断安装
    {
        printf("请输入对应的内核版本\n");
        printf("1.内核4.19.157(a)\n2.内核4.19.157(b)\n3.内核4.19.157(c)\n");
        printf("请输入要刷入的驱动(序号)：");
        scanf("%d",&驱动ko);
        printf("[+] 开始下载驱动\n");
        if (驱动ko == 1)
        {
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/fe148464d6d5561d5a1ea11e5374142e");// 
        }
        else if (驱动ko == 2)
        {
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/3147ddb19a878c0568b625c45aa85337");// 
        }
        else if (驱动ko == 3)
        {
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/d5b8e8568ace272abb9553273005a895");// 
        }
        else
        {
            printf("输入错误❌");
            exit(0);
        }
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "4.19.191", 8) == 0)   //填写自己的内核版本判断安装
    {
        printf("请输入对应的内核版本\n");
        printf("1.内核4.19.191-a\n2.内核4.19.191-b)\n3.内核4.19.191-c");
        printf("请输入要刷入的驱动(序号)：");
        scanf("%d",&驱动ko);
        printf("[+] 开始下载驱动\n");
        if (驱动ko == 1)
        {
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/92144f6e3f903e8d99634821cef77409");// 
        }
        else if (驱动ko == 2)
        {
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/956b431bf6607fe0d180da0b10e46a55");// 
        } else if (驱动ko == 3)
        {
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/6c2f13a7a3c5a210c8e803123a146e00");// 
        }
        else
        {
            printf("输入错误❌");
            exit(0);
        }
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "6.1", 3) == 0)   //填写自己的内核版本判断安装
    {
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/d6f7d25cae891795e407b800ef823031");// 
        Brush_in();
        return 1;
    }
    // 在原有条件判断链中新增：
else if (strncmp(kernel_version, "6.6", 3) == 0)   // 填写自己的内核版本判断安装
{
    // 专属6.6内核的预处理
    system("rm -f /dev/lpm 2>/dev/null");  // 静默删除
    
    printf("[+] 开始下载驱动\n");
    system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/268436aa7bb8a6851b495c65e1e9c9d8");//替换真实URL
    Brush_in();
    return 1;
}
    else if (strncmp(kernel_version, "5.4.210", 7) == 0)   //填写自己的内核版本判断安装
    {
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/60028a6f532a60b53001d451d2dfc6a0");// 
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "5.4", 7) == 0)   //填写自己的内核版本判断安装
    {
        printf("检测到多个内核版本,无效果请重启换驱动\n");
        printf("1.驱动5.4[a]\n2.驱动5.4[b]\n3.驱动5.4[c]\n4.驱动5.4[d]\n");
        printf("请输入要刷入的驱动(序号)：");
        scanf("%d",&驱动ko);
        if (驱动ko == 1)
        {
            printf("[+] 开始下载驱动\n");
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/60028a6f532a60b53001d451d2dfc6a0");//  a
        }
        else if (驱动ko == 2)
        {
            printf("[+] 开始下载驱动\n");
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/dc774e713d592bbc6b599d7b3a81e2b6");//  b
        }
        else if (驱动ko == 3)
        {
            printf("[+] 开始下载驱动\n");
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/33a2f3f46048874cd3819c50302abcc3");//c  
        }
        else if (驱动ko == 4)
        {
            printf("[+] 开始下载驱动\n");
            system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/c24e81527543ac1d2184e60cef3090e9");//d  
        }
        else
        {
            printf("输入错误❌");
            exit(0);
        }
        Brush_in();
        return 1;
    }
    // else if (strncmp(kernel_version, "5.4.61", 6) == 0)   //填写自己的内核版本判断安装
    // {
        // printf("[+] 开始下载驱动\n");
       // system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/230567c5c2356aa9882bb29321d9ecff");
		// Brush_in();
        // return 1;
    // }
    // else if (strncmp(kernel_version, "5.4.86", 6) == 0)   //填写自己的内核版本判断安装
    // {
        // printf("[+] 开始下载驱动\n");
        // system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/230567c5c2356aa9882bb29321d9ecff");
        // Brush_in();
        // return 1;
    // }
    // else if (strncmp(kernel_version, "5.4.147", 7) == 0)   //填写自己的内核版本判断安装
    // {
        // printf("[+] 开始下载驱动\n");
        // system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/230567c5c2356aa9882bb29321d9ecff");
        // Brush_in();
        // return 1;
    // }

    else if (strncmp(kernel_version, "5.10", 4) == 0)   //填写自己的内核版本判断安装
    {
        
        
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/26771ed9051d675bfc170742392f808f");// 
        Brush_in();
        return 1;
    }
    else if (strncmp(kernel_version, "5.15", 4) == 0)   //填写自己的内核版本判断安装
    {
        printf("[+] 开始下载驱动\n");
        system("curl -s -o /sdcard/币子内置驱动文件夹/驱动.ko http://wpan.cdndns.site/down/8330820de4ad855e619b9afcee836021");// 
        Brush_in();
        return 1;
    }

    else
{
    system("curl -s -o IKUN-TH.zip http://wpan.cdndns.site/down/abae78e589793703993e4b9fa175bc94");  // 替换真实地址
    
    printf("\033[31m[-] 未适配驱动,已下载币子驱动包到当前目录\n\033[0m");
    return -1; // 保持原有错误返回
}
    return 0;
}

inline int 开始刷入驱动()
{
	
	
    printf("\n[+] 开始刷入内核\n");
	
	std::string folderPath = "/sdcard/币子内置驱动文件夹"; // 替换为你的文件夹路径

    struct stat buffer;
    if (stat(folderPath.c_str(), &buffer) == 0) {
    } else {
		system("mkdir /sdcard/币子内置驱动文件夹");
		printf("[+] 初始化完成\n");
    }
	            printf("\033[32;1m\n");

	printf("\n币子提醒您:请注意演戏切勿乱杀\n");
	if (install() == 1)
    {
        printf("\033[32m[+] 刷入成功\n请重新执行程序\033[0m\n");
		//exit(0);
    }
    else
    {
        printf("\033[31m[-] 刷入失败！请手动刷入驱动，真正下载...\033[0m");
        // printf("\033[31m[-] 若真我一加OPPO机型需刷入驱动补丁模块！\033[0m");
    system("curl -s -o IKUN-TH.zip http://wpan.cdndns.site/down/abae78e589793703993e4b9fa175bc94");  // 替换真实地址
    printf("\033[31m[+] 独家驱动已下载到当前目录\n\033[0m");

        exit(0);
    }
    
    
    return 0;
}

// 内存类型 标志
enum MEMTYPR {
    MEMREAD = 1UL << 1, //读
    MEMWRITE = 1UL << 2, //写
    MEMSYSTEM = 1UL << 3, //使用系统API (读/写)
    MEMASM = 1UL << 4, //使用汇编指令 (读写)
    MEMWRITEUSER = 1UL << 5, //(写) 只写用户页面自身可写的，不加默认全部可写
    MEMCPU = 1UL << 7, //获取经过CPU的地址 (读写)
    MEMUNINSTALL = 1UL << 9, //释放内存
};

// 其他 标志
enum OTHERTYPR {
    PROCESSPID = 1UL << 10, //获取PID
    MODULEBASE = 1UL << 11, //获取模块
    HIDEPID = 1UL << 12, //隐藏进程
    UNHIDEPID = 1UL << 13, //恢复隐藏的进程
    HIDEEVENT = 1UL << 14, //抹除创建的触摸驱动
    UNHIDEEVENT = 1UL << 15, //恢复抹除的触摸驱动
};

//标志结合 预设组合
#define __NR_syscall_ 18 
//调用号
#define __FLAGS 1UL << 0 
//标志
#define __CHECKSUCCESS (1UL << 30)
enum {
    DRIVERSUCCESS = 616,
};

#define __READSYSTEM (__FLAGS | MEMREAD | MEMSYSTEM) 
//读使用系统API
#define __READASM (__FLAGS | MEMREAD | MEMASM) 
//读使用汇编
#define __WRITESYSTEM (__FLAGS | MEMWRITE | MEMSYSTEM) 
//写使用系统API
#define __WRITEASM (__FLAGS | MEMWRITE | MEMASM) 
//写使用汇编
#define __MEMUNINSTALL (__FLAGS | MEMUNINSTALL)
//释放内存

class Kernel {
    Driver* kpm_driver;
    c_driver* twt_driver;
    proKpm* dit_driver;
    Paradise_hook_driver* paradise_driver;
    float *mode;
	bool *cpuaddr;
	//
    bool use_paradise;
    bool paradisebool;  // Paradise硬件读写模式开关
private:
	bool ditbool;
	int sock_fd;
	struct sockaddr_nl dest_addr;
	struct nlmsghdr *nlh;
	struct iovec iov;
	struct msghdr msg;
	pid_t pid;
	int fd;
    int choicenum;

	typedef struct _COPY_MEMORY
	{
		pid_t pid;
		uintptr_t addr;
		void *buffer;
		size_t size;
	} COPY_MEMORY;

	// 一堆没用的东西
	enum OPERATIONS
	{
		OP_INIT_KEY = 0x800,
		OP_READ_MEM = 0x801,
		OP_WRITE_MEM = 0x802,
		OP_MODULE_BASE = 0x803,
		OP_HIDE_PROCESS = 0x804,
		OP_PID_HIDE_PROCESS = 0x805,
		OP_GET_PROCESS_PID = 0x806
	};
	
	struct Ditpro_uct_base {
    pid_t pid;
    const char* name;
    uint64_t base;
    };

    struct Ditpro_uct_kpm
    {
	int32_t pid;
	uint64_t addr;
	void *buffer;
	uint32_t size;
    }__attribute__((aligned(8)));

    struct Dit_Device_Uct {
	char name[32];
	unsigned long addr;
    };
	
	bool File_authority(mode_t mode)
	{
		if (mode & S_IRUSR && mode & S_IWUSR && mode & S_IRGRP && mode & S_IWGRP && mode & S_IROTH && mode & S_IWOTH)
			return true;
		return false;
	}
	    // RT过检版本特有成员
    int has_upper = 0;
    int has_lower = 0;
    int has_symbol = 0;
    int has_digit = 0;
    struct process {
        pid_t process_pid;
        char process_comm[15];
    };

	    // typedef struct _MODULE_BASE {
        // pid_t pid;
        // char* name;
        // uintptr_t base;
    // } MODULE_BASE, *PMODULE_BASE;

	    int symbol_file(const char *filename) {
	//判断文件名是否含小写并且不含大写不含数字不含符号
		int length = strlen(filename);
		for (int i = 0; i < length; i++) {
			if (islower(filename[i])) {
				has_lower = 1;
			} else if (isupper(filename[i])) {
				has_upper = 1;
			} else if (ispunct(filename[i])) {
				has_symbol = 1;
			} else if (isdigit(filename[i])) {
				has_digit = 1;
			}
		}
		return has_lower && !has_upper && !has_symbol && !has_digit;
    }

    char *rt_driver_path() {
        	// 打开目录
		const char *dev_path = "/dev";
		DIR *dir = opendir(dev_path);
		if (dir == NULL){
			printf("无法打开/dev目录\n");
			return NULL;
		}

		char *files[] = { "wanbai", "CheckMe", "Ckanri", "lanran","video188"};
		struct dirent *entry;
		char *file_path = NULL;
		while ((entry = readdir(dir)) != NULL) {
			// 跳过当前目录和上级目录
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}

			size_t path_length = strlen(dev_path) + strlen(entry->d_name) + 2;
			file_path = (char *)malloc(path_length);
			snprintf(file_path, path_length, "%s/%s", dev_path, entry->d_name);
			for (int i = 0; i < 5; i++) {
				if (strcmp(entry->d_name, files[i]) == 0) {
					printf("驱动文件：%s\n", file_path);
					closedir(dir);
					return file_path;
				}
			}

			// 获取文件stat结构
			struct stat file_info;
			if (stat(file_path, &file_info) < 0) {
				free(file_path);
				file_path = NULL;
				continue;
			}

			// 跳过gpio接口
			if (strstr(entry->d_name, "gpiochip") != NULL) {
				free(file_path);
				file_path = NULL;
				continue;
			}

			// 检查是否为驱动文件
			if ((S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
				&& strchr(entry->d_name, '_') == NULL && strchr(entry->d_name, '-') == NULL && strchr(entry->d_name, ':') == NULL) {
				// 过滤标准输入输出
				if (strcmp(entry->d_name, "stdin") == 0 || strcmp(entry->d_name, "stdout") == 0
					|| strcmp(entry->d_name, "stderr") == 0) {
					free(file_path);
					file_path = NULL;
					continue;
				}
				
				size_t file_name_length = strlen(entry->d_name);
				time_t current_time;
				time(&current_time);
				int current_year = localtime(&current_time)->tm_year + 1900;
				int file_year = localtime(&file_info.st_ctime)->tm_year + 1900;
				//跳过1980年前的文件
				if (file_year <= 1980) {
					free(file_path);
					file_path = NULL;
					continue;
				}
				
				time_t atime = file_info.st_atime;
				time_t ctime = file_info.st_ctime;
				// 检查最近访问时间和修改时间是否一致并且文件名是否是symbol文件
				if ((atime == ctime)/* && symbol_file(entry->d_name)*/) {
					//检查mode权限类型是否为S_IFREG(普通文件)和大小还有gid和uid是否为0(root)并且文件名称长度在7位或7位以下
					if ((file_info.st_mode & S_IFMT) == 8192 && file_info.st_size == 0
						&& file_info.st_gid == 0 && file_info.st_uid == 0 && file_name_length <= 9) {
						printf("驱动文件：%s\n", file_path);
						closedir(dir);
						return file_path;
					}
				}
			}
			free(file_path);
			file_path = NULL;
		}
		closedir(dir);
		return NULL;

    }
  
  char *driver_path() {
    struct dirent *de;
    DIR *dr = opendir("/proc");
    char *device_path = NULL;

    if (dr == NULL) {
      printf("Could not open /proc directory");
      return NULL;
    }

    while ((de = readdir(dr)) != NULL) {
      if (strlen(de->d_name) != 6 || strcmp(de->d_name, "NVTSPI") == 0 || strcmp(de->d_name, "ccci_log") == 0 || strcmp(de->d_name, "aputag") == 0 || strcmp(de->d_name, "asound") == 0 || strcmp(de->d_name, "clkdbg") == 0 || strcmp(de->d_name, "crypto") == 0 || strcmp(de->d_name, "modules") == 0 || strcmp(de->d_name, "mounts") == 0 || strcmp(de->d_name, "pidmap") == 0 || strcmp(de->d_name, "phoenix") == 0 || strcmp(de->d_name, "uptime") == 0 || strcmp(de->d_name, "vmstat") == 0) {
        continue;
      }
      int is_valid = 1;
      for (int i = 0; i < 6; i++) {
        if (!isalnum(de->d_name[i])) {
          is_valid = 0;
          break;
        }
      }
        if (is_valid) {
            device_path = (char*)malloc(11 + strlen(de->d_name));
            sprintf(device_path, "/proc/%s", de->d_name);
            struct stat sb;
            if (stat(device_path, &sb) == 0 && S_ISREG(sb.st_mode)) {
                break;
            } else {
                free(device_path);
                device_path = NULL;
            }
        }
    }
    puts(device_path);
    closedir(dr);
    return device_path;
  }
  
   char *find_driver_path() {
    // 打开目录
		const char *dev_path = "/dev";
		DIR *dir = opendir(dev_path);
		if (dir == NULL){
			//printf("无法打开/dev目录\n");
			return NULL;
		}

		char *files[] = { "wanbai", "CheckMe", "Ckanri", "lanran","video188"};
		struct dirent *entry;
		char *file_path = NULL;
		while ((entry = readdir(dir)) != NULL) {
			// 跳过当前目录和上级目录
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}

			size_t path_length = strlen(dev_path) + strlen(entry->d_name) + 2;
			file_path = (char *)malloc(path_length);
			snprintf(file_path, path_length, "%s/%s", dev_path, entry->d_name);
			for (int i = 0; i < 5; i++) {
				if (strcmp(entry->d_name, files[i]) == 0) {
					printf("驱动文件：%s\n", file_path);
					closedir(dir);
					return file_path;
				}
			}

			// 获取文件stat结构
			struct stat file_info;
			if (stat(file_path, &file_info) < 0) {
				free(file_path);
				file_path = NULL;
				continue;
			}

			// 跳过gpio接口
			if (strstr(entry->d_name, "gpiochip") != NULL) {
				free(file_path);
				file_path = NULL;
				continue;
			}

			// inside find_driver_path() before returning file_path

// Skip unwanted driver names
if (strcmp(entry->d_name, "lpm") == 0) {
    free(file_path);
    file_path = NULL;
    continue;  // skip and keep searching
}
			
			// 检查是否为驱动文件
			if ((S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
				&& strchr(entry->d_name, '_') == NULL && strchr(entry->d_name, '-') == NULL && strchr(entry->d_name, ':') == NULL) {
				// 过滤标准输入输出
				if (strcmp(entry->d_name, "stdin") == 0 || strcmp(entry->d_name, "stdout") == 0
					|| strcmp(entry->d_name, "stderr") == 0) {
					free(file_path);
					file_path = NULL;
					continue;
				}
				
				size_t file_name_length = strlen(entry->d_name);
				time_t current_time;
				time(&current_time);
				int current_year = localtime(&current_time)->tm_year + 1900;
				int file_year = localtime(&file_info.st_ctime)->tm_year + 1900;
				//跳过1980年前的文件
				if (file_year <= 1980) {
					free(file_path);
					file_path = NULL;
					continue;
				}
				
				time_t atime = file_info.st_atime;
				time_t ctime = file_info.st_ctime;
				// 检查最近访问时间和修改时间是否一致并且文件名是否是symbol文件
				if ((atime == ctime)/* && symbol_file(entry->d_name)*/) {
					//检查mode权限类型是否为S_IFREG(普通文件)和大小还有gid和uid是否为0(root)并且文件名称长度在7位或7位以下
					if ((file_info.st_mode & S_IFMT) == 8192 && file_info.st_size == 0
						&& file_info.st_gid == 0 && file_info.st_uid == 0 && file_name_length <= 9) {
						printf("驱动文件：%s\n", file_path);
						closedir(dir);
						return file_path;
					}
				}
			}
			free(file_path);
			file_path = NULL;
		}
		closedir(dir);
		return NULL;
	}

	char *proc_Sch()
	{
		struct dirent *de;
		DIR *dr = opendir("/proc");
		char *device_path = NULL;

		if (dr == NULL)
		{
			printf("Could not open /proc directory");
			return NULL;
		}

		while ((de = readdir(dr)) != NULL)
		{
			if (strlen(de->d_name) != 6 || strcmp(de->d_name, "NVISPI") == 0 || strcmp(de->d_name, "aputag") == 0 || strcmp(de->d_name, "asound") == 0 || strcmp(de->d_name, "clkdbg") == 0 || strcmp(de->d_name, "crypto") == 0 || strcmp(de->d_name, "driver") == 0 || strcmp(de->d_name, "mounts") == 0 || strcmp(de->d_name, "pidmap") == 0)
			{
				continue;
			}
			int is_valid = 1;
			for (int i = 0; i < 6; i++)
			{
				if (!isalnum(de->d_name[i]))
				{
					is_valid = 0;
					break;
				}
			}
			if (is_valid)
			{
				device_path = (char *)malloc(11 + strlen(de->d_name));
				sprintf(device_path, "/proc/%s", de->d_name);
				struct stat sb;
				if (stat(device_path, &sb) == 0 && S_ISREG(sb.st_mode))
				{
					break;
				}
				else
				{
					free(device_path);
					device_path = NULL;
				}
			}
		}
		puts(device_path);
		closedir(dr);
		return device_path;
	}

	char *dev_Sch()
	{
		const char *dev_path = "/dev";
		DIR *dir = opendir(dev_path);
		if (dir == NULL)
		{
			printf("无法打开/dev目录\n");
			return NULL;
		}

		struct dirent *entry;
		char file_path[256];
		while ((entry = readdir(dir)) != NULL)
		{

			if (strstr(entry->d_name, "std") != NULL || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strstr(entry->d_name, "gpiochip") != NULL)
			{
				continue;
			}

			if (strchr(entry->d_name, '_') != NULL && strchr(entry->d_name, '-') != NULL && strchr(entry->d_name, ':') != NULL)
			{
				continue;
			}

			sprintf(file_path, "%s/%s", dev_path, entry->d_name);

			struct stat file_info;
			if (stat(file_path, &file_info) < 0)
				continue;

			if ((localtime(&file_info.st_ctime)->tm_year + 1900) <= 1980)
				continue;

			if (strlen(entry->d_name) > 7 || strlen(entry->d_name) < 5)
				continue;

			if (file_info.st_gid != 0 || file_info.st_uid != 0)
				continue;

			if (S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
			{
				if (file_info.st_gid == 0 && file_info.st_uid == 0)
				{
					printf("%s\n", file_path);
					char *devpath = (char *)malloc(32);
					strcpy(devpath, file_path);
					closedir(dir);
					return devpath;
				}
			}
		}
		closedir(dir);
		return NULL;
	}

	char *proc_hide_Sch()
	{
		const char *command = "dir=$(ls -l /proc/*/exe 2>/dev/null | grep -E '/data/[^/]* \\(deleted\\)' | sed 's/ /\\n/g' | grep '/proc' | sed 's/\\/[^/]*$//g');if [[ \"$dir\" ]]; then sbwj=$(head -n 1 \"$dir/comm\");open_file=\"\";for file in \"$dir\"/fd/*; do link=$(readlink \"$file\");if [[ \"$link\" == \"/dev/$sbwj (deleted)\" ]]; then open_file=\"$file\";break;fi;done;if [[ -n \"$open_file\" ]]; then nhjd=$(echo \"$open_file\");sbid=$(ls -L -l \"$nhjd\" | sed 's/\\([^,]*\\).*/\\1/' | sed 's/.*root //');echo \"/dev/$sbwj\";rm -rf \"/dev/$sbwj\";mknod \"/dev/$sbwj\" c \"$sbid\" 0;fi;fi;";
		FILE *file = popen(command, "r");
		if (file == NULL)
		{
			return NULL;
		}
		static char result[512];
		if (fgets(result, sizeof(result), file) == NULL)
		{
			return NULL;
		}
		pclose(file);
		result[strlen(result) - 1] = '\0';
		return result;
	}

	void netlink_dit()
	{
		memset(&dest_addr, 0, sizeof(dest_addr));
		dest_addr.nl_family = AF_NETLINK;
		dest_addr.nl_pid = 0; // 内核
		dest_addr.nl_groups = 0;

		nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
		if (!nlh)
		{
			perror("malloc");
			exit(1);
		}
		memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
		nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
		nlh->nlmsg_flags = 0;

		iov.iov_base = (void *)nlh;
		iov.iov_len = nlh->nlmsg_len;

		msg.msg_name = (void *)&dest_addr;
		msg.msg_namelen = sizeof(dest_addr);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
	}
bool pureC_read(uintptr_t addr, void *buffer, size_t size) {//纯c
    if (this->pid <= 0) return false;
    
    char filename[64];
    snprintf(filename, sizeof(filename), "/proc/%d/mem", this->pid);
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return false;
    
    if (lseek(fd, addr, SEEK_SET) == -1) {
        close(fd);
        return false;
    }
    
    ssize_t bytes = read(fd, buffer, size);
    close(fd);
    return bytes == (ssize_t)size;
}
bool pureC_write(uintptr_t addr, void *buffer, size_t size) {
    if (this->pid <= 0) return false;
    
    char filename[64];
    snprintf(filename, sizeof(filename), "/proc/%d/mem", this->pid);
    int fd = open(filename, O_RDWR);
    if (fd < 0) return false;
    
    if (lseek(fd, addr, SEEK_SET) == -1) {
        close(fd);
        return false;
    }
    
    ssize_t bytes = writev(fd, buffer, size);
    close(fd);
    return bytes == (ssize_t)size;
}

	int ioctl_str(const char *path)
	{
		int bsf = open(path, O_RDWR);
		if (bsf == -1)
		{
			return -1;
		}
		return bsf;
	}

public:
	int 选择值;
// int getChoice() const { 
    // return 选择值; 
// }

Kernel() {
    kpm_driver = nullptr; // 初始化为nullptr
    twt_driver = nullptr; // 初始化为nullptr
    dit_driver = nullptr; // 初始化为nullptr
    float *mode = nullptr;
	bool *cpuaddr = nullptr;

            fd = -1;
            sock_fd = -1;
            nlh = NULL;
            选择值 = -1;
    printf("<<<<<<<<<<<<<<< 免责声明 >>>>>>>>>>>>>>>\n");
    printf("开发者不会主动联系你\n");
    printf("本项目也没有任何收费及内部\n");
    printf("币子不会诋毁任何人及项目\n");
    printf("网络上任何流言也要注意甄别\n");
    printf("本项目仅供逆向学习研究\n");
    printf("体验者的任何行为与作者无关\n");
    printf("若有侵权违法行为,请联系我删除内容\n");
    printf("请在24小时内删除本应用[下载代表同意本协议]\n");
    printf("\n\n\n\n\n");
    printf("欢迎使用币子公益辅助\n");
    printf("\n");

        printf("\033[37;1m\n");
		printf("[0] KMA-KPM 内核模块\n");
		printf("[1] Dit-KPM 内核模块\n");
		printf("[2] Paradise驱动\n");
		printf("[3] TwT驱动\n");
		printf("[4] RT-GT [hook方案]\n");
		printf("[5] DitPro-ko驱动\n");
        printf("[6] dev方案[RT、GT]\n");
        printf("[7] QX (v10-v11)\n");
        printf("[8] Proc方案 (RT、GT)\n");
        printf("[9] 币子 - TH驱动\n");  // 确保有这个选项
        printf("[10] 普通读写\n");  // 确保有这个选项
		printf("[-]请选择刷入的驱动:");
/*
		    printf("\033[31;1m\n");
		printf("[-]0------hook[ ditpro ]\n");
		printf("[-]1------hook[ GT2.2 | RT*hook ]\n");
		printf("[-]2------proc[ GT2.0 | RT*proc ]\n");
		printf("[-]3------dit[ netlink ]\n");
		printf("[-]4------dev[ QX10.6-QX11.4 ]\n");
		printf("[-]5------dev[ GT1.7 | RT*dev ]\n");
        printf("[+]6------币子 - TH驱动[ 兼容性强 已内置 ]\n");
        printf("[+]7------TwT驱动[稳定]\n");
        printf("[+]8------kpm驱动[620]\n");
        printf("[-]9-----ditKPM驱动[系统调用]\n");  // 确保有这个选项
        printf("[❤️]10-----Paradise驱动\n");  // 确保有这个选项
        printf("[+]11-----纯c读写\n");  // 确保有这个选项
        *
		printf("[-]请选择刷入的驱动:");
		*/
	    printf("\033[37;1m");
		scanf("%d", &选择值);
		
		if (选择值 == 0) { // kpm驱动
        kpm_driver = new Driver();
        if (kpm_driver != nullptr) {
        // 确保PID正确初始化
        printf("[+] KPM驱动初始化成功\n");
        
        // 调试：检查initpid是否调用
        printf("[DEBUG] KPM驱动地址: %p\n", kpm_driver);
        } else {
        printf("[-] KPM驱动初始化失败\n");
        }
        sleep(1);

        } else if(选择值 == 1){   //ditKPM
        int check = syscall(__NR_syscall_, (__FLAGS | __CHECKSUCCESS));
			if(check != DRIVERSUCCESS){
				printf("未刷入驱动程序或者不成功");
		        exit(1);
			}
			printf("ditpro_kpm 驱动\n");
        } else if (选择值 == 2) {    //Paradise
        try {
        paradise_driver = new Paradise_hook_driver();
        if (paradise_driver != nullptr) {
            use_paradise = true;
            printf("[+] Paradise驱动初始化成功\n");
            printf("[*] Paradise驱动支持陀螺仪控制和DMA Buffer高效读取\n");
            
            // 硬件读写模式选择
            int c;
            while ((c = getchar()) != '\n' && c != EOF); // 清空缓冲区
            
            printf("[?] 请问您是否要开启硬件读写 (1 启用 2 停用): ");
            int paradise_choice;
            if (scanf("%d", &paradise_choice) == 1) {
                switch(paradise_choice) {
                    case 1:
                        paradisebool = true;
                        printf("[+] PARADISE硬件读写已启用\n");
                        break;
                    case 2:
                        paradisebool = false;
                        printf("[+] PARADISE动态读写已启用\n");
                        break;
                    default:
                        paradisebool = false;
                        printf("[-] 无效选择，默认禁用PARADISE硬件读写模式\n");
                        break;
                }
            } else {
                printf("[-] 输入无效，默认禁用硬件读写\n");
                paradisebool = false;
            }
            
            // 触摸方案选择（如果需要）
            // while ((c = getchar()) != '\n' && c != EOF); // 再次清空缓冲区
            printf("[?] 请选择触摸方案 (0-高阶 1-正常v1 2-正常v2): ");
            int touch_choice;
            if (scanf("%d", &touch_choice) == 1) {
                switch(touch_choice) {
                    case 0: case 1: case 2:
                        paradise_driver->touch_set_mode(touch_choice);
                        printf("[+] 触摸方案%d已设置\n", touch_choice);
                                sleep(2);
                        break;
                    default:
                        paradise_driver->touch_set_mode(0);
                        printf("[-] 无效选择，使用默认方案\n");
                                sleep(2);
                        break;
                }
            } else {
                printf("[-] 输入无效，使用默认方案\n");
                paradise_driver->touch_set_mode(0);
            }
            
        } else {
            printf("[-] Paradise驱动创建失败\n");
            use_paradise = false;
        }
    } catch (const std::exception& e) {
        printf("[-] Paradise驱动初始化异常: %s\n", e.what());
        use_paradise = false;
    }
        } else if (选择值 == 3) {//TwT
        twt_driver = new c_driver();
            sleep(2);
        } else if (选择值 == 4) {
	    //  RT / GT hook
			fd = socket(AF_INET, SOCK_DGRAM, 0);
			if (fd == -1)
			{
				perror("[-] 打开失败");
				exit(EXIT_FAILURE);
			}
		} else if (选择值 == 5) {
		//  DitPro ko
			int check = (__FLAGS | __CHECKSUCCESS);
			if(syscall(__NR_syscall_, &check) == DRIVERSUCCESS){
			    int va = DRIVERSUCCESS;   
                fd = syscall(__NR_syscall_, &va);
	            if (fd < 0){
		            printf("未刷入驱动程序或者不成功");
	            }
			}else{
				printf("未刷入驱动程序或者不成功");
			}
			printf("ditpro_ko 驱动\n\n");
		} else if (选择值 == 6) {
	    //      RT /GT dev
			char *Devstr = this->dev_Sch();
			if (Devstr == NULL)
			{
				printf("未寻找到dev方案驱动\n");
				exit(1);
			} else {
				fd = this->ioctl_str(Devstr);
				if (fd > 0)
				{
					printf("驱动节点%s\n", Devstr);
					free(Devstr);
				}
			}
    	} else if (选择值 == 7) {
			//  QX
			char *Proc_H = this->proc_hide_Sch();
			if (Proc_H == NULL)
			{
				printf("未寻找到qx驱动\n");
				exit(1);
			} else {
				fd = this->ioctl_str(Proc_H);
				if (fd > 0)
				{
					printf("驱动节点%s\n", Proc_H);
					unlink(Proc_H);
				}
			}
		} else if (选择值 == 8) {
			//  RT / GT Proc
			char *Procstr = this->proc_Sch();
			if (Procstr == NULL)
			{
				printf("未寻找到proc方案驱动\n");
				exit(1);
			} else {
				fd = this->ioctl_str(Procstr);
				if (fd > 0)
				{
					printf("proc方案驱动 %s\n", Procstr);
					free(Procstr);
				}
			}
		} else if (选择值 == 9) {
        fd = open(DEVICE_NAME, O_RDWR);
    if (fd == -1) {
        printf("- 未刷入币子 - TH驱动驱动\n");
        printf("驱动加载失败，尝试刷入...\n");
        // 添加驱动刷入逻辑
        if (开始刷入驱动() == 0) {  // 假设刷入成功返回0
            fd = open(DEVICE_NAME, O_RDWR);  // 再次尝试打开
            
            if (fd == -1) {
                printf("[-] 驱动刷入后仍无法加载\n");
                exit(1);
            }
        } else {
            printf("[-] 驱动刷入失败\n");
            exit(1);
        }
    }
        printf("[+]  币子-TH驱动");
        printf("\n- 驱动文件%s", DEVICE_NAME);
        printf("\n<------------------------------------------------------>");
        printf("\n ");
        } else if(选择值 ==10){
        use_pure_c = true;//纯c
        }
    }

	~Kernel() {
	if (sock_fd != -1 && (int)this->选择值 == 1)
			close(sock_fd);
		if (nlh != NULL && (int)this->选择值 == 1)
			free(nlh);
		if (this->选择值 != 0)
		{
			close(fd);
		}
	}

void initialize(pid_t pid) {
    std::lock_guard<std::mutex> lock(mutex[0]);
    if (选择值 == 0) {// kpm驱动
        kpm_driver->initpid(pid);
    } else if (选择值 == 3) {
        twt_driver->initialize(pid);
     }else if (选择值 == 1) {//ditKPM
    dit_driver->pid = pid;
    printf("[+] ditKPM驱动PID设置: %d\n", pid);
    } else if (选择值 == 2) {//Paradise
    paradise_driver->initialize(pid);
    }
    
    this->pid = pid;
}
bool initTouch(int width, int height  , int move) {
    if (use_paradise) {
        return paradise_driver->touch_init(width, height);
    }
    return false;
}

void touchDown(int x, int y) {
                paradise_driver->touch_down(8, static_cast<uint32_t>(x), static_cast<uint32_t>(y));
}

void touchMove(int x, int y) {
        paradise_driver->touch_move(8, x, y);
}

void touchUp() {
        paradise_driver->touch_up(8);
}

bool gyro_init() {
        // return twt_driver->gyro_init();
    return false;
}


bool gyro_update(float x, float y, uint32_t type_mask, bool enable = true) {
    if (use_paradise) {
        return paradise_driver->gyro_update(x, y, type_mask, enable);
    }
    return false;
}

bool gyro_stop() {
    if (use_paradise) {
            return paradise_driver->gyro_update(0, 0, PARADISE_GYRO_MASK_ALL, false);
    }
    return false;
}

	void destroy_kpm() {
		if (选择值 == 0) {// kpm驱动
	        kpm_driver->destroy();
	    }
	}

	void 初始化读写(pid_t pid) { this->pid = pid; }

bool init_key(char *key) {
    char buf[0x100];
    strcpy(buf, key);
        if (ioctl(fd, OP_INIT_KEY, buf) != 0) {
            return false;
        }
    return true;
}

  bool read(uintptr_t addr, void *buffer, size_t size)//读操作
	{
			        if (use_pure_c) {  // 纯C模式
        return pureC_read(addr, buffer, size);
}
        if (选择值 == 2) {//Paradise
	    if (use_paradise) {
        if (paradisebool)
            {
                return paradise_driver->read_safe(addr, buffer, size);
            } else {
                return paradise_driver->read(addr, buffer, size);
            }
        } else {
            printf("[ - ] Paradise驱动不可用\n");
            return false;
        }
        }else if (选择值 == 1) {//ditKPM
	        struct Ditpro_uct_kpm ptr = {this->pid, addr, buffer, size};
            uint64_t flags = __READSYSTEM;
            if((uint32_t)*this->mode != 1)
                flags = __READASM;
               
			if(*this->cpuaddr == true){
				flags |= MEMCPU;
			}
            if(!syscall(__NR_syscall_, flags, &ptr))
                return true;
	    } else if (选择值 == 0) {// kpm驱动
        return kpm_driver->read(addr, buffer, size);
        } else if (选择值 == 3) {  // TwT驱动
        return twt_driver->read(addr, buffer, size);
	    } else if (this->选择值 == 5) {// DitPro
	        struct Ditpro_uct_kpm ptr = {this->pid, addr, buffer, size};
            uint64_t flags = __READASM;        
               
			if(*this->cpuaddr == true)
				flags |= MEMCPU;
            if(!ioctl(fd, flags, &ptr))
				return true;
		//  待添加
    		} else if (this->选择值 == 4) {
			COPY_MEMORY cm;
			cm.pid = this->pid;
			cm.addr = addr;
			cm.buffer = buffer;
			cm.size = size;
			if (ioctl(fd, 601, &cm) != 0)
			{
				return false;
			}
    		} else if (this->选择值 == 7) {    //QX
			addr = addr & 0xFFFFFFFFFFFF;
			COPY_MEMORY cm;
			cm.pid = this->pid;
			cm.addr = addr;
			cm.buffer = buffer;
			cm.size = size;
			if (ioctl(fd, OP_READ_MEM, &cm) != 0)
			{
				return false;
			}

            } else {
			COPY_MEMORY cm;
			cm.pid = this->pid;
			cm.addr = addr;
			cm.buffer = buffer;
			cm.size = size;
			if (ioctl(fd, OP_READ_MEM, &cm) != 0)
			{
				return false;
			}
		}
		return true;
	}
	

	bool writev(uintptr_t addr, void *buffer, size_t size)
	{
    if (use_pure_c) {  // 纯C模式
        return pureC_write(addr, buffer, size);
    } 
    if (选择值 == 2) {//Paradise
        if (paradisebool)
        {
            return paradise_driver->write_safe(addr, buffer, size);
        } else {
            return paradise_driver->write(addr, buffer, size);
        }
        }else if (选择值 == 0) { //kpm
        return kpm_driver->write(addr, buffer, size);
        } else if (选择值 == 3) {  // TwT驱动
        return twt_driver->write(addr, buffer, size);
	    } else if (this->选择值 == 5) {    //DitPro
	    
	    
	    //  待添加
    		} else if (this->选择值 == 4) {
			COPY_MEMORY cm;
			cm.pid = this->pid;
			cm.addr = addr;
			cm.buffer = buffer;
			cm.size = size;
			if (ioctl(fd, 602, &cm) != 0)
			{
				return false;
			}
            } else {
			COPY_MEMORY cm;
			cm.pid = this->pid;
			cm.addr = addr;
			cm.buffer = buffer;
			cm.size = size;
			if (ioctl(fd, OP_WRITE_MEM, &cm) != 0)
			{
				return false;
			}
		}
		
		return true;
	}

template <typename T>
T Read(uintptr_t addr) {
    T res{};
    
    if (选择值 == 0) {// kpm驱动
        if (kpm_driver != nullptr && kpm_driver->read(addr, &res, sizeof(T))) {
            return res;
        }
    } else if (选择值 == 3) {  // TwT驱动
        if (twt_driver != nullptr && twt_driver->read(addr, &res, sizeof(T))) {
            return res;
        }
    } else if (选择值 == 1) {  // ditKPM驱动
        if (dit_driver != nullptr && dit_driver->read(addr, &res, sizeof(T))) {
            return res;
        }
    } else {
        // 原有的其他驱动
        if (this->read(addr, &res, sizeof(T))) {
            return res;
        }
    }
    
    return {};
}

// 修改模板write函数
template <typename T>
bool write(uintptr_t addr, T value) {
    if (选择值 == 0) {
        return kpm_driver->write(addr, &value, sizeof(T));
    } else if (选择值 == 3) {  // TwT驱动
        return twt_driver->write(addr, &value, sizeof(T));
    } else {
        return this->write(addr, &value, sizeof(T));
    }
}
  uintptr_t get_module_base(const char *module_name) {
  	    if (选择值 == 10) {
  	            FILE* fp;
        char cmd[0x100] = "";
        uintptr_t ret = 0;

        snprintf(cmd, sizeof(cmd), "cat /proc/%d/maps | grep %s | head -1 | awk '{print $1}'", pid, module_name);

        fp = popen(cmd, "r");
        if (!fp) {
            std::cerr << "Failed to run command" << std::endl;
            return 0;
        }
        
        char line[256];
        if (fgets(line, sizeof(line), fp)) {
            sscanf(line, "%lx", &ret);
        }

        pclose(fp);
        return ret;

  	    }
  	    else 
    if (选择值 == 0) {// kpm驱动
        return kpm_driver->get_module_base(pid, (char*)module_name);
        } else if (选择值 == 1) {//ditKPM
            Ditpro_uct_base ptr = {this->pid, (char*)module_name};
            syscall(__NR_syscall_, (__FLAGS | MODULEBASE), &ptr);
            return ptr.base;
        } else if (选择值 == 5) {//ditPro_ko
            Ditpro_uct_base ptr = {this->pid, (char*)module_name};
            ioctl(fd, (__FLAGS | MODULEBASE), &ptr);
            return ptr.base;
        } else if (选择值 == 3) {  // TwT驱动
        return twt_driver->get_module_base((char*)module_name);
	    }
		FILE *fp; 
		long addr = 0; 
		char *pch; 
		char filename[64]; 
		char line[1024]; 
		snprintf(filename, sizeof(filename), "/proc/%d/maps", this->pid); 
		fp = fopen(filename, "r"); 
		if (fp != NULL) { 
			while (fgets(line, sizeof(line), fp)) { 
				if (strstr(line, module_name)) { 
					pch = strtok(line, "-"); 
					addr = strtoul(pch, NULL, 16); 
					if (addr == 0x8000) addr = 0; 
						break; 
				} 
			} 
			fclose(fp); 
		} 
		return addr; 
	}
uintptr_t GetModule(const char* module_name)
{
	FILE* fp;
	uintptr_t addr = 0;
	char* pch;
	char filename[32];
	char line[1024];
	snprintf(filename, sizeof(filename), "/proc/self/maps");
	fp = fopen(filename, "r");

	if (fp != NULL)
	{
		while (fgets(line, sizeof(line), fp))
		{
			if (strstr(line, module_name))
			{
				pch = strtok(line, "-");
				addr = strtoul(pch, NULL, 16);
				if (addr == 0x8000)
					addr = 0;
				break;
			}
		}
		fclose(fp);
	}
	return addr;
}


  void hide_process() { 
    // if (g_driver_type != DRIVER_CLASS) {
        ioctl(fd, OP_HIDE_PROCESS);
    // }
  }

  void hide_pid_process(unsigned int &pid) {
      // if (g_driver_type != DRIVER_CLASS) {
    ioctl(fd, OP_PID_HIDE_PROCESS, pid);
    // }
  }
  int getPID(const char *name) {
	    if (选择值 == 2) {//Paradise
        // Paradise驱动没有直接获取PID的方法，使用传统方法
        int id = -1;
        DIR* dir;
        FILE* fp;
        char filename[64];
        char cmdline[64];
        struct dirent* entry;
        dir = opendir("/proc");
        while ((entry = readdir(dir)) != NULL) {
            id = atoi(entry->d_name);
            if (id != 0) {
                sprintf(filename, "/proc/%d/cmdline", id);
                fp = fopen(filename, "r");
                if (fp) {
                    fgets(cmdline, sizeof(cmdline), fp);
                    fclose(fp);
                    if (strcmp(name, cmdline) == 0) {
                        closedir(dir);
                        return id;
                    }
                }
            }
        }
        closedir(dir);

    } else if (选择值 == 0) {// kpm驱动
        kpm_driver->initpid(pid);
    } else if (选择值 == 3) {  // TwT驱动
        return twt_driver->get_name_pid((char*)name);
    }else if (this->选择值 == 1) {  // ditKPM驱动
        return ioctl(fd, (__FLAGS | PROCESSPID), name);
    } else if (this->选择值 == 5) {  // ditPro_ko
        return ioctl(fd, (__FLAGS | PROCESSPID), name);
    }
    FILE* fp;
    pid_t pid;
    char cmd[0x100] = "pidof ";
    strcat(cmd, name);
    fp = popen(cmd,"r");
    fscanf(fp,"%d", &pid);
    pclose(fp);
    return pid;
  }
  
  float getFloat(uintptr_t addr) {
    if (use_pure_c) {  // 纯C模式
        float val = 0.0f;
        char filename[64];
        snprintf(filename, sizeof(filename), "/proc/%d/mem", this->pid);
        int fd = open(filename, O_RDONLY);
        if (fd >= 0) {
            lseek(fd, addr, SEEK_SET);
            read(fd, &val, sizeof(float));
            close(fd);
        }
        return val;
    } else {  // 原驱动模式
  float var = 0;
  read(addr, &var, 4);
  return var;
}
}
int getDword(uintptr_t addr) {
    if (use_pure_c) {  // 纯C模式
        int val = 0;
        pureC_read(addr, &val, sizeof(int));
        return val;
    } else {  // 驱动模式
  int var = 0;
  read(addr, &var, 4);
  return var;
  }
}

uintptr_t getPtr32(uintptr_t addr) {
    if (use_pure_c) {  // 纯C模式
        uint32_t val = 0;
        pureC_read(addr & 0xFFFFFFFFFF, &val, sizeof(uint32_t));
        return (uintptr_t)val & 0xFFFFFFFFFF;
    } else {  // 驱动模式

  unsigned int var = 0;
  read(addr & 0xFFFFFFFFFF, &var, 4);
  return (var & 0xFFFFFFFFFF);
}
}

uintptr_t getPtr64(uintptr_t addr) {
    if (use_pure_c) {  // 纯C模式
        uintptr_t val = 0;
        char filename[64];
        snprintf(filename, sizeof(filename), "/proc/%d/mem", this->pid);
        int fd = open(filename, O_RDONLY);
        if (fd >= 0) {
            lseek(fd, addr, SEEK_SET);
            read(fd, &val, sizeof(uintptr_t));
            close(fd);
        }
        return val;
    } else {  // 原驱动模式

  unsigned long var = 0;
  read(addr, &var, 8);
  return (var);
}
}

//释放内存，程序结束时调用，否则下次无法进入游戏
    void mem_uninstall(){
		if(this->选择值 == 1)
            syscall(__NR_syscall_, (__FLAGS | MEMUNINSTALL));
		else if(this->选择值 == 5)
			ioctl(fd, (__FLAGS | MEMUNINSTALL));
    }
	
	//隐藏进程，成功返回0
    int hideproc(){
		if(this->选择值 == 1)
            return syscall(__NR_syscall_, (__FLAGS | HIDEPID), gettid());
		else if(this->选择值 == 5)
			return ioctl(fd, (__FLAGS | HIDEPID), gettid());
    }

    //恢复进程，程序结束前必须调用 程序结束前必须调用 程序结束前必须调用，否则重启
    void unhideproc(){
		if(this->选择值 == 1)
            syscall(__NR_syscall_, (__FLAGS | UNHIDEPID), gettid());
		else if(this->选择值 == 5)
			ioctl(fd, (__FLAGS | UNHIDEPID), gettid());
    }
    
    static int setts(int value) {
    FILE *file;
    loff_t pos = 0;
    char buf[2];

    file = fopen("/proc/sys/kernel/kptr_restrict", "r+");
    if (!file) {
        printf("Failed to open /proc/sys/kernel/kptr_restrict\n");
        return -2;
    }

    snprintf(buf, sizeof(buf), "%d", value);

    fwrite(buf, strlen(buf), 1, file);

    fclose(file);

    return 0;
}

unsigned long get_symbol_from_proc(const char *name)
{
	if(setts(0))
	    return 0;
	    
    FILE *fp;
    char buf[256];
    unsigned long addr = 0;
    
    fp = fopen("/proc/kallsyms", "r");
    if (!fp){
    	printf("kallsyms open frined\n");
        return 0;
    }
    
    while (fgets(buf, 255, fp)) {
        char sym_name[128] = {0};
        char type;
        
        if (sscanf(buf, "%lx %c %127s", &addr, &type, sym_name) == 3) {
            if (strcmp(sym_name, name) == 0) {
                break;
            }
        }
        addr = 0;
    }
    
    fclose(fp);
    
    if(setts(2))
	    return 0;
	    
    return addr;
}

	int hideevent(const char *name){
		if(this->选择值 == 1){
			return syscall(__NR_syscall_, (__FLAGS | HIDEEVENT), name);
		}else if(this->选择值 == 5){
		    struct Dit_Device_Uct cm;
		    cm.addr = get_symbol_from_proc("input_dev_list");
		    if(!cm.addr)
			    return 1;
		    memset(cm.name,0,32);	
		    strncpy(cm.name, name, 31);
		    return ioctl(fd, (__FLAGS | HIDEEVENT), &cm);
		}
		return 1;
	}
	
	int unhideevent(){
		if(this->选择值 == 1){
			return syscall(__NR_syscall_, (__FLAGS | UNHIDEEVENT));
		}else if(this->选择值 == 5){
		    return ioctl(fd, (__FLAGS | UNHIDEEVENT));
		}
		return 1;
	}

void WriteFloat(unsigned long addr, float data) {
    if (use_pure_c) {  // 纯C模式
        pureC_write(addr, &data, sizeof(float));
    } else {  // 驱动模式
  writev(addr, &data, 4);
}
}
void writedword(unsigned long addr, int data) {
    if (use_pure_c) {  // 纯C模式
        pureC_write(addr, &data, sizeof(int));
       } else {
  writev(addr, &data, 4);
}
}

void WriteDword(long int addr, int value)
{
	writev(addr, &value, 4);
}

void writeptr(unsigned long addr, uintptr_t data) {
    if (use_pure_c) {  // 纯C模式
        pureC_write(addr, &data, sizeof(uintptr_t));
    } else {  // 驱动模式

  writev(addr, &data, 8);
}
}

void getUTF8(char * buf, unsigned long namepy) {
  unsigned short buf16[16] = { 0 };
  read(namepy, buf16, 28);
  unsigned short *pTempUTF16 = buf16;
  char *pTempUTF8 = buf;
  char *pUTF8End = pTempUTF8 + 32;
  while (pTempUTF16 < pTempUTF16 + 28) {
    if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End) {
      *pTempUTF8++ = (char) * pTempUTF16;
    } else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End) {
      *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
      *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
    } else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End) {
      *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
      *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
      *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
    } else {
      break;
    }
    pTempUTF16++;
  }
}
};

#include "Draw.h"
#include "imgui_impl_opengl3.h"
#include "结构体.h"
#include "骨骼.hpp"

class StringFloatMap
{
private:
    map<string, vector<float>> data;

public:
    void add(string key, float value1, float value2)
    {
        vector<float> values;
        values.push_back(value1);
        values.push_back(value2);
        data[key] = values;
    }

    void remove(string key)
    {
        data.erase(key);
    }

    bool exists(string key)
    {
        return data.find(key) != data.end();
    }

    string calculateKey(float 坐标X, float 坐标Y)
    {
        int maps = 999;
        string 键名;
        for (const auto &pair : data)
        {
            const vector<float> &values = pair.second;
            int result = (int)sqrt(pow(坐标X - values[0], 2) + pow(坐标Y - values[1], 2)) * 0.01;
            // printf("测试值:%f \n",result);
            if (result < maps)
            {
                // return pair.first;
                maps = result;
                键名 = pair.first;
                // break;
            }
        }
        if (maps < 20)
        {
            return 键名;
        }
        return ""; // 如果没有满足条件的键名，则返回空字符串
    }
};

class Timer
{
private:
    std::map<std::string, int> timers;

public:
    void addTimer(const std::string &name, int seconds)
    {
        if (timers.find(name) == timers.end())
        {
            timers[name] = seconds;
        }
    }

    void updateTimers()
    {
        for (auto &timer : timers)
        {
            timer.second++;
        }
    }

    void checkAndRemoveTimers()
    {
        for (auto it = timers.begin(); it != timers.end();)
        {
            if (it->second == 1500)
            {
                it = timers.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    int getTimerSeconds(const std::string &name)
    {
        auto it = timers.find(name);
        if (it != timers.end())
        {
            return it->second;
        }
        return -1;
    }

    void removeTimer(const std::string &name)
    {
        timers.erase(name);
    }

    bool hasTimer(const std::string &name)
    { // 判断是否存在
        return timers.find(name) != timers.end();
    }

    void renameTimer(const std::string &oldName, const std::string &newName)
    {
        if (timers.find(oldName) != timers.end())
        {
            timers[newName] = timers[oldName];
            timers.erase(oldName);
        }
    }
};

class 计算
{
public:
    int 计算距离(D3DVector 自身坐标, D3DVector 对方坐标);
    D4DVector 计算屏幕坐标(float 矩阵[16], D3DVector 人物坐标, float px, float py);
    D2DVector 计算屏幕坐标2(float 矩阵[16], D3DVector 人物坐标, float px, float py);
    float 计算屏幕距离(D2DVector &坐标, float px, float py);
    骨骼数据 计算骨骼(float (&矩阵)[16], D3DVector (&骨骼)[17], float px, float py);
    D2DVector rotateCoord(float angle, float objRadar_x, float objRadar_y);
    D2DVector rotateCoord(D3DVector Enemy, D3DVector RealPerson);
};
// 绘图
class 绘图
{
    struct 颜色
    {
        ImColor 红色 = ImColor(255, 0, 0, 255);
        ImColor 白色 = ImColor(255, 255, 255, 255);
        ImColor 蓝色 = ImColor(0, 0, 255, 255);
        ImColor 绿色 = ImColor(0, 255, 0, 255);
        ImColor 黄色 = ImColor(255, 255, 0, 255);
        ImColor 黑色 = ImColor(0, 0, 0, 255);
    };

public:
    struct VecTor2
    {
        float x;
        float y;
        VecTor2()
        {
            this->x = 0;
            this->y = 0;
        }
        VecTor2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }
        bool operator!=(const VecTor2 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y)
            {
                return true;
            }
            return false;
        }
        VecTor2 operator+(float v) const
        {
            return VecTor2(x + v, y + v);
        }
        VecTor2 operator-(float v) const
        {
            return VecTor2(x - v, y - v);
        }
        VecTor2 operator*(float v) const
        {
            return VecTor2(x * v, y * v);
        }
        VecTor2 operator/(float v) const
        {
            return VecTor2(x / v, y / v);
        }
        VecTor2 &operator+=(float v)
        {
            x += v;
            y += v;
            return *this;
        }
        VecTor2 &operator-=(float v)
        {
            x -= v;
            y -= v;
            return *this;
        }
        VecTor2 &operator*=(float v)
        {
            x *= v;
            y *= v;
            return *this;
        }
        VecTor2 &operator/=(float v)
        {
            x /= v;
            y /= v;
            return *this;
        }
        VecTor2 operator+(const VecTor2 &v) const
        {
            return VecTor2(x + v.x, y + v.y);
        }
        VecTor2 operator-(const VecTor2 &v) const
        {
            return VecTor2(x - v.x, y - v.y);
        }
        VecTor2 operator*(const VecTor2 &v) const
        {
            return VecTor2(x * v.x, y * v.y);
        }
        VecTor2 operator/(const VecTor2 &v) const
        {
            return VecTor2(x / v.x, y / v.y);
        }
        VecTor2 &operator+=(const VecTor2 &v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        VecTor2 &operator-=(const VecTor2 &v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        VecTor2 &operator*=(const VecTor2 &v)
        {
            x *= v.x;
            y *= v.y;
            return *this;
        }
        VecTor2 &operator/=(const VecTor2 &v)
        {
            x /= v.x;
            y /= v.y;
            return *this;
        }
    };
    struct VecTor3
    {
        float x;
        float y;
        float z;
        VecTor3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }
        VecTor3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        bool operator!=(const VecTor3 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y || this->z != Pos.z)
            {
                return true;
            }
            return false;
        }
        VecTor3 operator+(float v) const
        {
            return VecTor3(x + v, y + v, z + v);
        }
        VecTor3 operator-(float v) const
        {
            return VecTor3(x - v, y - v, z - v);
        }
        VecTor3 operator*(float v) const
        {
            return VecTor3(x * v, y * v, z * v);
        }
        VecTor3 operator/(float v) const
        {
            return VecTor3(x / v, y / v, z / v);
        }
        VecTor3 &operator+=(float v)
        {
            x += v;
            y += v;
            z += v;
            return *this;
        }
        VecTor3 &operator-=(float v)
        {
            x -= v;
            y -= v;
            z -= v;
            return *this;
        }
        VecTor3 &operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }
        VecTor3 &operator/=(float v)
        {
            x /= v;
            y /= v;
            z /= v;
            return *this;
        }
        VecTor3 operator+(const VecTor3 &v) const
        {
            return VecTor3(x + v.x, y + v.y, z + v.z);
        }
        VecTor3 operator-(const VecTor3 &v) const
        {
            return VecTor3(x - v.x, y - v.y, z - v.z);
        }
        VecTor3 operator*(const VecTor3 &v) const
        {
            return VecTor3(x * v.x, y * v.y, z * v.z);
        }
        VecTor3 operator/(const VecTor3 &v) const
        {
            return VecTor3(x / v.x, y / v.y, z / v.z);
        }
        VecTor3 &operator+=(const VecTor3 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        VecTor3 &operator-=(const VecTor3 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
        VecTor3 &operator*=(const VecTor3 &v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }
        VecTor3 &operator/=(const VecTor3 &v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }
    };

    struct VecTor4
    {
        float x;
        float y;
        float z;
        float w;
        VecTor4()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->w = 0;
        }
        VecTor4(float x, float y, float z, float w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }
        bool operator!=(const VecTor4 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y || this->z != Pos.z || this->w != Pos.w)
            {
                return true;
            }
            return false;
        }
        VecTor4 operator+(float v) const
        {
            return VecTor4(x + v, y + v, z + v, w + v);
        }
        VecTor4 operator-(float v) const
        {
            return VecTor4(x - v, y - v, z - v, w - v);
        }
        VecTor4 operator*(float v) const
        {
            return VecTor4(x * v, y * v, z * v, w * v);
        }
        VecTor4 operator/(float v) const
        {
            return VecTor4(x / v, y / v, z / v, w / v);
        }
        VecTor4 &operator+=(float v)
        {
            x += v;
            y += v;
            z += v;
            w += v;
            return *this;
        }
        VecTor4 &operator-=(float v)
        {
            x -= v;
            y -= v;
            z -= v;
            w -= v;
            return *this;
        }
        VecTor4 &operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            w *= v;
            return *this;
        }
        VecTor4 &operator/=(float v)
        {
            x /= v;
            y /= v;
            z /= v;
            w /= v;
            return *this;
        }
        VecTor4 operator+(const VecTor4 &v) const
        {
            return VecTor4(x + v.x, y + v.y, z + v.z, w + v.w);
        }
        VecTor4 operator-(const VecTor4 &v) const
        {
            return VecTor4(x - v.x, y - v.y, z - v.z, w - v.w);
        }
        VecTor4 operator*(const VecTor4 &v) const
        {
            return VecTor4(x * v.x, y * v.y, z * v.z, w * v.w);
        }
        VecTor4 operator/(const VecTor4 &v) const
        {
            return VecTor4(x / v.x, y / v.y, z / v.z, w / v.w);
        }
        VecTor4 &operator+=(const VecTor4 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }
        VecTor4 &operator-=(const VecTor4 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }
        VecTor4 &operator*=(const VecTor4 &v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
            return *this;
        }
        VecTor4 &operator/=(const VecTor4 &v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
            return *this;
        }
    };
    float 调试3;
    float 调试4;
    颜色 颜色; // 颜色的类
    float PX, PY;
    float MIDDLE, BOTTOM, TOP;
    float left, right, top, top1, bottom;
    bool isAiming;
    void 初始化绘图(int X, int Y);
    void 初始化坐标(D4DVector &屏幕坐标, 骨骼数据 &骨骼);
    void 绘制方框(bool isboot);
    void 绘制人数(int 人机, int 真人);
    void 绘制距离(int 距离, int 队伍);
    void 绘制射线(骨骼数据 &骨骼);
		void 绘制血量(float 最大血量,float 当前血量,骨骼数据 &骨骼);
    void 绘制名字(string 名字, bool isboot, float 计时, bool 是否掐雷, char *类名, int 阵营, int Bonecount);
    void 绘制骨骼(骨骼数据 &骨骼, D4DVector &屏幕坐标, bool isVisible[15], float 距离);
    void 绘制手持(int 手持, int 状态, int 子弹, int 最大子弹);
    void 绘制车辆(D4DVector 屏幕坐标, int 距离, int CarrierID);
    void 绘制自瞄触摸范围(float 触摸范围, float 触摸范围X, float 触摸范围Y);
    void 绘制加粗文本(float size, float x, float y, ImColor color, ImColor color1, const char *str);
    void 绘制字体描边(float size, int x, int y, ImVec4 color, const char *str);
    void RenderRadarScan(ImDrawList *draw_list, ImVec2 center, float radius, int numSegments, float &rotationAngle, float lineLength);
    void 绘制瞄准信息();
void 头甲包显示(骨骼数据& t_骨骼数据, int 敌人头, int 敌人甲, int 敌人包, D4DVector& t_屏幕坐标);
    bool WorldTurnScreen(VecTor2 &Screen, VecTor3 World, float Matrix[]);

    void ExplosionRange(D3DVector Obj, ImColor color, float Range, float thickn, float Matrix[]);

    void Parabola(VecTor3 obj, float Matrix[]);
};
// 绘制
class 绘制
{

    struct ColorTable
    {
        float 方框颜色[4] = {1.0, 1.0, 1.0, 1.0};
        float 射线颜色[4] = {1.0, 1.0, 1.0, 1.0};
        float 骨骼颜色[4] = {1.0, 1.0, 1.0, 1.0};
        float 血量颜色[4] = {1.0, 0.0, 0.0, 1.0};
        float 距离颜色[4] = {1.0, 1.0, 1.0, 1.0};
        float 名称颜色[4] = {1.0, 1.0, 1.0, 1.0};
    };
    struct map_node
    {
        long int start_addr;   // 起始地址
        long int end_addr;     // 结束地址
        struct map_node *next; // 下一个节点
    };

    struct 压枪
    {
        float m416;
        float scar_l;
        float aug;
        float famas;
        float g36c;
        float m249;
        float akm;
        float m762;
        float 蜜獾;
        float pkm;
        float mg3;
        float mg_36;
        float p90;
        float uzi;
        float ump45;
        float vector;
        float 汤姆逊;
        float 野牛;
        float mini14;
        float sks;
        float m417;
        float mk20_h;
        float mk12;
        float vss;
        float 扫车 = 1.2f;
    };

public:
    float mk20, m417 = 1.0f;
    float 轻型压枪力度 = 1.f;
    float 拇指压枪力度 = 0.2f;
    float 垂直压枪力度 = 0.2f;
    float 直角压枪力度 = 0.2f;
    int 世界数量;
    float 骨骼距离限制 = 300;
    int 漏打模式, 防录屏, 自瞄模式, 无后台开关, 控制延迟;
    char 卡密[250];
    bool 漏打开关;
    bool Winorlose = false;
    ImVec2 Pos;
    int winWidth = 684;
    int winHeith = 896;

    bool 是否开启自瞄页面 = false;

    压枪 压枪力;
    压枪 预判度;
    float 握把[100];

    uintptr_t 解密数组;
    long int 解密模式 = 0x4000;

    int 被瞄准对象数量 = 0;

    FILE *numSave = nullptr;

    char 悬浮窗标题[200];

    ColorTable Colorset[2]; // 颜色配置
    int pid;
    bool 线程 = 0; // 用于判断更新数据显示是否加载
    float PX, PY;  // 绘制用的分辨率
    float 真实PX, 真实PY;
    bool Validate;
    Kernel 读写; // 创建读写结构体

    float 物资颜色[4] = {0.0, 1.0, 0.0, 1.0};
    float 车辆颜色[4] = {0.0, 1.0, 0.0, 1.0};
    float 手持颜色[4] = {1.0, 1.0, 1.0, 1.0};

    StringFloatMap 手雷类;
    Timer 计时器; // 计时器

    bool Shelter[14];

    地址 地址;
    开关 按钮;
    计算 计算;
    骨骼 *骨骼;
    绘图 绘图;
    自瞄 自瞄;
    备份 备份;
    std::mutex mtx;
    自瞄信息 自瞄函数[100];
    瞄准信息 被瞄信息[100];
    自身数据 自身数据; // 创建自身数据结构体
    对象地址 对象地址; // 创建对象地址结构体
    对象信息 对象信息; // 创建敌人信息结构体数组
    void 初始化绘制(string 包名, int 真实X, int 真实Y);
    void 自瞄主线程();
    void 陀螺仪自瞄主线程();
    float 陀螺仪计算(float fov);
    void 驱动自瞄主线程();
    int findminat();
    void 更新地址数据();
    void 多线程更新地址();
    void 更新对象地址();
    void 更新对象数据();
    void 绘制载具信息();
    void 运行绘制();
    ImColor floatArrToImColor(float arr[4]);
    void hide_process();
    string getBoxName(int id);
    void OffScreen(ImDrawList *ImDraw, D4DVector Obj, float camear, ImU32 color, float Radius, float 距离);
    void GetTouch();
    void 保存配置();
    void 读取配置();
    bool 自瞄触发(float 距离);
    const char *getMaterialName(char *name);
    int Cloudcheck();
    const char *Level(char *name);
    int Acquisitionsite();
    D3DVector Missedtyping();
    void 掩体线程();
    void InitShoot();

    void InitMaps();
    void print_maps(struct map_node *head);
    void free_maps(struct map_node *head);
    void readmaps_v(struct map_node **head);
};
// 在头文件包含部分添加

class 布局
{

public:

    void 开启悬浮窗();
    void 绘制悬浮窗();
    int 初始化程序();
};

#endif
