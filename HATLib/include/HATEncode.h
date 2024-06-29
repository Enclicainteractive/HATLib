#ifndef HATENCODE_H
#define HATENCODE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint> // for int16_t
#include "HATFormat.h" // Include the HAT format definitions

class HATEncoder {
public:
    HATEncoder(const std::string& inputFilePath, const std::string& outputFilePath, int sampleRate, int bitRate, int audioChannels, const std::unordered_map<std::string, std::string>& metadata);
    void encode();
    void readWavFile();
    std::vector<uint8_t> compressData(const std::vector<int16_t>& data, float& compressionRatio); 
    void writeHATFile(const HATHeader& header, const TrackInfo& trackInfo, const std::vector<uint8_t>& compressedData);

private:
    std::string inputFilePath;
    std::string outputFilePath;
    int sampleRate;
    int bitRate;
    int audioChannels;
    std::vector<int16_t> audioData;
    std::unordered_map<std::string, std::string> metadata;
};

#endif // HATENCODE_H
