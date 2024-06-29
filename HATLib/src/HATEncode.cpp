#include "HATEncode.h"
#include "dr_wav.h"
#include <fstream>
#include <iostream>
#include <zlib.h>
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
    header.length = audioData.size();

    TrackInfo trackInfo;
    trackInfo.artist = metadata.at("artist");
    trackInfo.description = "";  // Assuming description is empty for simplicity
    trackInfo.trackName = "";  // Assuming track name is empty for simplicity
    trackInfo.trackNumber = 1;  // Assuming single track

    std::vector<uint8_t> compressedData = compressData(audioData, header.compressionRatio);
    header.compressionRatio = static_cast<float>(audioData.size() * sizeof(int16_t)) / static_cast<float>(compressedData.size());

    writeHATFile(header, trackInfo, compressedData);
}

void HATEncoder::readWavFile() {
    drwav wav;
    if (!drwav_init_file(&wav, inputFilePath.c_str(), NULL)) {
        std::cerr << "Error: Failed to open WAV file. Possible reasons:" << std::endl;
        std::cerr << "1. The file is not a valid WAV file." << std::endl;
        std::cerr << "2. The file is corrupted." << std::endl;
        std::cerr << "3. The file is not readable due to permissions." << std::endl;
        return;
    }

    sampleRate = wav.sampleRate;
    bitRate = wav.bitsPerSample * wav.channels * wav.sampleRate;
    audioChannels = wav.channels;

    // Read PCM frames
    audioData.resize(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, audioData.data());

    drwav_uninit(&wav);
}

std::vector<uint8_t> HATEncoder::compressData(const std::vector<int16_t>& data, float& compressionRatio) {
    uLongf compressedSize = compressBound(data.size() * sizeof(int16_t));
    std::vector<uint8_t> compressedData(compressedSize);

    if (compress(compressedData.data(), &compressedSize, reinterpret_cast<const Bytef*>(data.data()), data.size() * sizeof(int16_t)) != Z_OK) {
        std::cerr << "Compression failed!" << std::endl;
        return {};
    }

    compressedData.resize(compressedSize);
    compressionRatio = static_cast<float>(data.size() * sizeof(int16_t)) / static_cast<float>(compressedSize);
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

    outputFile.write(trackInfo.artist.c_str(), 256);  
    outputFile.write(trackInfo.description.c_str(), 256);  
    outputFile.write(trackInfo.trackName.c_str(), 256);  
    outputFile.write(reinterpret_cast<const char*>(&trackInfo.trackNumber), sizeof(trackInfo.trackNumber));

    outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());

    outputFile.write("HATEOF", 6);

    outputFile.close();
}
