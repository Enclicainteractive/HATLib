#include "HATDecode.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include "HATFormat.h"

HATDecoder::HATDecoder(const std::string& inputFilePath)
    : inputFilePath(inputFilePath) {}

void HATDecoder::decode() {
    std::ifstream inputFile(inputFilePath, std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        std::cerr << "\033[31m Error opening input file." << std::endl << "\033[39m";
        return;
    }

    std::streamsize fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    readHATHeader(inputFile, header);
    readTrackInfo(inputFile, trackInfo);

    std::vector<uint8_t> compressedData(header.datalength);  // Use datalength to read compressed data
    inputFile.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());

    // Verify checksum before decompression
    uint16_t storedChecksum = static_cast<uint16_t>(compressedData[compressedData.size() - 2]) |
                              (static_cast<uint16_t>(compressedData[compressedData.size() - 1]) << 8);
    compressedData.resize(compressedData.size() - 2);

    uint16_t calculatedChecksum = std::accumulate(compressedData.begin(), compressedData.end(), 0u);
    if (storedChecksum != calculatedChecksum) {
        std::cerr << "\033[31m Checksum mismatch! Data may be corrupted." << std::endl << "\033[39m";
        return;
    }

    audioData = decompressData(compressedData, header.length);

    inputFile.close();

    if (header.length != audioData.size()) {
        std::cerr << "\033[31m Decompression failed! Size mismatch. Expected: " << header.length << ", Got: " << audioData.size() << std::endl << "\033[39m";
    } else {
        std::cout << "\033[32m Decompression successful. Decompressed data size matches the expected size.\033[39m" << std::endl;
    }

    std::cout << "File size: \033[32m" << fileSize << "\033[39m bytes, Expected total data size (including headers): \033[32m" << (header.datalength + sizeof(HATHeader) + sizeof(TrackInfo) + 4) << " \033[39mbytes" << std::endl;
    if (fileSize != (header.datalength + sizeof(HATHeader) + sizeof(TrackInfo) + 4)) {
        std::cerr << "\033[93m File size mismatch! The entire file size does not match the expected total data size." << std::endl << "\033[39m";
    }
}

void HATDecoder::readHATHeader(std::ifstream& inputFile, HATHeader& header) {
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
    inputFile.read(reinterpret_cast<char*>(&header.datalength), sizeof(header.datalength));
}

void HATDecoder::readTrackInfo(std::ifstream& inputFile, TrackInfo& trackInfo) {
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
    inputFile.read(reinterpret_cast<char*>(&trackInfo.seekMarker), sizeof(trackInfo.seekMarker));
}

std::vector<int16_t> HATDecoder::decompressData(const std::vector<uint8_t>& compressedData, size_t dataSize) {
    // Remove checksum before decompression
    std::vector<uint8_t> dataWithoutChecksum(compressedData.begin(), compressedData.end() - 2);

    std::vector<uint8_t> stageData = dataWithoutChecksum;
    for (int stage = 0; stage < 3; ++stage) { // Apply 3 stages of decompression
        stageData = decompressStage(stageData);
    }

    std::vector<int16_t> decompressedData;
    decompressedData.reserve(dataSize);
    size_t i = 0;

    while (i < stageData.size()) {
        uint8_t runLength = stageData[i];
        int16_t value = static_cast<int16_t>(stageData[i + 1]) | (static_cast<int16_t>(stageData[i + 2]) << 8);

        for (size_t j = 0; j < runLength; ++j) {
            decompressedData.push_back(value);
        }

        i += 3;
    }

    if (decompressedData.size() != dataSize) {
        std::cerr << "\033[31m Decompression failed! Size mismatch. Expected: " << dataSize << ", Got: " << decompressedData.size() << std::endl << "\033[39m";
        return {};
    }

    return decompressedData;
}

std::vector<uint8_t> HATDecoder::decompressStage(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> decompressedData;
    size_t i = 0;

    while (i < data.size()) {
        uint8_t runLength = data[i];
        uint8_t value = data[i + 1];

        for (size_t j = 0; j < runLength; ++j) {
            decompressedData.push_back(value);
        }

        i += 2;
    }

    return decompressedData;
}