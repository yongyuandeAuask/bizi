
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <fstream>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <iostream>
#include <fstream>
#include<iostream>
#include<ctime>
using namespace std;

#include <Draw.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include "辅助类.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <limits.h>
#include <malloc.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "res/weiyan.h"//删除微验请删除这行代码
#include "res/cJSON.h"//删除微验请删除这行代码
#include "res/cJSON.c"//删除微验请删除这行代码
#include "res/Encrypt.h"//删除微验请删除这行代码
#include "图片调用.h"
#include <curl/curl.h>

/*币子防御*/
/*币子防御*/

string zmxx , ylj;

int abs_ScreenX, abs_ScreenY;

int 无后台, 自瞄选项, 漏打;

布局 布局;
绘制 绘制;

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

// 配置文件类
class 配置管理器 {
private:
    std::string 配置文件路径 = "/data/币子启动配置.txt";
    std::map<std::string, std::string> 配置数据;
    
public:
    // 保存配置到文件
    void 保存配置(int 无后台, int 自瞄模式, int 音量控制, int 漏打模式 = 0) {
        std::ofstream 文件(配置文件路径);
        if (!文件.is_open()) {
            printf("\033[1;33m[!] 无法保存配置，但程序可以继续运行\033[0m\n");
            return;
        }
        
        文件 << "无后台=" << 无后台 << "\n";
        文件 << "自瞄模式=" << 自瞄模式 << "\n";
        文件 << "音量控制=" << 音量控制 << "\n";
        文件 << "漏打模式=" << 漏打模式 << "\n";
        
        文件.close();
        printf("\033[1;32m[✓] 配置已自动保存\033[0m\n");
    }
    
    // 从文件读取配置
    bool 读取配置() {
        配置数据.clear();
        std::ifstream 文件(配置文件路径);
        
        if (!文件.is_open()) {
            return false; // 配置文件不存在
        }
        
        std::string 行;
        while (std::getline(文件, 行)) {
            size_t 等号位置 = 行.find('=');
            if (等号位置 != std::string::npos) {
                std::string 键 = 行.substr(0, 等号位置);
                std::string 值 = 行.substr(等号位置 + 1);
                配置数据[键] = 值;
            }
        }
        
        文件.close();
        return !配置数据.empty();
    }
    
    // 获取配置值
    int 获取整数配置(const std::string& 键, int 默认值 = 0) {
        auto 查找结果 = 配置数据.find(键);
        if (查找结果 != 配置数据.end()) {
            try {
                return std::stoi(查找结果->second);
            } catch (...) {
                return 默认值;
            }
        }
        return 默认值;
    }
    
    // 清空配置
    void 清空配置() {
        配置数据.clear();
        remove(配置文件路径.c_str());
    }
};

