// VoiceInputService.h
#pragma once
#include "miniaudio.h"
#include <thread>
#include <atomic>
#include <mutex>

class VoiceInputService {
public:
    VoiceInputService();
    ~VoiceInputService();

    // Start capturing audio input from the microphone
    void start();

    // Stop capturing audio input
    void stop();

    // Returns the current normalized volume in range [0.0, 1.0]
    float getVolume();

private:
    // Called by miniaudio to process input frames
    static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    // Internal volume calculation from audio samples
    void processAudio(const float* input, ma_uint32 frameCount);

    ma_device m_device;
    std::atomic<float> m_currentVolume;
    std::mutex m_mutex;
    bool m_running = false;
};
