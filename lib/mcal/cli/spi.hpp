/// @author Priyal Taneja
/// @date 2025-10-19

#pragma once
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <string>

#include "periph/spi.hpp"

namespace mcal::cli {

class SpiMaster : public macfe::periph::SpiMaster {
private:
    std::string name_;

public:
    SpiMaster(std::string name) : name_(name) {
        std::cout << std::format("Created SPI Master {}", name_) << std::endl;
    }

    void Transmit(uint8_t* tx_data, size_t length) override {
        std::cout << std::format("SPI {} Transmitting {} bytes: ", name_,
                                 length);
        for (size_t i = 0; i < length && i < 16; ++i) {
            std::cout << std::format("{:02X} ", tx_data[i]);
        }
        if (length > 16) {
            std::cout << "...";
        }
        std::cout << std::endl;
    }

    void Receive(uint8_t* rx_data, size_t length) override {
        std::cout << std::format("SPI {} Receiving {} bytes", name_, length)
                  << std::endl;
        std::cout << " | Enter hex values (e.g., FF 00 A5) or press Enter for "
                     "default 0xFF: ";

        // read line of input
        std::string line;
        std::getline(std::cin >> std::ws, line);

        if (line.empty()) {
            // default: fill with 0xFF
            memset(rx_data, 0xFF, length);
        } else {
            // parse hex values from input
            size_t idx = 0;
            size_t pos = 0;
            while (pos < line.length() && idx < length) {
                if (std::isxdigit(line[pos])) {
                    unsigned int value;
                    std::sscanf(line.c_str() + pos, "%2x", &value);
                    rx_data[idx++] = static_cast<uint8_t>(value);
                    pos += 2;
                } else {
                    pos++;
                }
            }
            // fill remaining data with 0xFF
            while (idx < length) {
                rx_data[idx++] = 0xFF;
            }
        }

        std::cout << " | Received: ";
        for (size_t i = 0; i < length && i < 16; ++i) {
            std::cout << std::format("{:02X} ", rx_data[i]);
        }
        if (length > 16) {
            std::cout << "...";
        }
        std::cout << std::endl;
    }

    void TransmitReceive(uint8_t* tx_data, uint8_t* rx_data,
                         size_t length) override {
        std::cout << std::format("SPI {} TransmitReceive {} bytes", name_,
                                 length)
                  << std::endl;
        std::cout << " | TX: ";
        for (size_t i = 0; i < length && i < 16; ++i) {
            std::cout << std::format("{:02X} ", tx_data[i]);
        }
        if (length > 16) {
            std::cout << "...";
        }
        std::cout << std::endl;

        std::cout << " | Enter RX hex values or press Enter for default 0xFF: ";

        // read line of input
        std::string line;
        std::getline(std::cin >> std::ws, line);

        if (line.empty()) {
            // default: fill with 0xFF
            memset(rx_data, 0xFF, length);
        } else {
            // parse hex values from input
            size_t idx = 0;
            size_t pos = 0;
            while (pos < line.length() && idx < length) {
                if (std::isxdigit(line[pos])) {
                    unsigned int value;
                    std::sscanf(line.c_str() + pos, "%2x", &value);
                    rx_data[idx++] = static_cast<uint8_t>(value);
                    pos += 2;
                } else {
                    pos++;
                }
            }
            // fill remaining data with 0xFF
            while (idx < length) {
                rx_data[idx++] = 0xFF;
            }
        }

        std::cout << " | RX: ";
        for (size_t i = 0; i < length && i < 16; ++i) {
            std::cout << std::format("{:02X} ", rx_data[i]);
        }
        if (length > 16) {
            std::cout << "...";
        }
        std::cout << std::endl;
    }
};

}  // namespace mcal::cli