#ifndef HATFORMAT_H
#define HATFORMAT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint> // Ensure this is included

const std::string HAT_VERSION = "1.0";

enum CompressionMethod {
    LOSSLESS
};

struct HATHeader {
    std::string version;
    uint8_t channels;
    float spatialData[3];
    float compressionRatio;
    CompressionMethod compressionMethod;
    uint8_t tracks;
    uint32_t sampleRate;
    uint32_t bitRate;
    uint32_t length;
    uint32_t datalength;
};

struct TrackInfo {
    std::string artist;
    std::string description;
    std::string trackName;
    int32_t trackNumber;
    uint32_t seekMarker;
};

uint16_t calculateChecksum(const std::vector<uint8_t>& data);
std::vector<uint8_t> compressData(const std::vector<int16_t>& data, float& compressionRatio);
std::vector<int16_t> decompressData(const std::vector<uint8_t>& compressedData, size_t dataSize);

#endif
