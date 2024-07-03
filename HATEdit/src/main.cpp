#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <cstring> // Include this header for memcpy and memset
#include "HATDecode.h"
#include "HATFormat.h" // Include this header for HATHeader

class HATEditor {
public:
    HATEditor(const std::string& filePath) : filePath(filePath), decoder(filePath) {
        decoder.decode();
        loadFile();
        loadHeader();
    }

    void loadFile() {
        std::ifstream file(filePath, std::ios::binary);
        if (file.is_open()) {
            fileData.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
        } else {
            throw std::runtime_error("Failed to open file.");
        }
    }

    void saveFile(const std::string& outputFilePath) {
        std::ofstream file(outputFilePath, std::ios::binary);
        if (file.is_open()) {
            file.write(fileData.data(), fileData.size());
            file.close();
        } else {
            throw std::runtime_error("Failed to save file.");
        }
    }

    void loadHeader() {
        if (fileData.size() >= sizeof(HATHeader)) {
            std::memcpy(&header, fileData.data(), sizeof(HATHeader));
        } else {
            throw std::runtime_error("File too small to contain a valid header.");
        }
    }

    void saveHeader() {
        if (fileData.size() >= sizeof(HATHeader)) {
            std::memcpy(fileData.data(), &header, sizeof(HATHeader));
        } else {
            throw std::runtime_error("File too small to contain a valid header.");
        }
    }

    void printHeaderInfo() {
        std::cout << "HAT File Header Information:" << std::endl;
        std::cout << "Version: " << header.version << std::endl;
        std::cout << "Channels: " << static_cast<int>(header.channels) << std::endl;
        std::cout << "Spatial Data: (" << header.spatialData[0] << ", " << header.spatialData[1] << ", " << header.spatialData[2] << ")" << std::endl;
        std::cout << "Compression Ratio: " << header.compressionRatio << std::endl;
        std::cout << "Tracks: " << static_cast<int>(header.tracks) << std::endl;
        std::cout << "Sample Rate: " << header.sampleRate << std::endl;
        std::cout << "Bit Rate: " << header.bitRate << std::endl;
        std::cout << "Length: " << header.length << std::endl;
        std::cout << "Data Length: " << header.datalength << std::endl;
    }

    void printTrackInfo() {
        std::cout << "Track Information:" << std::endl;
        std::cout << "Artist: " << decoder.getArtist() << std::endl;
        std::cout << "Description: " << decoder.getDescription() << std::endl;
        std::cout << "Track Name: " << decoder.getTrackName() << std::endl;
        std::cout << "Track Number: " << decoder.getTrackNumber() << std::endl;
    }

    void editArtist(const std::string& newArtist) {
        updateField(decoder.getArtist(), newArtist);
    }

    void editTrackName(const std::string& newTrackName) {
        updateField(decoder.getTrackName(), newTrackName);
    }

    void editDescription(const std::string& newDescription) {
        updateField(decoder.getDescription(), newDescription);
    }

    void editVersion(const std::string& newVersion) {
        header.version = newVersion;
        pendingChanges.push_back("Updated version to " + newVersion);
    }

    void editChannels(uint8_t newChannels) {
        header.channels = newChannels;
        pendingChanges.push_back("Updated channels to " + std::to_string(newChannels));
    }

    void editSpatialData(float x, float y, float z) {
        header.spatialData[0] = x;
        header.spatialData[1] = y;
        header.spatialData[2] = z;
        pendingChanges.push_back("Updated spatial data to (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
    }

    void editCompressionRatio(float newCompressionRatio) {
        header.compressionRatio = newCompressionRatio;
        pendingChanges.push_back("Updated compression ratio to " + std::to_string(newCompressionRatio));
    }

    void editTracks(uint8_t newTracks) {
        header.tracks = newTracks;
        pendingChanges.push_back("Updated tracks to " + std::to_string(newTracks));
    }

    void editSampleRate(uint32_t newSampleRate) {
        header.sampleRate = newSampleRate;
        pendingChanges.push_back("Updated sample rate to " + std::to_string(newSampleRate));
    }

    void editBitRate(uint32_t newBitRate) {
        header.bitRate = newBitRate;
        pendingChanges.push_back("Updated bit rate to " + std::to_string(newBitRate));
    }

    void viewPendingChanges() const {
        std::cout << "Pending Changes:" << std::endl;
        for (size_t i = 0; i < pendingChanges.size(); ++i) {
            std::cout << i + 1 << ": " << pendingChanges[i] << std::endl;
        }
    }

    void deletePendingChange(size_t index) {
        if (index > 0 && index <= pendingChanges.size()) {
            pendingChanges.erase(pendingChanges.begin() + index - 1);
            std::cout << "Change " << index << " deleted." << std::endl;
        } else {
            std::cerr << "Invalid index." << std::endl;
        }
    }

private:
    void updateField(const std::string& oldValue, const std::string& newValue) {
        if (oldValue != newValue) {
            auto pos = std::search(fileData.begin(), fileData.end(), oldValue.begin(), oldValue.end());
            if (pos != fileData.end()) {
                std::copy(newValue.begin(), newValue.end(), pos);
                pendingChanges.push_back("Updated from \"" + oldValue + "\" to \"" + newValue + "\"");
            } else {
                std::cerr << "Failed to update: old value not found in file." << std::endl;
            }
        }
    }

    std::string filePath;
    std::vector<char> fileData;
    std::vector<std::string> pendingChanges;
    HATDecoder decoder;
    HATHeader header;
};

void showCommands() {
    std::cout << "Commands:" << std::endl;
    std::cout << "1. view header - View HAT file header information" << std::endl;
    std::cout << "2. view track - View track information" << std::endl;
    std::cout << "3. edit artist - Edit artist name" << std::endl;
    std::cout << "4. edit trackname - Edit track name" << std::endl;
    std::cout << "5. edit description - Edit description" << std::endl;
    std::cout << "6. edit version - Edit version (Warning: Can break HAT audio files)" << std::endl;
    std::cout << "7. edit channels - Edit channels (Warning: Can break HAT audio files)" << std::endl;
    std::cout << "8. edit spatialdata - Edit spatial data (Warning: Can break HAT audio files)" << std::endl;
    std::cout << "9. edit compressionratio - Edit compression ratio (Warning: Can break HAT audio files)" << std::endl;
    std::cout << "10. edit tracks - Edit tracks (Warning: Can break HAT audio files)" << std::endl;
    std::cout << "11. edit samplerate - Edit sample rate (Warning: Can break HAT audio files)" << std::endl;
    std::cout << "12. edit bitrate - Edit bit rate (Warning: Can break HAT audio files)" << std::endl;
    std::cout << "13. view changes - View pending changes" << std::endl;
    std::cout << "14. delete change - Delete a pending change" << std::endl;
    std::cout << "15. save - Save the edited HAT file" << std::endl;
    std::cout << "16. exit - Exit the program" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: HATEdit <input HAT file>" << std::endl;
        return 1;
    }

