#ifndef BEAUTIFY_H
#define BEAUTIFY_H
#include <android/log.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "美化.h"
int 美化() {
    const char* 临时文件路径 = "/data/local/tmp/币子手持美化.sh";
    if (access(临时文件路径, F_OK) == 0) {
        if (unlink(临时文件路径) != 0) {
            perror("删除旧文件失败");
        }
    }
    int fd = open(临时文件路径, O_WRONLY | O_CREAT | O_TRUNC, 0700);
    if (fd == -1) {
        perror("创建临时文件失败");
        return -1;
    }
    size_t 总长度 = sizeof(币子手持美化);
    const size_t 块大小 = 4096;  
    for (size_t 偏移 = 0; 偏移 < 总长度; 偏移 += 块大小) {
        size_t 剩余 = 总长度 - 偏移;
        size_t 本次写入大小 = (剩余 > 块大小) ? 块大小 : 剩余;
        ssize_t 结果 = write(fd, 币子手持美化 + 偏移, 本次写入大小);
        if (结果 <= 0) {
            perror("写入文件失败");
            close(fd);
            return -1;
        }
    }
    close(fd);
    string cmd = string(临时文件路径) + " &";
    int 状态 = system(cmd.c_str());
    return 0;
}
#endif 
