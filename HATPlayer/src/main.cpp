#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "HATDecode.h"
#include <iostream>
#include <vector>

// Callback function for audio playback
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    auto* audioData = reinterpret_cast<std::vector<int16_t>*>(pDevice->pUserData);
    static size_t currentSample = 0;
    size_t sampleCount = frameCount * pDevice->playback.channels;

    for (size_t i = 0; i < sampleCount; ++i) {
        if (currentSample < audioData->size()) {
            reinterpret_cast<int16_t*>(pOutput)[i] = (*audioData)[currentSample++];
        } else {
            reinterpret_cast<int16_t*>(pOutput)[i] = 0;
        }
    }

    (void)pInput;
}

void printHeaderInfo(const HATDecoder& decoder) {
    std::cout << "HAT File Header Information:" << std::endl;
    std::cout << "Version: " << decoder.getVersion() << std::endl;
    std::cout << "Channels: " << decoder.getChannels() << std::endl;
    const float* spatialData = decoder.getSpatialData();
    std::cout << "Spatial Data: (" << spatialData[0] << ", " << spatialData[1] << ", " << spatialData[2] << ")" << std::endl;
    std::cout << "Compression Ratio: " << decoder.getCompressionRatio() << std::endl;
    std::cout << "Tracks: " << decoder.getTracks() << std::endl;
    std::cout << "Sample Rate: " << decoder.getSampleRate() << std::endl;
    std::cout << "Bit Rate: " << decoder.getBitRate() << std::endl;
    std::cout << "Length: " << decoder.getAudioData().size() << std::endl;
}

void printTrackInfo(const HATDecoder& decoder) {
    std::cout << "Track Information:" << std::endl;
    std::cout << "Artist: " << decoder.getArtist() << std::endl;
    std::cout << "Description: " << decoder.getDescription() << std::endl;
    std::cout << "Track Name: " << decoder.getTrackName() << std::endl;
    std::cout << "Track Number: " << decoder.getTrackNumber() << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: HATPlayer <input HAT file>" << std::endl;
        return 1;
    }

    std::string inputFilePath = argv[1];

    HATDecoder decoder(inputFilePath);
    decoder.decode();

    // Display header info
    printHeaderInfo(decoder);

    // Display track info
    printTrackInfo(decoder);

    // Display audio data
    const auto& audioData = decoder.getAudioData();
    std::cout << "Audio Data (first 100 samples):" << std::endl;
    for (size_t i = 0; i < 100 && i < audioData.size(); ++i) {
        std::cout << audioData[i] << " ";
    }
    std::cout << std::endl;

    // Initialize miniaudio
    ma_device_config deviceConfig;
    ma_device device;
    ma_result result;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_s16;
    deviceConfig.playback.channels = decoder.getChannels();
    deviceConfig.sampleRate = decoder.getSampleRate(); // Adjust based on your audio data

    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = const_cast<std::vector<int16_t>*>(&audioData);

    result = ma_device_init(NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize playback device." << std::endl;
        return -1;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to start playback device." << std::endl;
        ma_device_uninit(&device);
        return -1;
    }

    std::cout << "Playing audio..." << std::endl;
    std::cin.get(); // Wait for user input to stop playback

    ma_device_uninit(&device);
    return 0;
}
