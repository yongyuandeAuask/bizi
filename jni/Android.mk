# 预编译库定义部分
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := driver
LOCAL_SRC_FILES := driver.a
include $(PREBUILT_STATIC_LIBRARY)


# imgui_static 预编译库定义
include $(CLEAR_VARS)
LOCAL_MODULE := imgui_static
LOCAL_SRC_FILES := src/CPUaffinity/libPikachu.a
include $(PREBUILT_STATIC_LIBRARY)

# curl_static 预编译库定义
include $(CLEAR_VARS)
LOCAL_MODULE := curl_static
LOCAL_SRC_FILES := include/lib/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

# prebuilt_embree 预编译库定义
include $(CLEAR_VARS)
LOCAL_MODULE := prebuilt_embree
LOCAL_SRC_FILES := lib/libembree3.a
include $(PREBUILT_STATIC_LIBRARY)

# prebuilt_sys 预编译库定义
include $(CLEAR_VARS)
LOCAL_MODULE := prebuilt_sys
LOCAL_SRC_FILES := lib/libsys.a
include $(PREBUILT_STATIC_LIBRARY)

# prebuilt_math 预编译库定义
include $(CLEAR_VARS)
LOCAL_MODULE := prebuilt_math
LOCAL_SRC_FILES := lib/libmath.a
include $(PREBUILT_STATIC_LIBRARY)

# prebuilt_tasking 预编译库定义
include $(CLEAR_VARS)
LOCAL_MODULE := prebuilt_tasking
LOCAL_SRC_FILES := lib/libtasking.a
include $(PREBUILT_STATIC_LIBRARY)

# prebuilt_simd 预编译库定义
include $(CLEAR_VARS)
LOCAL_MODULE := prebuilt_simd
LOCAL_SRC_FILES := lib/libsimd.a
include $(PREBUILT_STATIC_LIBRARY)

# prebuilt_lexers 预编译库定义
include $(CLEAR_VARS)
LOCAL_MODULE := prebuilt_lexers
LOCAL_SRC_FILES := lib/liblexers.a
include $(PREBUILT_STATIC_LIBRARY)

# 主模块构建部分
include $(CLEAR_VARS)

LOCAL_MODULE := 币子.sh
LOCAL_CFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions -std=c++17
LOCAL_CPPFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++17
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions
#LOCAL_CPPFLAGS += -mllvm -irobf -mllvm -irobf-indbr -mllvm -irobf-icall -mllvm -irobf-indgv -mllvm -irobf-cff -mllvm -irobf-cie -mllvm -irobf-cfe -mllvm --level-indbr=3 -mllvm --level-icall=3 -mllvm --level-indgv=3 -mllvm --level-cie=3 -mllvm --level-cfe=3
# LOCAL_CPPFLAGS += -mllvm -irobf -mllvm -irobf-indbr -mllvm -level-indbr=3 -mllvm -irobf-icall -mllvm -level-icall=3 -mllvm -irobf-indgv -mllvm -level-indgv=3 -mllvm -irobf-cse

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/My_Utils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/curl
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/drivers
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
#模型
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/embree3
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/foundation

FILE_LIST += $(wildcard $(LOCAL_PATH)/src/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/drivers/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ImGui/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/include/T3_authentication/*.c*)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

# 更新静态库列表，添加了第一个文件中的所有预编译静态库
LOCAL_STATIC_LIBRARIES := driver  imgui_static curl_static \
                          prebuilt_embree prebuilt_sys prebuilt_math \
                          prebuilt_tasking prebuilt_simd prebuilt_lexers
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM -lGLESv2 -lGLESv3 -lOpenSLES
LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/*.a

include $(BUILD_EXECUTABLE)
