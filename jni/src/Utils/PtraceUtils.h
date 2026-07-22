/* 
 * by币子
 
 
 */

#include <asm/ptrace.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <elf.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "Utils.h"
#if defined(__aarch64__) //x64
#define pt_regs user_pt_regs
#define uregs regs
#define ARM_pc pc
#define ARM_sp sp
#define ARM_cpsr pstate
#define ARM_lr regs[30]
#define ARM_r0 regs[0]
#define PTRACE_GETREGS PTRACE_GETREGSET
#define PTRACE_SETREGS PTRACE_SETREGSET
#elif defined(__x86_64__) //x86_64
#define pt_regs user_regs_struct
#define eax rax
#define esp rsp
#define eip rip
#elif defined(__i386__) //x86
#define pt_regs user_regs_struct
#endif

// rest predefined
#define CPSR_T_MASK (1u << 5)
struct pt_regs currentRegs, originalRegs;//无模块
//Attach ptrace to process using the processID
int ptraceAttach(pid_t pid) {
    int status = 0;
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0) {
        LOGE("ptraceAttach error, pid: %d, error: %s", pid, strerror(errno));
        return -1;
    }

    LOGI("ptraceAttach success");
    waitpid(pid, &status, WUNTRACED);

    return 0;
}

//Used to the attached process doesn't stop
int ptraceContinue(pid_t pid) {
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0) {
        LOGE("ptraceContinue error, pid: %d, error: %s", pid, strerror(errno));
        return -1;
    }

    LOGI("ptraceContinue success");
    return 0;
}

//Detach from the target process
int ptraceDetach(pid_t pid) {
    if (ptrace(PTRACE_DETACH, pid, NULL, 0) < 0) {
        LOGE("ptraceDetach error, pid: %d, err: %s", pid, strerror(errno));
        return -1;
    }

    LOGI("ptraceDetach success");
    return 0;
}

//get the register value of the process
int ptrace_getregs(pid_t pid, struct pt_regs *regs) {
    #if defined(__aarch64__)
        int regset = NT_PRSTATUS;
        struct iovec ioVec;

        ioVec.iov_base = regs;
        ioVec.iov_len = sizeof(*regs);
        if (ptrace(PTRACE_GETREGSET, pid, (void *)regset, &ioVec) < 0){
            LOGE("ptrace_getregs: Can not get register values, io %llx, %d\n", ioVec.iov_base,ioVec.iov_len);
            return -1;
        }

        return 0;
    #else
        if (ptrace(PTRACE_GETREGS, pid, NULL, regs) < 0) {
            LOGE("Get Ptrace regs error, error: %s\n", strerror(errno));
            return -1;
        }
    #endif
    return 0;
}

//set the register value of the process
int ptrace_setregs(pid_t pid, struct pt_regs *regs){
    #if defined(__aarch64__)
        int regset = NT_PRSTATUS;
        struct iovec ioVec;

        ioVec.iov_base = regs;
        ioVec.iov_len = sizeof(*regs);
        if (ptrace(PTRACE_SETREGSET, pid, (void *)regset, &ioVec) < 0) {
            LOGE("ptrace_setregs: Can not get register values");
            return -1;
        }

        return 0;
    #else
        if (ptrace(PTRACE_SETREGS, pid, NULL, regs) < 0) {
            LOGE("Set Regs error, pid:%d, err:%s\n", pid, strerror(errno));
            return -1;
        }
    #endif
    return 0;
}

//Return value of any function in the process
//Return value is stored in the corresponding register
long ptrace_getret(struct pt_regs *regs) {
    #if defined(__i386__) || defined(__x86_64__) //
        return regs->eax;
    #elif defined(__arm__) || defined(__aarch64__) //
        return regs->ARM_r0;
    #else
        LOGE("Device not supported");
    #endif
}

long ptrace_getpc(struct pt_regs *regs) {
    #if defined(__i386__) || defined(__x86_64__)
        return regs->eip;
    #elif defined(__arm__) || defined(__aarch64__)
        return regs->ARM_pc;
    #else
        LOGE("Device not supported");
    #endif
}

//get the dlopen address of the target process
void *getDlOpenAddr(pid_t pid) {
    void *dlOpenAddress;

    //Emulators such as noxplayer have different sdk versions
    //This works for android 5 - 7
    //If the emulator has a higher android version, adjust this
    if (arch == deviceArchitecture::x86 || arch == deviceArchitecture::x86_64) {
        dlOpenAddress = getRemoteFuncAddr(pid, linkerPath, (void *) dlopen);
    } else {
        if (sdkVersion <= 23) { // Android 7
            dlOpenAddress = getRemoteFuncAddr(pid, linkerPath, (void *) dlopen);
        } else {
            dlOpenAddress = getRemoteFuncAddr(pid, libDLPath, (void *) dlopen);
        }
    }

    LOGE("dlopen getRemoteFuncAddr: 0x%lx", (uintptr_t)dlOpenAddress);
    return dlOpenAddress;
}

