#ifndef HATFORMAT_H
#define HATFORMAT_H

#include <string>
#include <vector>
#include <cstdint>

const std::string HAT_VERSION = "1.0";
const int MAX_CHANNELS = 32;

enum CompressionMethod {
    NONE,
    LOSSLESS
};

struct HATHeader {
    std::string version;
    int channels;
    float spatialData[3]; // x, y, z
    float compressionRatio;
    CompressionMethod compressionMethod;
    int tracks;
    int sampleRate;
    int bitRate;
    int length;
};

struct TrackInfo {
    std::string artist;
    std::string description;
    std::string trackName;
    int trackNumber;
    int seekMarker;
};

struct CompressionDataMarker {
    int position;
    int length;
};

// Functions for handling HAT format (if any)
void printHATHeader(const HATHeader& header);

#endif // HATFORMAT_H
