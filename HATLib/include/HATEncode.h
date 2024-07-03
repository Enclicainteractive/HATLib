#ifndef HATENCODE_H
#define HATENCODE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint> // Ensure this is included
#include "HATFormat.h"

class HATEncoder {
public:
    HATEncoder(const std::string& inputFilePath, const std::string& outputFilePath, int sampleRate, int bitRate, int audioChannels, const std::unordered_map<std::string, std::string>& metadata);
    void encode();
    
private:
    std::string inputFilePath;
    std::string outputFilePath;
    int sampleRate;
    int bitRate;
    int audioChannels;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<int16_t> audioData;

    void readWavFile();
    void writeHATFile(const HATHeader& header, const TrackInfo& trackInfo, const std::vector<uint8_t>& compressedData);
};

#endif