//get the dlsym address of the target process
//can be used in the future to call a specific function in the injected process
void *getDlsymAddr(pid_t pid) {
    void *dlSymAddress;

    if (arch == deviceArchitecture::x86 || arch == deviceArchitecture::x86_64) {
        dlSymAddress = getRemoteFuncAddr(pid, linkerPath, (void *) dlopen);
    } else {
        if (sdkVersion <= 23) {
            dlSymAddress = getRemoteFuncAddr(pid, linkerPath, (void *) dlsym);
        } else {
            dlSymAddress = getRemoteFuncAddr(pid, libDLPath, (void *) dlsym);
        }
    }

    LOGE("dlsym getRemoteFuncAddr: 0x%lx", (uintptr_t) dlSymAddress);
    return dlSymAddress;
}

//get the dlerror address of the target process
void *getDlerrorAddr(pid_t pid) {
    void *dlErrorAddress;

    if (arch == deviceArchitecture::x86 || arch == deviceArchitecture::x86_64) {
        dlErrorAddress = getRemoteFuncAddr(pid, linkerPath, (void *) dlopen);
    } else {
        if (sdkVersion <= 23) {
            dlErrorAddress = getRemoteFuncAddr(pid, linkerPath, (void *) dlerror);
        } else {
            dlErrorAddress = getRemoteFuncAddr(pid, libDLPath, (void *) dlerror);
        }
    }

    LOGE("dlerror getRemoteFuncAddr: 0x%lx", (uintptr_t) dlErrorAddress);
    return dlErrorAddress;
}

//read data from the target process
int ptrace_readdata(pid_t pid, uint8_t *pSrcBuf, uint8_t *pDestBuf, size_t size) {
    long nReadCount = 0;
    long nRemainCount = 0;
    uint8_t *pCurSrcBuf = pSrcBuf;
    uint8_t *pCurDestBuf = pDestBuf;
    long lTmpBuf = 0;
    long i = 0;

    nReadCount = size / sizeof(long);
    nRemainCount = size % sizeof(long);

    for (i = 0; i < nReadCount; i++) {
        lTmpBuf = ptrace(PTRACE_PEEKTEXT, pid, pCurSrcBuf, 0);
        memcpy(pCurDestBuf, (char *) (&lTmpBuf), sizeof(long));
        pCurSrcBuf += sizeof(long);
        pCurDestBuf += sizeof(long);
    }

    if (nRemainCount > 0) {
        lTmpBuf = ptrace(PTRACE_PEEKTEXT, pid, pCurSrcBuf, 0);
        memcpy(pCurDestBuf, (char *) (&lTmpBuf), nRemainCount);
    }

    return 0;
}

//write data to the target process
int ptrace_writedata(pid_t pid, uint8_t *pWriteAddr, uint8_t *pWriteData, size_t size) {
    long nWriteCount = 0;
    long nRemainCount = 0;
    uint8_t *pCurSrcBuf = pWriteData;
    uint8_t *pCurDestBuf = pWriteAddr;
    long lTmpBuf = 0;
    long i = 0;

    nWriteCount = size / sizeof(long);
    nRemainCount = size % sizeof(long);

    mprotect(pWriteAddr, size, PROT_READ | PROT_WRITE | PROT_EXEC);

    for (i = 0; i < nWriteCount; i++) {
        memcpy((void *)(&lTmpBuf), pCurSrcBuf, sizeof(long));
        if (ptrace(PTRACE_POKETEXT, pid, (void *)pCurDestBuf, (void *)lTmpBuf) < 0) {
            LOGE("Write Remote Memory error, MemoryAddr: 0x%lx, error:%s\n", (uintptr_t)pCurDestBuf, strerror(errno));
            return -1;
        }
        pCurSrcBuf += sizeof(long);
        pCurDestBuf += sizeof(long);
    }

    if (nRemainCount > 0) {
        lTmpBuf = ptrace(PTRACE_PEEKTEXT, pid, pCurDestBuf, NULL);
        memcpy((void *)(&lTmpBuf), pCurSrcBuf, nRemainCount);
        if (ptrace(PTRACE_POKETEXT, pid, pCurDestBuf, lTmpBuf) < 0){
            LOGE("Write Remote Memory error, MemoryAddr: 0x%lx, err:%s\n", (uintptr_t)pCurDestBuf, strerror(errno));
            return -1;
        }
    }
    return 0;
}

