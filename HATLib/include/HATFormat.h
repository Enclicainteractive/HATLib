#ifndef HATFORMAT_H
#define HATFORMAT_H

#ifdef _WIN32
    #ifdef HATLIB_EXPORTS
        #define HATLIB_API __declspec(dllexport)
    #else
        #define HATLIB_API __declspec(dllimport)
    #endif
#else
    #define HATLIB_API
#endif

#include <string>
#include <vector>
#include <cstdint>

const std::string HAT_VERSION = "1.0";
const int MAX_CHANNELS = 32;

enum CompressionMethod {
    NONE,
    LOSSLESS
};

struct HATLIB_API HATHeader {
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

struct HATLIB_API TrackInfo {
    std::string artist;
    std::string description;
    std::string trackName;
    int trackNumber;
    int seekMarker;
};

struct HATLIB_API CompressionDataMarker {
    int position;
    int length;
};

// Functions for handling HAT format (if any)
HATLIB_API void printHATHeader(const HATHeader& header);

#endif // HATFORMAT_H
