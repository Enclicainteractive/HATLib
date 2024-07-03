#include "HATEncode.h"
#include "dr_wav.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include "HATFormat.h"

HATEncoder::HATEncoder(const std::string& inputFilePath, const std::string& outputFilePath, int sampleRate, int bitRate, int audioChannels, const std::unordered_map<std::string, std::string>& metadata)
    : inputFilePath(inputFilePath), outputFilePath(outputFilePath), sampleRate(sampleRate), bitRate(bitRate), audioChannels(audioChannels), metadata(metadata) {}

void HATEncoder::encode() {
    readWavFile();

    HATHeader header;
    header.version = HAT_VERSION;
    header.channels = audioChannels;
    header.spatialData[0] = 0.0f;
    header.spatialData[1] = 0.0f;
    header.spatialData[2] = 0.0f;
    header.compressionMethod = LOSSLESS;
    header.tracks = 1;
    header.sampleRate = sampleRate;
    header.bitRate = bitRate;
    header.length = static_cast<int>(audioData.size());

    float compressionRatio = 0.0f;
    std::vector<uint8_t> compressedData = compressData(audioData, compressionRatio);
    header.compressionRatio = compressionRatio;
    header.datalength = static_cast<int>(compressedData.size());

    TrackInfo trackInfo;
    trackInfo.artist = metadata.count("artist") ? metadata.at("artist") : "Unknown Artist";
    trackInfo.description = metadata.count("description") ? metadata.at("description") : "No Description";
    trackInfo.trackName = metadata.count("trackName") ? metadata.at("trackName") : "Unknown Track";
    trackInfo.trackNumber = 1;
    trackInfo.seekMarker = 0;

    writeHATFile(header, trackInfo, compressedData);
}

void HATEncoder::readWavFile() {
    drwav wav;
    if (!drwav_init_file(&wav, inputFilePath.c_str(), NULL)) {
        std::cerr << "Error: Failed to open WAV file." << std::endl;
        return;
    }

    sampleRate = wav.sampleRate;
    bitRate = wav.bitsPerSample * wav.channels * wav.sampleRate;
    audioChannels = wav.channels;

    audioData.resize(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, audioData.data());

    drwav_uninit(&wav);
}

uint16_t calculateChecksum(const std::vector<uint8_t>& data) {
    uint16_t checksum = 0;
    for (auto byte : data) {
        checksum += byte;
    }
    return checksum;
}



std::vector<uint8_t> compressData(const std::vector<int16_t>& data, float& compressionRatio) {
    std::vector<uint8_t> compressedData;
    size_t dataSize = data.size();
    size_t i = 0;

    while (i < dataSize) {
        int16_t value = data[i];
        size_t runLength = 1;
        
        while (i + runLength < dataSize && data[i + runLength] == value && runLength < 255) {
            runLength++;
        }
        
        compressedData.push_back(static_cast<uint8_t>(runLength));
        compressedData.push_back(static_cast<uint8_t>(value & 0xFF));
        compressedData.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));

        i += runLength;
    }

    // Multi-stage compression
    std::vector<uint8_t> tempData = compressedData;
    for (int stage = 0; stage < 3; ++stage) { // Apply 3 stages of compression
        std::vector<uint8_t> stageCompressedData;
        size_t tempSize = tempData.size();
        size_t j = 0;

        while (j < tempSize) {
            uint8_t value = tempData[j];
            size_t runLength = 1;
            
            while (j + runLength < tempSize && tempData[j + runLength] == value && runLength < 255) {
                runLength++;
            }
            
            stageCompressedData.push_back(static_cast<uint8_t>(runLength));
            stageCompressedData.push_back(value);

            j += runLength;
        }

        tempData = stageCompressedData;
    }

    compressedData = tempData;

   
    uint16_t checksum = calculateChecksum(compressedData);
    compressedData.push_back(static_cast<uint8_t>(checksum & 0xFF));
    compressedData.push_back(static_cast<uint8_t>((checksum >> 8) & 0xFF));

    compressionRatio = static_cast<float>(dataSize * sizeof(int16_t)) / static_cast<float>(compressedData.size());
    std::cout << "Original size: " << dataSize * sizeof(int16_t) << ", Compressed size: " << compressedData.size() << ", Compression ratio: " << compressionRatio << std::endl;
    return compressedData;
}

void HATEncoder::writeHATFile(const HATHeader& header, const TrackInfo& trackInfo, const std::vector<uint8_t>& compressedData) {
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file." << std::endl;
        return;
    }

    outputFile.write(header.version.c_str(), 4);
    outputFile.write(reinterpret_cast<const char*>(&header.channels), sizeof(header.channels));
    outputFile.write(reinterpret_cast<const char*>(header.spatialData), sizeof(header.spatialData));
    outputFile.write(reinterpret_cast<const char*>(&header.compressionRatio), sizeof(header.compressionRatio));
    outputFile.write(reinterpret_cast<const char*>(&header.compressionMethod), sizeof(header.compressionMethod));
    outputFile.write(reinterpret_cast<const char*>(&header.tracks), sizeof(header.tracks));
    outputFile.write(reinterpret_cast<const char*>(&header.sampleRate), sizeof(header.sampleRate));
    outputFile.write(reinterpret_cast<const char*>(&header.bitRate), sizeof(header.bitRate));
    outputFile.write(reinterpret_cast<const char*>(&header.length), sizeof(header.length));
    outputFile.write(reinterpret_cast<const char*>(&header.datalength), sizeof(header.datalength));

    outputFile.write(trackInfo.artist.c_str(), 256);
    outputFile.write(trackInfo.description.c_str(), 256);
    outputFile.write(trackInfo.trackName.c_str(), 256);
    outputFile.write(reinterpret_cast<const char*>(&trackInfo.trackNumber), sizeof(trackInfo.trackNumber));
    outputFile.write(reinterpret_cast<const char*>(&trackInfo.seekMarker), sizeof(trackInfo.seekMarker));

    outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());

    outputFile.close();
}