//call a function on the target process
int ptrace_call(pid_t pid, uintptr_t ExecuteAddr, long *parameters, long num_params,struct pt_regs *regs) {
    #if defined(__i386__)
        regs->esp -= (num_params) * sizeof(long);
        if (0 != ptrace_writedata(pid, (uint8_t *)regs->esp, (uint8_t *)parameters,(num_params) * sizeof(long))){
            return -1;
        }

        long tmp_addr = 0x0;
        regs->esp -= sizeof(long);
        if (0 != ptrace_writedata(pid, (uint8_t *)regs->esp, (uint8_t *)&tmp_addr, sizeof(tmp_addr))){
            return -1;
        }

        regs->eip = ExecuteAddr;

        if (ptrace_setregs(pid, regs) == -1 || ptraceContinue(pid) == -1) {
            return -1;
        }

        int stat = 0;
        waitpid(pid, &stat, WUNTRACED);

        LOGI("ptrace call return value is %d", stat);
        while (stat != 0xb7f) {
            if (ptraceContinue(pid) == -1) {
                LOGE("ptrace call error");
                return -1;
            }
            waitpid(pid, &stat, WUNTRACED);
        }

        if (ptrace_getregs(pid, regs) == -1) {
            return -1;
        }

    #elif defined(__x86_64__)
        LOGE("Ptrace call x86_64");
        int num_param_registers = 6;
        if (num_params > 0)
            regs->rdi = parameters[0];
        if (num_params > 1)
            regs->rsi = parameters[1];
        if (num_params > 2)
            regs->rdx = parameters[2];
        if (num_params > 3)
            regs->rcx = parameters[3];
        if (num_params > 4)
            regs->r8 = parameters[4];
        if (num_params > 5)
            regs->r9 = parameters[5];

        if (num_param_registers < num_params){
            regs->esp -= (num_params - num_param_registers) * sizeof(long); //Allocate stack space, the direction of the stack is from high address to low address
            if (0 != ptrace_writedata(pid, (uint8_t *)regs->esp, (uint8_t *)&parameters[num_param_registers], (num_params - num_param_registers) * sizeof(long))){
                return -1;
            }
        }

        long tmp_addr = 0x0;
        regs->esp -= sizeof(long);
        if (0 != ptrace_writedata(pid, (uint8_t *)regs->esp, (uint8_t *)&tmp_addr, sizeof(tmp_addr))) {
            return -1;
        }

        regs->eip = ExecuteAddr;

        if (ptrace_setregs(pid, regs) == -1 || ptrace_continue(pid) == -1) {
            return -1;
        }

        int stat = 0;
        waitpid(pid, &stat, WUNTRACED);

        while (stat != 0xb7f){
            if (ptrace_continue(pid) == -1){
                //printf("[-] ptrace call error");
                return -1;
            }
            waitpid(pid, &stat, WUNTRACED);
        }

    #elif defined(__arm__) || defined(__aarch64__)
    #if defined(__arm__)
        int num_param_registers = 4;
    #elif defined(__aarch64__) // 64-bit real machine
        int num_param_registers = 8;
    #endif
        int i = 0;
        for (i = 0; i < num_params && i < num_param_registers; i++){
            regs->uregs[i] = parameters[i];
        }

        if (i < num_params) {
            regs->ARM_sp -= (num_params - i) * sizeof(long);
            if (ptrace_writedata(pid, (uint8_t *)(regs->ARM_sp), (uint8_t *)&parameters[i], (num_params - i) * sizeof(long)) == -1)
                return -1;
        }

        regs->ARM_pc = ExecuteAddr;
        if (regs->ARM_pc & 1){
            regs->ARM_pc &= (~1u);
            regs->ARM_cpsr |= CPSR_T_MASK;
        } else {
            regs->ARM_cpsr &= ~CPSR_T_MASK;
        }

        regs->ARM_lr = 0;

        long lr_val = 0;
        char sdk_ver[32];
        memset(sdk_ver, 0, sizeof(sdk_ver));
        __system_property_get("ro.build.version.sdk", sdk_ver);
        if (atoi(sdk_ver) <= 23) {
            lr_val = 0;
        } else { // Android 7.0
            static long start_ptr = 0;
            if (start_ptr == 0){
                start_ptr = (long)getModuleBaseAddr1(pid, libcPath);
            }
            lr_val = start_ptr;
        }
        regs->ARM_lr = lr_val;

        if (ptrace_setregs(pid, regs) == -1 || ptraceContinue(pid) == -1){
            return -1;
        }

        int stat = 0;
        waitpid(pid, &stat, WUNTRACED);

        while ((stat & 0xFF) != 0x7f){
            if (ptraceContinue(pid) == -1) {
                return -1;
            }
            waitpid(pid, &stat, WUNTRACED);
        }

        if (ptrace_getregs(pid, regs) == -1){
            return -1;
        }

    #else
        LOGE("Unsupported device")
    #endif
    return 0;
}


