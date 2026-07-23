# Android build notes

# How to build and run

1. Ensure ANDROID_NDK is set and ndk-build is available.

2. Build (arm64 example):

   $ ANDROID_NDK=/path/to/android-ndk
   $ $ANDROID_NDK/ndk-build NDK_APPLICATION_MK=Application.mk APP_ABI=arm64-v8a -j8

3. Find the produced executable under libs/arm64-v8a/start.sh or obj/local/arm64-v8a/start.sh and push to device:

   $ adb push libs/arm64-v8a/start.sh /data/local/tmp/
   $ adb shell "chmod 755 /data/local/tmp/start.sh"

4. Run on device (likely needs root to read another process memory):

   $ adb shell "su -c /data/local/tmp/start.sh"

5. Watch logs:

   $ adb logcat | grep physx_native_exe