// 修改后的main函数部分
int main(int argc, char* argv[]) {

    int 防录屏 = 1;      // std::stoi(argv[1]);
    int 自瞄模式 = 0;    // std::stoi(argv[2]);
    int 无后台开关 = 0;  // std::stoi(argv[3]);
    int 漏打模式 = 0;    // std::stoi(argv[4]);
    int 无后台 = 0;

    配置管理器 配置;
    bool 使用上次配置 = false;
        system("clear");



       //删除微验请取消注释这行
//分段错误不是驱动问题就是你code判断填错了 一键对接那可以看见
    const static char *_wyHost = "wy.llua.cn";
	const static char *_wyAppid = "70291";	//修改这个 1/4
	const static char *_wyAppkey = "p9d34081d0509e13734";//修改这个 2/4
	const static char *_wyRc4key = "k2b670a35d419c6f938";//修改这个 3/4
	// 以上信息勿动
	
	const static char *_kmPath = "/sdcard/km";//卡密路径 APK内核识别的路径同一个
	// 卡密路径

	const static char *_imeiPath = "/sdcard/imei";
	// 机器码路径
	
	const static bool _ggSwitch = false;
	// 公告开关

	printf("\033[35;1m");		// 粉红色
	printf("欢迎使用币子内核 尊贵的VIP用户 免费卡密获取@bizinb\n",_wyAppid);
	printf("\033[32;1m");		// 绿色
	printf("\n币子内核为你保驾护航 @bizinb\n\n");
	printf("\033[33;1m");		// 黄色

	if (_ggSwitch){
	    char _ggUrl[1024];
	    sprintf(_ggUrl, "app=%s",_wyAppid);
    	char *_ggData = httppost(_wyHost,"api/?id=notice",_ggUrl);
    	char* _deggData=Decrypt(_ggData, _wyRc4key);
    	cJSON *_ggJson = cJSON_Parse(_deggData);
    	int _ggCode = cJSON_GetObjectItem(_ggJson, "code")->valueint;
    	if (_ggCode == 200){//不是这个code判断
    		cJSON *_ggMsg = cJSON_GetObjectItem(_ggJson, "msg");
            char *_appgg = cJSON_GetObjectItem(_ggMsg, "app_gg")->valuestring;
    	    printf("\n\n公告:%s\n\n",_appgg);
    	}
	}
	
	home_main:
	char _Kami[40];
	if (fopen(_kmPath, "r") == NULL)
	{
		printf("\033[31;1m");
		printf("[@bizinb]请输入卡密:");
        char _inputKm[] = "";
	    scanf("%s",&_inputKm);
        FILE *fp = fopen(_kmPath, "w");
        if (fp != NULL) {
            fprintf(fp, "%s", _inputKm);
		    fclose(fp);
        }
        std::cout << "读取成功！正在链接服务器" << std::endl;
	}
	fscanf(fopen(_kmPath, "r"), "%s", &_Kami);
	char _Imei[40];
	if (fopen(_imeiPath, "r") == NULL)
	{
		printf("\033[31;1m");
		printf("设备码获取失败\n");
		srand(time(NULL));
        char* _Str = (char*)malloc((20 + 1) * sizeof(char));
        for (int i = 0; i < 20; i++) {
            int _randomNum = rand() % 26;
            _Str[i] = 'a' + _randomNum;
        }
        _Str[20] = '\0';
    
        FILE *fp = fopen(_imeiPath, "w");
        if (fp == NULL) {
            printf("文件创建失败");
            return 0;
        }
        fprintf(fp, "%s", _Str);
        fclose(fp);
        std::cout << "设备码已重新获取！正在重新验证卡密" << std::endl;
	}
	fscanf(fopen(_imeiPath, "r"), "%s", &_Imei);
	printf("卡密： %s\n设备码： %s\n\n", _Kami, _Imei);
	if (_Kami == "" or _Imei == "")
	{
		printf("\033[31;1m");
		printf("无设备码或者卡密");
		return 0;
	}
	time_t _Timet = time(NULL);
	int _Time = time(&_Timet);
    srand(time(NULL));
	char _Value[1024];
	char _Sign[1024];
	char _Data[1024];
	sprintf(_Value, "%d%d", _Time,rand());
	sprintf(_Sign, "kami=%s&markcode=%s&t=%d&%s", _Kami, _Imei, _Time, _wyAppkey);
	unsigned char *_SignData = (unsigned char *)_Sign;
	MD5_CTX md5c;
	MD5Init(&md5c);
	unsigned char _Decrypt[16];
	MD5Update(&md5c, _SignData, strlen((char *)_SignData));
	MD5Final(&md5c, _Decrypt);
	char _SignMd5[33] = { 0 };
	for (int i = 0; i < 16; i++)
	{
		sprintf(&_SignMd5[i * 2], "%02x", _Decrypt[i]);
	}
	sprintf(_Data, "kami=%s&markcode=%s&t=%d&sign=%s&value=%s", _Kami, _Imei, _Time, _SignMd5, _Value);
    char *_enData=Encrypt(_Data, _wyRc4key);
	char _deData[1024];
	sprintf(_deData, "&data=%s", _enData);
	char _deUrl[1024];
	sprintf(_deUrl, "api/?id=kmlogin&app=%s",_wyAppid);
	char *_loginData = httppost(_wyHost,_deUrl,_deData);
	char* _deloginData=Decrypt(_loginData, _wyRc4key);
	cJSON *_loginJson = cJSON_Parse(_deloginData);
	int _loginCode = cJSON_GetObjectItem(_loginJson, "xb34091862aa9871328c6ca74875063c1")->valueint;
	int _loginTime = cJSON_GetObjectItem(_loginJson, "p645ee9de7bd65595884d4a38b53ddbeb")->valueint;
	char *_loginMsg = cJSON_GetObjectItem(_loginJson, "nb73d7ed95d1b6ba7223df7d8661fcfdb")->valuestring;
	char *_loginCheck = cJSON_GetObjectItem(_loginJson, "ea681092c8e0332fbd8a83e2ce6cf04dd")->valuestring;
	if (_loginCode == 72267)//code判断修改这个 4/4 不写等分段错误
	{
		cJSON *_loginMsgs = cJSON_GetObjectItem(_loginJson, "nb73d7ed95d1b6ba7223df7d8661fcfdb");
	    char *_checkCode = cJSON_GetObjectItem(_loginMsgs, "w43fad5f47dd4266441df9b659318a09e")->valuestring;
		long _loginVip = cJSON_GetObjectItem(_loginMsgs, "h3e272e62cf9e1b9013b8c849a0cdbd29")->valuedouble;
		long _loginId = cJSON_GetObjectItem(_loginMsgs, "p85ea6ca424b265392a4fcd30c527a817")->valuedouble;
		char _deCheck[1024];
		sprintf(_deCheck, "%d%s%s",_loginTime,_wyAppkey,_Value);
		unsigned char *_deCheckData = (unsigned char *)_deCheck;
		MD5_CTX md5c;
		MD5Init(&md5c);
		unsigned char _Decrypt[16];
		MD5Update(&md5c, _deCheckData, strlen((char *)_deCheckData));
		MD5Final(&md5c, _Decrypt);
		char _checkMd5[33] = { 0 };
		for (int i = 0; i < 16; i++)
		{
			sprintf(&_checkMd5[i * 2], "%02x", _Decrypt[i]);
		}
		if ((string)_checkCode != "1dbd09de85dc99445f1ad23d9377cacd"){
		    return 0;
		}
		if ((string)_checkMd5 == _loginCheck)
		{
			printf("\033[32;1m");	// 绿色
			printf("登录成功  币子为你保驾护航\n|||—>—>—>—>–>–>–>–>–>—>—>–>–>–>–>|||\n");
			if (_loginVip)
			{
				char _vipTime[11];
				sprintf(_vipTime, "%ld", _loginVip);
				time_t _timeStamp = std::atoll(_vipTime);
				std::tm * _timeInfo = std::localtime(&_timeStamp);
				char _buffer[80];
				std::strftime(_buffer, sizeof(_buffer), "%Y-%m-%d %H:%M:%S", _timeInfo);
				std::cout << "到期时间：\n|||—>—>—>—>–>–>–>–>–>—>—>–>–>–>–>|||\n" << _buffer << std::endl;
				//到期自动退出
				signal(SIGALRM, _exit); 
                alarm(_loginVip-_Time); 
			}
		}
		else
		{
			printf("校验失败\n");
			remove(_kmPath);
		    goto home_main;
		    return 0;
		}
	}
	else
	{
		printf("\033[35;1m");	// 粉红色
		cout << _loginMsg << endl;
		remove(_kmPath);
		goto home_main;
		return 0;
	}





//       */   //删除微验请取消注释这行


        system("clear");

        布局.初始化程序();
        加载内存图片();
        system("clear");
    printf("\033[3;4;90m温馨提示:📋井本软件/程序由大型人工智能模型ChatGTP开发,仅用于修复游戏漏洞进行参考,禁止任何破坏游戏行为\n🔒为了用户账号安全,请确保用户设备环境正常,拥有足够的演戏意识避免人工巡查\n🛠️本软件/程序如因任何不可抗因素停止运行或出现异常,我们会尽快修复以致力于修复游戏漏洞进行参考\n💳本程序/软件为用户自愿捐赠进行网络验证无任何违法行为\n✅继续使用本软件/程序默认同意以上条款\n✅请不要去除本辅助的频道号，这是本频道唯一更新的动力！币子频道:@bizinhnb\033[0m\n");

    // 询问是否使用上次配置
    if (配置.读取配置()) {
        printf("\033[1;33m[♥] 是否使用上次启动配置\n");
        printf("    [1] 是，使用上次配置\n");
        printf("    [2] 否，重新选择配置\n");
        printf("    [+] 请输入选择: \033[0m");
        
        std::string 选择;
        std::cin >> 选择;
        
        if (选择 == "1" || 选择 == "一" || 选择 == "y" || 选择 == "Y") {
            使用上次配置 = true;
            
            // 从配置读取值
            int 上次无后台 = 配置.获取整数配置("无后台", 1);
            int 上次自瞄模式 = 配置.获取整数配置("自瞄模式", 0);
            int 上次音量控制 = 配置.获取整数配置("音量控制", 0);
            
            // 应用配置
            无后台 = 上次无后台;
            自瞄模式 = 上次自瞄模式;
            zmxx = std::to_string(上次自瞄模式 + 1); // 转换为用户输入格式
            ylj = std::to_string(上次音量控制);
            
            printf("\n\033[1;32m[✓] 已应用上次配置：\n");
            printf("    无后台: %s\n", 无后台 == 1 ? "有后台" : "无后台");
            printf("    自瞄模式: ");
            if (自瞄模式 == 0) printf("触摸自瞄\n");
            else if (自瞄模式 == 1) printf("无模块陀螺仪自瞄\n");
            else printf("单透模式\n");
            printf("    音量控制: %s\033[0m\n\n", 上次音量控制 == 1 ? "开启" : "关闭");
        }
    }
    
    // 如果用户不使用上次配置，则进行正常选择流程
    if (!使用上次配置) {
        // 无后台选择
        printf("\033[31;1m");
        std::cout << std::endl << "1.有后台 2.无后台: ";
        std::cin >> 无后台;
        if (无后台 == 1) {
            std::cout << "\033[1;32m[+] 有后台模式已开启\033[0m\n";
        } else {
            pid_t pids = fork();
            if (pids > 0) {
                exit(0);
            }
            std::cout << "\033[1;32m[+] 无后台模式已开启\033[0m\n";
        }
        
        // 自瞄模式选择
        printf("\n🎯 请选择你要启动的自瞄模式\n");
        printf("[1] 🎯 触摸自瞄\n");
        printf("[2] 🎮 无模块陀螺仪自瞄\n");
        printf("[3] ♥ 单透模式\n");
        printf("[+]请输入序号: ");
        
        std::cin >> zmxx;
        if (zmxx == "一" || zmxx == "1") {
            绘制.自瞄模式 = 0;
        } else if (zmxx == "二" || zmxx == "2") {
            绘制.自瞄模式 = 1;
        } else if (zmxx == "三" || zmxx == "3") {
            绘制.自瞄模式 = 2;
        }
        
        // 音量控制选择
        printf("\033[35;1m");
        printf("\n🎛️ 请选择是否使用音量键控制悬浮窗\n");
        printf("📱 按 音量+ 键为打开悬浮窗\n");
        printf("📱 按 音量- 键为关闭悬浮窗\n\n");
        printf("⚠️ 音量键控制(可能会导致加快耗电和发热，不建议开)\n");
        printf("🔘 [1=开启, 0=不开]: ");
        std::cin >> ylj;
        
        // 自动保存配置
        配置.保存配置(无后台, 绘制.自瞄模式, (ylj == "y" || ylj == "1") ? 1 : 0);
    }
    
    // 执行自瞄初始化
    if (绘制.自瞄模式 == 0) {
        Touch_Init(
            displayInfo.width, 
            displayInfo.height, 
            displayInfo.orientation, 
            false
        );
        绘制.自瞄.预判力度 = 1.55f;
        绘制.自瞄主线程();
        绘制.GetTouch();
        绘制.按钮.自瞄选项 = true;
    } else if (绘制.自瞄模式 == 1) {
        printf("\033[32;1m[✅] 已启用 陀螺仪自瞄\033[0m\n");
        Touch_Init(
            displayInfo.width, 
            displayInfo.height, 
            displayInfo.orientation, 
            true
        );
        绘制.自瞄.预判力度 = 1.55f;
        绘制.陀螺仪自瞄主线程();
        绘制.按钮.自瞄选项 = true;        
    } else if (绘制.自瞄模式 == 2) {
        Touch_Init(
            displayInfo.width, 
            displayInfo.height, 
            displayInfo.orientation, 
            true
        );
    }
    
    // 音量控制
    if (ylj == "y" || ylj == "1") {
        new std::thread(音量);
    }
    
    // 继续执行其他初始化
    绘制.读取配置();
    布局.开启悬浮窗();
    
    return 0;
}