//无模块
/**
 * @brief 获取SO文件大小并向上对齐到4KB页大小
 *
 * @param lib_path SO文件路径
 * @return size_t 对齐后的内存大小
 */
size_t get_so_size_aligned(const char *lib_path)
{
    struct stat st;
    if (stat(lib_path, &st) != 0)
    {
        return 0x4096; // 默认大小
    }

    size_t file_size = st.st_size;
    // 向上对齐到4KB页大小
    size_t aligned_size = ((file_size + 4096 - 1) / 4096) * 4096;

    // 确保最小内存大小
    if (aligned_size < 0x3000)
    {
        aligned_size = 0x3000;
    }

    return aligned_size;
}
pid_t get_pid_by_name(const char *name)
{
    FILE *fp;
    pid_t pid;
    char cmd[0x100] = "pidof ";

    strcat(cmd, name);
    fp = popen(cmd, "r");
    fscanf(fp, "%d", &pid);
    pclose(fp);
    return pid;
}
void *get_mmap_address(pid_t pid)
{
    return getRemoteFuncAddr(pid, libcPath, (void *)mmap);
}
void *get_munmap_address(pid_t pid)
{
    return getRemoteFuncAddr(pid, libcPath, (void *)munmap);
}
void enableSELinux()
{
    char line[1024];
    FILE *fp = fopen("/proc/mounts", "r");
    if (fp == NULL)
    {
        return; // 无法打开文件，直接返回
    }

    while (fgets(line, 1024, fp))
    {
        if (strstr(line, "selinuxfs"))
        {
            strtok(line, " ");
            char *selinux_dir = strtok(NULL, " ");

            // 为了避免缓冲区溢出，使用更安全的方式构建路径
            char selinux_path[1024];
            snprintf(selinux_path, sizeof(selinux_path), "%s/enforce", selinux_dir);

            FILE *fp_selinux = fopen(selinux_path, "w");
            if (fp_selinux != NULL)
            {
                char buf[2] = "1"; // 1 = Enforcing
                fwrite(buf, strlen(buf), 1, fp_selinux);
                fclose(fp_selinux);
            }
            break;
        }
    }
    fclose(fp);
}

void *callMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset, int pid)
{
    void *mmap_addr = get_mmap_address(pid);
    long parameters[6];
    parameters[0] = (uintptr_t)addr; // 设置为NULL表示让系统自动选择分配内存的地址
    parameters[1] = size;            // 映射内存的大小
    parameters[2] = prot;            // 表示映射内存区域 可读|可写|可执行
    parameters[3] = flags;           // 建立匿名映射
    parameters[4] = fd;              //  若需要映射文件到内存中，则为文件的fd
    parameters[5] = offset;          // 文件映射偏移量
    // 调用远程进程的mmap函数申请内存
    if (ptrace_call(pid, (uintptr_t)mmap_addr, parameters, 6, &currentRegs) == -1)
    {
        return nullptr;
    }
    // 获取mmap函数执行后的返回值
    return (void *)ptrace_getret(&currentRegs);
}
bool callMunmap(void *remoteAddr, size_t size, int pid)
{
    void *munmapAddr = get_munmap_address(pid);
    long parameters[2];
    parameters[0] = (uintptr_t)remoteAddr; // 申请的内存区域地址头
    parameters[1] = size;
    // 调用远程进程的munmap函数 卸载内存
    if (ptrace_call(pid, (uintptr_t)munmapAddr, parameters, 2, &currentRegs) == -1)
    {
        return false;
    }
    return true;
}

/**
 * @brief 改进的ptrace注入函数，专门处理system_server
 *
 * @param package_name 目标进程包名
 * @param so_path 要注入的so文件路径
 * @return int 0表示成功，-1表示失败
 */
