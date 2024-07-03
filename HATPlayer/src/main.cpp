#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "HATDecode.h"
#include <iostream>
#include <vector>
#include <cmath>

struct UserData {
    std::vector<int16_t>* audioData;
    const HATDecoder* decoder;
};

// Function to calculate the volume based on the distance from the origin
float calculateVolume(float x, float y, float z) {
    float distance = std::sqrt(x * x + y * y + z * z);
    return 1.0f / (1.0f + distance); // Simple inverse distance attenuation
}

// Function to calculate the panning based on the x coordinate
float calculatePanning(float x) {
    return x / std::sqrt(x * x + 1); // Normalized panning value
}

// Callback function for audio playback
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    UserData* userData = reinterpret_cast<UserData*>(pDevice->pUserData);
    auto* audioData = userData->audioData;
    const HATDecoder* decoder = userData->decoder;
    static size_t currentSample = 0;
    size_t sampleCount = frameCount * pDevice->playback.channels;

    const float* spatialData = decoder->getSpatialData();
    float volume = calculateVolume(spatialData[0], spatialData[1], spatialData[2]);
    float panning = calculatePanning(spatialData[0]);

    for (size_t i = 0; i < sampleCount; i += 2) { // Assuming stereo output
        if (currentSample < audioData->size()) {
            int16_t sample = (*audioData)[currentSample++];
            float leftSample = sample * volume * (1.0f - panning);
            float rightSample = sample * volume * (1.0f + panning);

            reinterpret_cast<int16_t*>(pOutput)[i] = static_cast<int16_t>(leftSample);
            reinterpret_cast<int16_t*>(pOutput)[i + 1] = static_cast<int16_t>(rightSample);
        } else {
            reinterpret_cast<int16_t*>(pOutput)[i] = 0;
            reinterpret_cast<int16_t*>(pOutput)[i + 1] = 0;
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
    std::cout << "Data Length: " << decoder.getAudioData().size() * sizeof(int16_t) << std::endl; // Added Data Length
}

void printTrackInfo(const HATDecoder& decoder) {
    std::cout << "Track Information:" << std::endl;
    std::cout << "Artist: " << decoder.getArtist() << std::endl;
    std::cout << "Description: " << decoder.getDescription() << std::endl;
    std::cout << "Track Name: " << decoder.getTrackName() << std::endl;
    std::cout << "Track Number: " << decoder.getTrackNumber() << std::endl;
}
void printColorCodedSample(int16_t sample) {
    if (sample < -20000) {
        std::cout << "\033[31m" << sample << "\033[0m "; // Red for very low values
    } else if (sample < -10000) {
        std::cout << "\033[33m" << sample << "\033[0m "; // Yellow for low values
    } else if (sample < 0) {
        std::cout << "\033[36m" << sample << "\033[0m "; // Cyan for negative values
    } else if (sample == 0) {
        std::cout << "\033[32m" << sample << "\033[0m "; // Green for zero
    } else if (sample <= 10000) {
        std::cout << "\033[36m" << sample << "\033[0m "; // Cyan for positive values
    } else if (sample <= 20000) {
        std::cout << "\033[33m" << sample << "\033[0m "; // Yellow for high values
    } else {
        std::cout << "\033[31m" << sample << "\033[0m "; // Red for very high values
    }
}

void displayAudioData(const std::vector<int16_t>& audioData) {
    std::cout << "Audio Data (first 100 samples):" << std::endl;
    for (size_t i = 0; i < 100 && i < audioData.size(); ++i) {
        printColorCodedSample(audioData[i]);
    }
    std::cout << std::endl;
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
    displayAudioData(decoder.getAudioData());
    std::cout << std::endl;

    // Initialize miniaudio
    ma_device_config deviceConfig;
    ma_device device;
    ma_result result;

    UserData userData = {const_cast<std::vector<int16_t>*>(&decoder.getAudioData()), &decoder};

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_s16;
    deviceConfig.playback.channels = 2; // Using stereo for panning
    deviceConfig.sampleRate = decoder.getSampleRate() * 2; // Adjust based on your audio data

    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &userData;

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
