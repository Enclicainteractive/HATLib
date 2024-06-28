#include "HATDecode.h"
#include <fstream>
#include <iostream>

HATDecoder::HATDecoder(const std::string& inputFilePath)
    : inputFilePath(inputFilePath) {}

void HATDecoder::decode() {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file." << std::endl;
        return;
    }

    HATHeader header;
    readHATHeader(inputFile, header);

    TrackInfo trackInfo;
    readTrackInfo(inputFile, trackInfo);

    std::vector<int16_t> compressedData(header.length);
    inputFile.read(reinterpret_cast<char*>(compressedData.data()), header.length * sizeof(int16_t));
    
    audioData = decompressData(compressedData, header.compressionRatio);

    inputFile.close();
}

void HATDecoder::readHATHeader(std::ifstream& inputFile, HATHeader& header) {
    // Read the version string (assume fixed length for simplicity)
    char version[4];
    inputFile.read(version, 4);
    header.version = std::string(version, 4);

    inputFile.read(reinterpret_cast<char*>(&header.channels), sizeof(header.channels));
    inputFile.read(reinterpret_cast<char*>(header.spatialData), sizeof(header.spatialData));
    inputFile.read(reinterpret_cast<char*>(&header.compressionRatio), sizeof(header.compressionRatio));
    inputFile.read(reinterpret_cast<char*>(&header.compressionMethod), sizeof(header.compressionMethod));
    inputFile.read(reinterpret_cast<char*>(&header.tracks), sizeof(header.tracks));
    inputFile.read(reinterpret_cast<char*>(&header.sampleRate), sizeof(header.sampleRate));
    inputFile.read(reinterpret_cast<char*>(&header.bitRate), sizeof(header.bitRate));
    inputFile.read(reinterpret_cast<char*>(&header.length), sizeof(header.length));
}

void HATDecoder::readTrackInfo(std::ifstream& inputFile, TrackInfo& trackInfo) {
    // Assuming the metadata is stored as fixed length strings for simplicity
    char artist[256];
    inputFile.read(artist, sizeof(artist));
    trackInfo.artist = std::string(artist);

    char description[256];
    inputFile.read(description, sizeof(description));
    trackInfo.description = std::string(description);

    char trackName[256];
    inputFile.read(trackName, sizeof(trackName));
    trackInfo.trackName = std::string(trackName);

    inputFile.read(reinterpret_cast<char*>(&trackInfo.trackNumber), sizeof(trackInfo.trackNumber));
}

std::vector<int16_t> HATDecoder::decompressData(const std::vector<int16_t>& compressedData, float compressionRatio) {
    std::vector<int16_t> decompressedData;
    size_t i = 0;

    while (i < compressedData.size()) {
        int16_t patternLength = compressedData[i++];
        int16_t patternValue = compressedData[i++];

        for (int j = 0; j < patternLength; ++j) {
            decompressedData.push_back(patternValue);
        }
    }

    return decompressedData;
}
