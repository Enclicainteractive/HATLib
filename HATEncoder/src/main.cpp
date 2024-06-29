#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include "HATEncode.h"
#include <iostream>
#include <unordered_map>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: HATEncoder <input WAV file> <output HAT file> <artist>" << std::endl;
        return 1;
    }

    std::string inputFilePath = argv[1];
    std::string outputFilePath = argv[2];
    std::string artist = argv[3];

    drwav wav;
    if (!drwav_init_file(&wav, inputFilePath.c_str(), NULL)) {
        std::cerr << "Failed to open WAV file." << std::endl;
        return 1;
    }

    int sampleRate = wav.sampleRate;
    int bitRate = wav.bitsPerSample * wav.channels * wav.sampleRate;
    int audioChannels = wav.channels;

    std::unordered_map<std::string, std::string> metadata;
    metadata["artist"] = artist;

    // Read PCM frames
    std::vector<int16_t> audioData(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, audioData.data());

    drwav_uninit(&wav);

    HATEncoder encoder(inputFilePath, outputFilePath, sampleRate, bitRate, audioChannels, metadata);
    encoder.encode();

    std::cout << "Encoding complete." << std::endl;
    return 0;
}