    std::string inputFilePath = argv[1];

    try {
        HATEditor editor(inputFilePath);

        std::string command;
        showCommands();

        while (true) {
            std::cout << "\nEnter command: ";
            std::getline(std::cin, command);

            if (command == "view header") {
                editor.printHeaderInfo();
            } else if (command == "view track") {
                editor.printTrackInfo();
            } else if (command == "edit artist") {
                std::string newArtist;
                std::cout << "Enter new artist: ";
                std::getline(std::cin, newArtist);
                editor.editArtist(newArtist);
            } else if (command == "edit trackname") {
                std::string newTrackName;
                std::cout << "Enter new track name: ";
                std::getline(std::cin, newTrackName);
                editor.editTrackName(newTrackName);
            } else if (command == "edit description") {
                std::string newDescription;
                std::cout << "Enter new description: ";
                std::getline(std::cin, newDescription);
                editor.editDescription(newDescription);
            } else if (command == "edit version") {
                std::string newVersion;
                std::cout << "Enter new version: ";
                std::getline(std::cin, newVersion);
                editor.editVersion(newVersion);
                std::cout << "\033[31mWarning: Editing the version can break HAT audio files.\033[0m" << std::endl;
            } else if (command == "edit channels") {
                uint8_t newChannels;
                std::cout << "Enter new channels: ";
                std::cin >> newChannels;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
                editor.editChannels(newChannels);
                std::cout << "\033[31mWarning: Editing the channels can break HAT audio files.\033[0m" << std::endl;
            } else if (command == "edit spatialdata") {
                float x, y, z;
                std::cout << "Enter new spatial data (format: x y z): ";
                std::cin >> x >> y >> z;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
                editor.editSpatialData(x, y, z);
                std::cout << "\033[31mWarning: Editing the spatial data can break HAT audio files.\033[0m" << std::endl;
            } else if (command == "edit compressionratio") {
                float newCompressionRatio;
                std::cout << "Enter new compression ratio: ";
                std::cin >> newCompressionRatio;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
                editor.editCompressionRatio(newCompressionRatio);
                std::cout << "\033[31mWarning: Editing the compression ratio can break HAT audio files.\033[0m" << std::endl;
            } else if (command == "edit tracks") {
                uint8_t newTracks;
                std::cout << "Enter new tracks: ";
                std::cin >> newTracks;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
                editor.editTracks(newTracks);
                std::cout << "\033[31mWarning: Editing the tracks can break HAT audio files.\033[0m" << std::endl;
            } else if (command == "edit samplerate") {
                uint32_t newSampleRate;
                std::cout << "Enter new sample rate: ";
                std::cin >> newSampleRate;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
                editor.editSampleRate(newSampleRate);
                std::cout << "\033[31mWarning: Editing the sample rate can break HAT audio files.\033[0m" << std::endl;
            } else if (command == "edit bitrate") {
                uint32_t newBitRate;
                std::cout << "Enter new bit rate: ";
                std::cin >> newBitRate;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
                editor.editBitRate(newBitRate);
                std::cout << "\033[31mWarning: Editing the bit rate can break HAT audio files.\033[0m" << std::endl;
            } else if (command == "view changes") {
                editor.viewPendingChanges();
            } else if (command == "delete change") {
                size_t index;
                std::cout << "Enter change index to delete: ";
                std::cin >> index;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
                editor.deletePendingChange(index);
            } else if (command == "save") {
                std::string outputFilePath;
                std::cout << "Enter output file path: ";
                std::getline(std::cin, outputFilePath);
                editor.saveHeader();
                editor.saveFile(outputFilePath);
                std::cout << "HAT file saved to " << outputFilePath << std::endl;
            } else if (command == "exit") {
                break;
            } else {
                std::cout << "Unknown command." << std::endl;
                showCommands();
            }
        }

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