// int main(int argc, char* argv[]) {
    // system("clear");
    // if (create_kill_script()) {
    // printf("🔧 杀死内核脚本创建成功\n");
// } else {
    // printf("⚠️ 杀死内核脚本创建失败\n");
// }

    // int 防录屏 = 1;      // std::stoi(argv[1]);
    // int 自瞄模式 = 0;    // std::stoi(argv[2]);
    // int 无后台开关 = 0;  // std::stoi(argv[3]);
    // int 漏打模式 = 0;    // std::stoi(argv[4]);
    // int 无后台 = 0;
    
    
    // printf("\033[1;31m------------------------------------------------\n");
    

    // // 无后台选择
    // printf("\033[31;1m");
    // std::cout << std::endl << "1.有后台 2.无后台";
    // std::cin >> 无后台;
    // if (无后台 == 1) {
        // std::cout << "\033[1;32m[+] 有后台模式已开启\033[0m\n";
    // } else {
    
        // pid_t pids = fork();
        // if (pids > 0) {
            // exit(0);
        // }
        // std::cout << "\033[1;32m[+] 无后台模式已开启\033[0m\n";
 // //       new std::thread(音量);
    // }
// /*
// if (绘制.漏打模式 == 1) {
    // 绘制.漏打开关 = true;  // 1 表示开启
