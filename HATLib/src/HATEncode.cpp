#include "HATEncode.h"
#include "dr_wav.h"
#include <fstream>
#include <iostream>

HATEncoder::HATEncoder(const std::string& inputFilePath, const std::string& outputFilePath, int sampleRate, int bitRate, int audioChannels, const std::unordered_map<std::string, std::string>& metadata)
    : inputFilePath(inputFilePath), outputFilePath(outputFilePath), sampleRate(sampleRate), bitRate(bitRate), audioChannels(audioChannels), metadata(metadata) {}

void HATEncoder::encode() {
    readWavFile();

    HATHeader header;
    header.version = "1.0";
    header.channels = audioChannels;
    header.spatialData[0] = 0.0f;
    header.spatialData[1] = 0.0f;
    header.spatialData[2] = 0.0f;
    header.compressionRatio = 0.5f;
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

    std::vector<int16_t> compressedData = compressData(audioData, header.compressionRatio);

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

std::vector<int16_t> HATEncoder::compressData(const std::vector<int16_t>& data, float compressionRatio) {
    std::vector<int16_t> compressedData;
    size_t maxPatternSize = static_cast<size_t>(10 * compressionRatio);

    size_t i = 0;
    while (i < data.size()) {
        size_t patternSize = 1;

        while (i + patternSize < data.size() && patternSize < maxPatternSize) {
            bool isPattern = true;
            for (size_t j = 0; j < patternSize; ++j) {
                if (data[i + j] != data[i + patternSize + j]) {
                    isPattern = false;
                    break;
                }
            }
            if (!isPattern) {
                break;
            }
            ++patternSize;
        }

        compressedData.push_back(patternSize);
        compressedData.push_back(data[i]);

        i += patternSize;
    }

    return compressedData;
}

void HATEncoder::writeHATFile(const HATHeader& header, const TrackInfo& trackInfo, const std::vector<int16_t>& compressedData) {
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

    outputFile.write(trackInfo.artist.c_str(), 256);  // Assuming fixed length for simplicity
    outputFile.write(trackInfo.description.c_str(), 256);  // Assuming fixed length for simplicity
    outputFile.write(trackInfo.trackName.c_str(), 256);  // Assuming fixed length for simplicity
    outputFile.write(reinterpret_cast<const char*>(&trackInfo.trackNumber), sizeof(trackInfo.trackNumber));

    outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size() * sizeof(int16_t));

    outputFile.close();
}