int PtraceInject(const char *package_name, const char *so_path)
{
    setupSystemLibs();
    if (isSELinuxEnabled() == 1)
    {
        disableSELinux();
    }
    // 根据包名获取PID
    pid_t pid = get_pid_by_name(package_name);
    if (pid == 0)
    {
        // printf("[-] 找不到包名为 %s 的进程\n", package_name);
        return -1;
    }
    // printf("[+] 找到目标进程: %s (PID: %d)\n", package_name, pid);

    // 检查so文件是否存在
    if (access(so_path, R_OK) != 0)
    {
        // printf("[-] so文件不存在或不可读: %s\n", so_path);
        return -1;
    }

    // attach到目标进程
    if (ptraceAttach(pid) != 0)
    {
        // printf("[-] 附加到进程失败\n");
        return -1;
    }
    // printf("[+] 成功附加到目标进程\n");

    int result = -1;
    void *remote_memory = NULL;

    do
    {

        // 获取当前寄存器
        if (ptrace_getregs(pid, &currentRegs) != 0)
        {
            // printf("[-] 获取寄存器失败\n");
            break;
        }

        // 保存原始寄存器
        memcpy(&originalRegs, &currentRegs, sizeof(currentRegs));

        // 尝试不同的内存分配策略
        size_t alloc_size = get_so_size_aligned(so_path); // 对齐到4KB

        // printf("[+] 尝试分配内存: size=0x%zx\n", alloc_size);

        // 首先尝试可读写权限
        remote_memory = callMmap(NULL, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0, pid);
        // enhanced_mmap(pid, alloc_size, PROT_READ | PROT_WRITE | PROT_EXEC);
        if (!remote_memory)
        {
            // printf("[-] 内存分配尝试失败\n");
            break;
        }
        // printf("[+] 在目标进程中分配内存成功: 0x%lx\n", (uintptr_t)remote_memory);

        // 将so路径写入目标进程
        // printf("[+] 写入so路径到目标进程: %s\n", so_path);
        if (ptrace_writedata(pid, (uint8_t *)remote_memory, (uint8_t *)so_path, strlen(so_path)) == -1)
        {
            // printf("[-] 写入so路径到目标进程失败\n");
            break;
        }
        // printf("[+] 成功写入so路径到目标进程\n");

        // 获取dlopen地址
        void *dlopen_addr = getDlOpenAddr(pid);
        if (!dlopen_addr)
        {
            // printf("[-] 获取dlopen地址失败\n");
            break;
        }
        // printf("[+] 获取dlopen地址成功: 0x%lx\n", (uintptr_t)dlopen_addr);

        // 准备调用dlopen的参数
        long parameters[2];
        parameters[0] = (uintptr_t)remote_memory; // so路径地址
        parameters[1] = RTLD_NOW | RTLD_GLOBAL;
        ; // 标志位

        // printf("[+] 调用dlopen加载so...\n");

        // 调用dlopen加载so
        if (ptrace_call(pid, (uintptr_t)dlopen_addr, parameters, 2, &currentRegs) == -1)
        {
            // printf("[-] 调用dlopen失败\n");
            break;
        }

        // 获取dlopen返回值
        void *so_handle = (void *)ptrace_getret(&currentRegs);
        // printf("[+] dlopen返回值: 0x%lx\n", (uintptr_t)so_handle);

        if ((long)so_handle == 0x0)
        {
            // printf("[-] dlopen返回空句柄，so加载失败\n");

            // 尝试获取错误信息
            void *dlerror_addr = getDlerrorAddr(pid);
            if (dlerror_addr)
            {
                long dlerror_params[0];
                if (ptrace_call(pid, (uintptr_t)dlerror_addr, dlerror_params, 0, &currentRegs) != -1)
                {
                    void *error_msg = (void *)ptrace_getret(&currentRegs);
                    if (error_msg && (long)error_msg != 0)
                    {
                        char error_buf[256] = {0};
                        if (ptrace_readdata(pid, (uint8_t *)error_msg, (uint8_t *)error_buf, sizeof(error_buf) - 1) == 0)
                        {
                            printf("[-] dlerror: %s\n", error_buf);
                        }
                    }
                }
            }
            break;
        }

        // printf("[+] so加载成功，句柄: 0x%lx\n", (uintptr_t)so_handle);

        // 恢复原始寄存器
        if (ptrace_setregs(pid, &originalRegs) == -1)
        {
            // printf("[-] 恢复寄存器失败\n");
            break;
        }

        // printf("[+] SO注入成功完成!\n");
        result = 0;

    } while (false);

    // 清理：如果失败且分配了内存，尝试释放
    if (result == -1 && remote_memory)
    {
        // printf("[+] 清理分配的内存...\n");
        size_t alloc_size = 0x1000; // 假设分配了1页
        callMunmap(remote_memory, alloc_size, pid);
    }

    // 解除附加
    ptraceDetach(pid);
    // printf("[+] 已从目标进程分离\n");
    enableSELinux(); // 恢复SELinux
    return result;
}
