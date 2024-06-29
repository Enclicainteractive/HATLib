#include "HATDecode.h"
#include <fstream>
#include <iostream>
#include <zlib.h>
#include "HATFormat.h"

HATDecoder::HATDecoder(const std::string& inputFilePath)
    : inputFilePath(inputFilePath) {}

void HATDecoder::decode() {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file." << std::endl;
        return;
    }

    readHATHeader(inputFile, header);
    readTrackInfo(inputFile, trackInfo);

    std::vector<uint8_t> compressedData(header.length);
    inputFile.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());

    audioData = decompressData(compressedData, header.length);

    inputFile.close();
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

    char eof[6];
    inputFile.read(eof, 6);
    if (std::string(eof, 6) != "HATEOF") {
        std::cerr << "Error: Invalid HAT file." << std::endl;
        return;
    }
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
}

std::vector<int16_t> HATDecoder::decompressData(const std::vector<uint8_t>& compressedData, size_t originalSize) {
    uLongf decompressedSize = originalSize * sizeof(int16_t);
    std::vector<int16_t> decompressedData(decompressedSize / sizeof(int16_t));

    if (uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedSize, compressedData.data(), compressedData.size()) != Z_OK) {
        std::cerr << "Decompression failed!" << std::endl;
        return {};
    }

    decompressedData.resize(decompressedSize / sizeof(int16_t));
    return decompressedData;
}
