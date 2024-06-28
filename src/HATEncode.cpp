#include "HATEncode.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

HATEncoder::HATEncoder(const std::string& inputFilePath, const std::string& outputFilePath, int sampleRate, int bitRate, int audioChannels, const std::unordered_map<std::string, std::string>& metadata)
    : inputFilePath(inputFilePath), outputFilePath(outputFilePath), sampleRate(sampleRate), bitRate(bitRate), audioChannels(audioChannels), metadata(metadata) {}

void HATEncoder::encode() {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file." << std::endl;
        return;
    }

    readWavFile(inputFile);

    HATHeader header;
    header.version = HAT_VERSION;
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
    if (metadata.find("artist") != metadata.end()) {
        trackInfo.artist = metadata.at("artist");
    }
    if (metadata.find("description") != metadata.end()) {
        trackInfo.description = metadata.at("description");
    }
    if (metadata.find("trackName") != metadata.end()) {
        trackInfo.trackName = metadata.at("trackName");
    }
    if (metadata.find("trackNumber") != metadata.end()) {
        trackInfo.trackNumber = std::stoi(metadata.at("trackNumber"));
    }

    std::vector<int16_t> compressedData = compressData(audioData, header.compressionRatio);
    
    writeHATFile(header, trackInfo, compressedData);

    inputFile.close();
}

void HATEncoder::readWavFile(std::ifstream& inputFile) {
    // Simplified WAV file reading
    inputFile.seekg(24);
    inputFile.read(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));
    inputFile.seekg(34);
    inputFile.read(reinterpret_cast<char*>(&bitRate), sizeof(bitRate));
    inputFile.seekg(22);
    inputFile.read(reinterpret_cast<char*>(&audioChannels), sizeof(audioChannels));

    inputFile.seekg(44); // Skip the header
    int16_t sample;
    while (inputFile.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        audioData.push_back(sample);
    }
}

std::vector<int16_t> HATEncoder::compressData(const std::vector<int16_t>& data, float compressionRatio) {
    std::vector<int16_t> compressedData;
    size_t maxPatternSize = static_cast<size_t>(10 * compressionRatio); // Example: max pattern size based on ratio

    size_t i = 0;
    while (i < data.size()) {
        size_t patternSize = 1;

        // Detect pattern size
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

        // Compress pattern
        compressedData.push_back(patternSize); // Length of the pattern
        compressedData.push_back(data[i]);     // Pattern value

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

    outputFile.write(header.version.c_str(), header.version.size());
    outputFile.write(reinterpret_cast<const char*>(&header.channels), sizeof(header.channels));
    outputFile.write(reinterpret_cast<const char*>(header.spatialData), sizeof(header.spatialData));
    outputFile.write(reinterpret_cast<const char*>(&header.compressionRatio), sizeof(header.compressionRatio));
    outputFile.write(reinterpret_cast<const char*>(&header.compressionMethod), sizeof(header.compressionMethod));
    outputFile.write(reinterpret_cast<const char*>(&header.tracks), sizeof(header.tracks));
    outputFile.write(reinterpret_cast<const char*>(&header.sampleRate), sizeof(header.sampleRate));
    outputFile.write(reinterpret_cast<const char*>(&header.bitRate), sizeof(header.bitRate));
    outputFile.write(reinterpret_cast<const char*>(&header.length), sizeof(header.length));

   
    outputFile.write(trackInfo.artist.c_str(), trackInfo.artist.size());
    outputFile.write(trackInfo.description.c_str(), trackInfo.description.size());
    outputFile.write(trackInfo.trackName.c_str(), trackInfo.trackName.size());
    outputFile.write(reinterpret_cast<const char*>(&trackInfo.trackNumber), sizeof(trackInfo.trackNumber));

    outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size() * sizeof(int16_t));

    outputFile.close();
}
