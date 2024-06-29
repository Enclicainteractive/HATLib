#include "HATFormat.h"
#include <iostream>

void printHATHeader(const HATHeader& header) {
    std::cout << "HAT Version: " << header.version << std::endl;
    std::cout << "Channels: " << header.channels << std::endl;
    std::cout << "Spatial Data: (" << header.spatialData[0] << ", "
              << header.spatialData[1] << ", " << header.spatialData[2] << ")" << std::endl;
    std::cout << "Compression Ratio: " << header.compressionRatio << std::endl;
    std::cout << "Compression Method: " << header.compressionMethod << std::endl;
    std::cout << "Tracks: " << header.tracks << std::endl;
    std::cout << "Sample Rate: " << header.sampleRate << std::endl;
    std::cout << "Bit Rate: " << header.bitRate << std::endl;
    std::cout << "Length: " << header.length << std::endl;
}
