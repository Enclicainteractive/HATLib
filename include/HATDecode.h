#ifndef HATDECODE_H
#define HATDECODE_H

#include "HATFormat.h"
#include <string>
#include <vector>

class HATDecoder {
public:
    HATDecoder(const std::string& inputFilePath);
    void decode();

private:
    std::string inputFilePath;
    std::vector<int16_t> audioData;

    void readHATHeader(std::ifstream& inputFile, HATHeader& header);
    void readTrackInfo(std::ifstream& inputFile, TrackInfo& trackInfo);
    std::vector<int16_t> decompressData(const std::vector<int16_t>& compressedData, float compressionRatio);
};

#endif // HATDECODE_H
