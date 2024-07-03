#ifndef HATDECODE_H
#define HATDECODE_H

#include <string>
#include <vector>
#include <cstdint> // Ensure this is included
#include "HATFormat.h"

class HATDecoder {
public:
    HATDecoder(const std::string& inputFilePath);
    void decode();
    
    int getSampleRate() const { return header.sampleRate; }
    int getBitRate() const { return header.bitRate; }
    int getChannels() const { return header.channels; }
    int getTracks() const { return header.tracks; }
    float getCompressionRatio() const { return header.compressionRatio; }
    const std::string& getVersion() const { return header.version; }
    const float* getSpatialData() const { return header.spatialData; }
    const std::string& getArtist() const { return trackInfo.artist; }
    const std::string& getDescription() const { return trackInfo.description; }
    const std::string& getTrackName() const { return trackInfo.trackName; }
    int getTrackNumber() const { return trackInfo.trackNumber; }
    const std::vector<int16_t>& getAudioData() const { return audioData; }
    std::vector<int16_t> decompressData(const std::vector<uint8_t>& compressedData, size_t dataSize); 
    std::vector<uint8_t> decompressStage(const std::vector<uint8_t>& data);

private:
    std::string inputFilePath;
    HATHeader header;
    TrackInfo trackInfo;
    std::vector<int16_t> audioData;

    void readHATHeader(std::ifstream& inputFile, HATHeader& header);
    void readTrackInfo(std::ifstream& inputFile, TrackInfo& trackInfo);

};

#endif
