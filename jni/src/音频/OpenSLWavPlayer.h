/* 
 * by币子
 
 
 */

#ifndef OPENSL_WAV_PLAYER_H
#define OPENSL_WAV_PLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <cstdint>
#include <atomic>
#include <thread>

#include "娃.h"

class OpenSLWavPlayer {
public:
    /**
     * 播放内存中的WAV音频（非阻塞）
     * @param wav_data xxd生成的C数组音频数据
     * @param wav_length 音频数据长度
     * 调用方式：OpenSLWavPlayer::play(my_audio, my_audio_len);
     *
     * 该函数通过创建一个新的OpenSLWavPlayer实例来异步播放音频数据。
     * 使用new关键字创建实例后，实例会在播放完成后自行销毁。
     */
    static void play(const uint8_t* wav_data, size_t wav_length) {
        // 创建一个新的OpenSLWavPlayer实例，并启动播放线程
        new OpenSLWavPlayer(wav_data, wav_length);
    }

private:
    const uint8_t* mWavData;      // WAV音频数据指针
    size_t mWavDataLen;           // WAV音频数据长度

    // OpenSL ES相关对象
    SLObjectItf mEngineObj = nullptr;    // OpenSL引擎对象
    SLEngineItf mEngine;                 // OpenSL引擎接口
    SLObjectItf mOutputMixObj = nullptr; // 输出混音对象
    SLObjectItf mPlayerObj = nullptr;    // 播放器对象
    SLPlayItf mPlayer;                   // 播放器接口
    SLAndroidSimpleBufferQueueItf mBufferQueue; // 缓冲区队列接口

    // 播放状态相关变量
    const uint8_t* mPcmData;    // PCM格式的音频数据指针
    size_t mPcmLength;          // PCM音频数据长度
    std::atomic<bool> mPlaying{false}; // 播放状态标志，原子操作保证线程安全
    std::thread mWorkerThread;  // 工作线程，用于处理音频播放

    /**
     * 构造函数
     * @param data WAV音频数据指针
     * @param length WAV音频数据长度
     *
     * 构造函数初始化音频数据，并启动一个工作线程来处理音频播放。
     * 工作线程负责初始化OpenSL ES环境，开始播放，并进入播放循环。
     * 完成后通过delete this自行销毁实例。
     */
    OpenSLWavPlayer(const uint8_t* data, size_t length) 
        : mWavData(data), mWavDataLen(length) {
        // 启动工作线程
        mWorkerThread = std::thread([this](){
            if(initialize()) { // 初始化OpenSL ES环境
                startPlayback(); // 开始播放
                playbackLoop();  // 进入播放循环
            }
            delete this; // 播放完成后自行销毁实例
        });
        mWorkerThread.detach(); // 分离线程，使其在后台运行
    }

    /**
     * 析构函数
     *
     * 释放所有OpenSL ES资源，包括引擎、输出混音和播放器对象。
     */
    ~OpenSLWavPlayer() {
        releaseResources();
    }

