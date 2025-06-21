
#define NOMINMAX
#define MINIAUDIO_IMPLEMENTATION

#include "VoiceInputService.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <algorithm>


VoiceInputService::VoiceInputService() {
    m_currentVolume = 0.0f;
    std::memset(&m_device, 0, sizeof(m_device));
}

VoiceInputService::~VoiceInputService() {
    stop();
}

void VoiceInputService::start() {
    if (m_running) return;

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = 1;
    deviceConfig.sampleRate = 48000;
    deviceConfig.dataCallback = dataCallback;
    deviceConfig.pUserData = this;

    if (ma_device_init(nullptr, &deviceConfig, &m_device) != MA_SUCCESS) {
        std::cerr << "❌ Failed to initialize microphone device." << std::endl;
        return;
    }

    if (ma_device_start(&m_device) != MA_SUCCESS) {
        std::cerr << "❌ Failed to start microphone device." << std::endl;
        ma_device_uninit(&m_device);
        return;
    }

    m_running = true;
    std::cout << "🎙️ Microphone listening started." << std::endl;
}

void VoiceInputService::stop() {
    if (!m_running) return;

    ma_device_uninit(&m_device);
    m_running = false;
    std::cout << "🛑 Microphone listening stopped." << std::endl;
}

float VoiceInputService::getVolume() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentVolume;
}

void VoiceInputService::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    VoiceInputService* service = reinterpret_cast<VoiceInputService*>(pDevice->pUserData);
    const float* input = reinterpret_cast<const float*>(pInput);

    if (service && input) {
        service->processAudio(input, frameCount);
    }
}

void VoiceInputService::processAudio(const float* input, ma_uint32 frameCount) {
    float sumSquares = 0.0f;
    for (ma_uint32 i = 0; i < frameCount; ++i) {
        sumSquares += input[i] * input[i];
    }

    float rms = std::sqrt(sumSquares / frameCount);
    float clamped = std::min(1.0f, std::max(0.0f, rms * 5.0f)); // Normalize

    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentVolume = clamped;
}