// } else if (绘制.漏打模式 == 2) {
    // 绘制.漏打开关 = false; // 2 表示关闭
// } else {
    // throw std::invalid_argument("漏吊开关");
// }*/


        // 布局.初始化程序();
        // 加载内存图片();
    // printf("\n🎯 请选择你要启动的自瞄模式\n");
    // printf("[1] 🎯 触摸自瞄\n");
    // printf("[2] 🎮 无模块陀螺仪自瞄\n");
    // printf("[3] ♥ 单透模式\n");
    // printf("[+]请输入序号:");

    // cin >> zmxx;
    // if (zmxx == "一" or zmxx == "1")
    // {
    // 绘制.自瞄模式 = 0;
    // }
    // else if (zmxx == "二" or zmxx == "2")
    // {
    // 绘制.自瞄模式 = 1;
    // }
    // else if (zmxx == "三" or zmxx == "3")
    // {
    // 绘制.自瞄模式 = 2;
    // }
    
        // if (绘制.自瞄模式 == 0)
        // {
             // Touch_Init(
         // displayInfo.width, 
         // displayInfo.height, 
         // displayInfo.orientation, 
         // false//写入触摸
     // );
        // 绘制.自瞄.预判力度 = 1.55f;
        // 绘制.自瞄主线程();
        // 绘制.GetTouch();
        // 绘制.按钮.自瞄选项 = true;
        // } else if (绘制.自瞄模式 == 1){

        // printf("\033[32;1m[✅] 已启用 陀螺仪自瞄\033[0m\n");
        // Touch_Init(
        // displayInfo.width, 
        // displayInfo.height, 
        // displayInfo.orientation, 
        // true//不写入触摸
    // );
    
        // 绘制.自瞄.预判力度 = 1.55f;
        // 绘制.陀螺仪自瞄主线程();
        // 绘制.按钮.自瞄选项 = true;        
        // // 绘制.自瞄.陀螺仪自瞄开启 = true;
        
        // } else if (绘制.自瞄模式 == 2){
        // Touch_Init(
         // displayInfo.width, 
         // displayInfo.height, 
         // displayInfo.orientation, 
         // true
     // );
// }
    // printf("\033[35;1m\n");
    // printf("\n🎛️ 请选择是否使用音量键控制悬浮窗\n");
    // printf("📱 按 音量+ 键为打开悬浮窗\n");
    // printf("📱 按 音量- 键为关闭悬浮窗\n\n");
    // printf("⚠️ 音量键控制(可能会导致加快耗电和发热，不建议开)\n");
    // printf("🔘 [1=开启, 0=不开]：");
    // cin >> ylj;
    // if (ylj == "y" or ylj == "1")
    // {
        // // extern int 音量();
        // // std::thread volume_thread(音量);
        // // volume_thread.detach();
         // new std::thread(音量);

    // }
    // else if (ylj == "n" or ylj == "0")
    // {
        // // 不做任何操作
    // }


        // 绘制.读取配置();
        // 布局.开启悬浮窗();
    // }