    /**
     * 初始化OpenSL ES环境
     * @return 初始化是否成功
     *
     * 该函数创建并配置OpenSL ES引擎、输出混音和音频播放器。
     * 同时配置音频格式、注册缓冲区队列回调，并准备音频播放。
     */
    bool initialize() {
        if(!parseWavHeader()) return false; // 解析WAV头信息

        // 创建OpenSL ES引擎
        if(slCreateEngine(&mEngineObj, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS) 
            return false;
        if((*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) 
            return false;
        if((*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngine) != SL_RESULT_SUCCESS) 
            return false;

        // 创建输出混音
        if((*mEngine)->CreateOutputMix(mEngine, &mOutputMixObj, 0, nullptr, nullptr) != SL_RESULT_SUCCESS) 
            return false;
        if((*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) 
            return false;

        // 配置音频源
        SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
        
        SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM,
            mNumChannels,
            static_cast<SLuint32>(mSampleRate * 1000), // 转换为milliHz
            static_cast<SLuint32>(mBitsPerSample),
            static_cast<SLuint32>(mBitsPerSample),
            mNumChannels == 2 ? 
                (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT) : SL_SPEAKER_FRONT_CENTER,
            SL_BYTEORDER_LITTLEENDIAN};

        SLDataSource audioSrc = {&loc_bufq, &format_pcm};

        // 配置音频接收器
        SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObj};
        SLDataSink audioSnk = {&loc_outmix, nullptr};

        // 创建播放器
        const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY};
        const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

        if((*mEngine)->CreateAudioPlayer(mEngine, &mPlayerObj, &audioSrc, &audioSnk,
            2, ids, req) != SL_RESULT_SUCCESS) return false;
        if((*mPlayerObj)->Realize(mPlayerObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) 
            return false;
        if((*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_PLAY, &mPlayer) != SL_RESULT_SUCCESS) 
            return false;
        if((*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, 
            &mBufferQueue) != SL_RESULT_SUCCESS) return false;

        // 注册缓冲区队列回调
        if((*mBufferQueue)->RegisterCallback(mBufferQueue, bufferCallback, this) != SL_RESULT_SUCCESS)
            return false;

        return true;
    }

    /**
     * 开始播放音频
     *
     * 设置播放状态为播放，并填充初始缓冲区。
     */
    void startPlayback() {
        mPlaying.store(true); // 设置播放状态为true
        (*mPlayer)->SetPlayState(mPlayer, SL_PLAYSTATE_PLAYING); // 设置播放器状态为播放
        enqueueData(); // 填充初始缓冲区
    }

    /**
     * 填充缓冲区队列
     *
     * 从PCM数据中读取数据并填充到缓冲区队列中。
     * 如果剩余数据不足一个缓冲区大小，则设置播放状态为false，停止播放。
     */
    void enqueueData() {
        static constexpr size_t BUFFER_SIZE = 4096; // 定义缓冲区大小为4096字节
        size_t remain = mPcmLength - (mCurrentPos - mPcmData); // 计算剩余数据量
        
        if(remain > 0) { // 如果还有剩余数据
            size_t chunk = std::min(remain, BUFFER_SIZE); // 计算本次填充的数据量
            (*mBufferQueue)->Enqueue(mBufferQueue, mCurrentPos, chunk); // 填充数据到缓冲区
            mCurrentPos += chunk; // 更新当前指针位置
        } else { // 如果没有剩余数据
            mPlaying.store(false); // 设置播放状态为false
        }
    }

    /**
     * 缓冲区队列回调函数
     * @param bq 缓冲区队列接口
     * @param ctx 上下文指针，指向当前OpenSLWavPlayer实例
     *
     * 当缓冲区队列需要更多数据时调用该回调函数。
     * 如果当前处于播放状态，则继续填充数据。
     */
    static void bufferCallback(SLAndroidSimpleBufferQueueItf bq, void* ctx) {
        OpenSLWavPlayer* self = static_cast<OpenSLWavPlayer*>(ctx); // 获取当前实例
        if(self->mPlaying) { // 如果处于播放状态
            self->enqueueData(); // 继续填充数据
        }
    }

    /**
     * 播放循环
     *
     * 持续检查播放状态，直到播放结束。
     * 播放结束后，线程结束，实例自行销毁。
     */
    void playbackLoop() {
        while(mPlaying) { // 持续检查播放状态
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 休眠100毫秒
        }
        // 播放结束自动析构
    }

    /**
     * 释放OpenSL ES资源
     *
     * 销毁所有OpenSL ES对象，包括播放器、输出混音和引擎。
     */
    void releaseResources() {
        if(mPlayerObj) { // 如果播放器对象存在
            (*mPlayerObj)->Destroy(mPlayerObj); // 销毁播放器对象
            mPlayerObj = nullptr; // 设置指针为nullptr
        }
        if(mOutputMixObj) { // 如果输出混音对象存在
            (*mOutputMixObj)->Destroy(mOutputMixObj); // 销毁输出混音对象
            mOutputMixObj = nullptr; // 设置指针为nullptr
        }
        if(mEngineObj) { // 如果引擎对象存在
            (*mEngineObj)->Destroy(mEngineObj); // 销毁引擎对象
            mEngineObj = nullptr; // 设置指针为nullptr
        }
    }

    // WAV解析参数
    uint32_t mSampleRate = 0;       // 采样率
    uint16_t mNumChannels = 0;      // 声道数
    uint16_t mBitsPerSample = 0;    // 每个采样点的位数
    const uint8_t* mCurrentPos = nullptr; // 当前处理的数据位置

    /**
     * 解析WAV文件头
     * @return 解析是否成功
     *
     * 该函数解析WAV文件的RIFF头，验证文件格式，并提取音频参数。
     * 包括采样率、声道数和每个采样点的位数。
     */
    bool parseWavHeader() {
        if(mWavDataLen < 44) return false; // 检查数据长度是否足够

        // RIFF头校验
        if(memcmp(mWavData, "RIFF", 4) != 0) return false; // 检查RIFF标识
        if(memcmp(mWavData + 8, "WAVE", 4) != 0) return false; // 检查WAVE标识

        size_t offset = 12; // 偏移量初始化为12
        bool fmtFound = false, dataFound = false; // 标志位初始化

        while(offset + 8 <= mWavDataLen) { // 循环处理每个块
            const uint8_t* chunk = mWavData + offset; // 当前块指针
            uint32_t chunkSize = readLE<uint32_t>(chunk + 4); // 当前块大小
            
            if(memcmp(chunk, "fmt ", 4) == 0) { // 如果是fmt块
                if(chunkSize < 16) return false; // 检查块大小
                if(readLE<uint16_t>(chunk + 8) != 1) return false; // 检查音频格式是否为PCM

                mNumChannels = readLE<uint16_t>(chunk + 10); // 提取声道数
                mSampleRate = readLE<uint32_t>(chunk + 12); // 提取采样率
                mBitsPerSample = readLE<uint16_t>(chunk + 22); // 提取每个采样点的位数
                fmtFound = true; // 设置fmt标志位
            } else if(memcmp(chunk, "data", 4) == 0) { // 如果是data块
                mPcmData = chunk + 8; // 提取PCM数据指针
                mPcmLength = chunkSize; // 提取PCM数据长度
                mCurrentPos = mPcmData; // 设置当前处理位置
                dataFound = true; // 设置data标志位
                break; // 结束循环
            }
            
            offset += 8 + chunkSize; // 更新偏移量
            if(offset > mWavDataLen) break; // 检查偏移量是否超出范围
        }

        return fmtFound && dataFound && mSampleRate > 0 && // 返回解析结果
               (mBitsPerSample == 8 || mBitsPerSample == 16) &&
               (mNumChannels == 1 || mNumChannels == 2);
    }

    /**
     * 小端读取模板函数
     * @tparam T 数据类型
     * @param data 数据指针
     * @return 读取的值
     *
     * 该函数以小端格式读取数据。
     * 适用于无符号整数类型。
     */
    template<typename T>
    static T readLE(const uint8_t* data) {
        static_assert(std::is_unsigned<T>::value, "T must be unsigned"); // 检查类型是否为无符号
        T value = 0;
        for(size_t i = 0; i < sizeof(T); ++i) { // 循环读取每个字节
            value |= static_cast<T>(data[i]) << (i * 8); // 按位或操作，位移到正确的位置
        }
        return value; // 返回读取的值
    }
};

#endif // OPENSL_WAV_PLAYER_H