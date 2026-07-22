#ifndef GRYO_H
#define GRYO_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>  // 添加这行
#include <errno.h>  // 添加这行

#include "module.h"
#include "../sysread.h"
// #include "../Ptrace/PtraceUtils.h"
// #include "src/Utils/PtraceUtils.h"
// #include "src/Utils/Utils.h"

struct gyro_infor {
	bool enable;
	float x;
	float y;
};

class FGyro {
private:
	sysread sys;
	gyro_infor gyro;
	uint64_t gryoAddress = NULL;
	static inline FGyro *instance = nullptr;
	
	uint64_t find_maps_vm(pid_t pid, const char *vm_name) {
		char maps_path[64];
		snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
		FILE *fp = fopen(maps_path, "r");
		if (!fp) {
			return 0;
		}
		char line[512];
		uint64_t start_addr = 0;
		int found = 0;
		while (fgets(line, sizeof(line), fp)) {
			if (strstr(line, vm_name)) {
				if (sscanf(line, "%lx-", &start_addr) == 1) {
					found = 1;
					break;
				}
			}
		}
		fclose(fp);
		return found ? start_addr : 0;
	}
	
	bool write_and_inject() {
		const char *file_path = "/data/app/libaa.so";
		unsigned int gryo_length = sizeof(gryo_module);
		int fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
		if (fd < 0) {
			return false;
		}
		ssize_t written = write(fd, gryo_module, gryo_length);
		close(fd);
		// ========== 修复这里 ==========
		if (written != (ssize_t)gryo_length) {  // 修改这行
			return false;
		}
		// ==============================
		chmod(file_path, 0777);
		int result = PtraceInject("system_server", "/data/app/libaa.so");
		if (unlink(file_path) == 0) {
		} else {
			return false;
		}
		return true;
	}
	
	uintptr_t getPid(const char* packageName) {
		pid_t pid = -1;
    	FILE* file = nullptr;
    	char command[0x100] = "pidof ";
    	strcat(command, packageName);
    	file = popen(command,"r");
    	fscanf(file,"%d", &pid);
    	pclose(file);
    	return pid;
	}
	
public:
	bool Init() {
	Gryo_Start:
		pid_t pid = getPid("system_server");
		sys.SetPid(pid);
		if (pid == 0 || pid < 0) {
			return false;
		}
		gryoAddress = find_maps_vm(pid, "FuckMem");
		if (gryoAddress) {
			Unlock_Event();
			gyro = sys.read<gyro_infor>(gryoAddress);
			return true;
		}
		if (!gryoAddress) {
			// ========== 添加注入后等待 ==========
			if (write_and_inject()) {
				usleep(500000);  // 等待500ms让注入生效
				goto Gryo_Start;
			}
			// ==================================
		}
		return false;
	}
	
	void Send_Event(float x, float y) {
		if (gryoAddress) {
			gyro = {true, x, y};
			sys.writeMemory(gryoAddress, &gyro, sizeof(gyro_infor));
		}
	}
	
	void Unlock_Event() {
		if (gryoAddress) {
			gyro = {false, 0, 0};
			sys.writeMemory(gryoAddress, &gyro, sizeof(gyro_infor));
		}
	}
	
	gyro_infor GetGyroInfo() const {
		return gyro;
	}
	
	static FGyro *GetInstance() {
		if (!instance) {
			instance = new FGyro();
		}
		return instance;
	}
};

//static FGyro Gyro;
void GTB(float arg1, float arg2, float *out_result);
void GTA(uint64_t arg1, uint64_t arg2, float *out_result);

#endif