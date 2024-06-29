#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <vector>
#include <map>
#include <cstring> // for std::memcpy

class ArithmeticCoding {
public:
    void encode(const std::vector<int16_t>& data, std::vector<uint16_t>& encodedData) {
        // Calculate frequency of each symbol
        std::map<int16_t, int> frequency;
        for (int16_t symbol : data) {
            frequency[symbol]++;
        }

        // Calculate cumulative frequency
        std::map<int16_t, double> cumulativeFreq;
        double cumFreq = 0.0;
        for (auto& pair : frequency) {
            cumulativeFreq[pair.first] = cumFreq;
            cumFreq += static_cast<double>(pair.second) / data.size();
        }

        // Encode data
        double low = 0.0, high = 1.0;
        for (int16_t symbol : data) {
            double range = high - low;
            high = low + range * (cumulativeFreq[symbol] + static_cast<double>(frequency[symbol]) / data.size());
            low = low + range * cumulativeFreq[symbol];
        }

        // Store the final value (midpoint of the interval)
        double finalValue = (low + high) / 2;
        encodedData = doubleToBytes(finalValue);
    }

    void decode(const std::vector<uint16_t>& encodedData, size_t dataSize, std::vector<int16_t>& decodedData) {
        // Reconstruct the final value
        double finalValue = bytesToDouble(encodedData);

        // Calculate frequency of each symbol
        std::map<int16_t, int> frequency = calculateFrequency(decodedData);

        // Calculate cumulative frequency
        std::map<int16_t, double> cumulativeFreq;
        double cumFreq = 0.0;
        for (auto& pair : frequency) {
            cumulativeFreq[pair.first] = cumFreq;
            cumFreq += static_cast<double>(pair.second) / dataSize;
        }

        // Decode data
        double low = 0.0, high = 1.0, value = finalValue;
        for (size_t i = 0; i < dataSize; ++i) {
            double range = high - low;
            double scaledValue = (value - low) / range;

            int16_t symbol = findSymbol(cumulativeFreq, scaledValue);
            decodedData.push_back(symbol);

            high = low + range * (cumulativeFreq[symbol] + static_cast<double>(frequency[symbol]) / dataSize);
            low = low + range * cumulativeFreq[symbol];
        }
    }

private:
    std::vector<uint16_t> doubleToBytes(double value) {
        std::vector<uint16_t> bytes(sizeof(double) / sizeof(uint16_t));
        std::memcpy(bytes.data(), &value, sizeof(double));
        return bytes;
    }

    double bytesToDouble(const std::vector<uint16_t>& bytes) {
        double value;
        std::memcpy(&value, bytes.data(), sizeof(double));
        return value;
    }

    std::map<int16_t, int> calculateFrequency(const std::vector<int16_t>& data) {
        std::map<int16_t, int> frequency;
        for (int16_t symbol : data) {
            frequency[symbol]++;
        }
        return frequency;
    }

    int16_t findSymbol(const std::map<int16_t, double>& cumulativeFreq, double value) {
        for (auto it = cumulativeFreq.rbegin(); it != cumulativeFreq.rend(); ++it) {
            if (value >= it->second) {
                return it->first;
            }
        }
        return cumulativeFreq.begin()->first; // Default return
    }
};

#endif // ARITHMETIC_H
