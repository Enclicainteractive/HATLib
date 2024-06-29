#ifndef HATENCODE_H
#define HATENCODE_H

#include "HATFormat.h"
#include <string>
#include <vector>
#include <unordered_map>

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

    void readWavFile(std::ifstream& inputFile);
    std::vector<int16_t> compressData(const std::vector<int16_t>& data, float compressionRatio);
    void writeHATFile(const HATHeader& header, const TrackInfo& trackInfo, const std::vector<int16_t>& compressedData);
};

#endif // HATENCODE_H