#ifndef HATDECODE_H
#define HATDECODE_H

#include "HATFormat.h"
#include <string>
#include <vector>

class HATDecoder {
public:
    HATDecoder(const std::string& inputFilePath);
    void decode();

    const std::vector<int16_t>& getAudioData() const { return audioData; }
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

    void readHATHeader(std::ifstream& inputFile, HATHeader& header);
    void readTrackInfo(std::ifstream& inputFile, TrackInfo& trackInfo);
    std::vector<int16_t> decompressData(const std::vector<uint8_t>& compressedData, size_t dataSize);

private:
    std::string inputFilePath;
    std::vector<int16_t> audioData;
    HATHeader header;
    TrackInfo trackInfo;
};

#endif // HATDECODE_H
