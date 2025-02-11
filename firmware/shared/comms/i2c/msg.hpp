#include <concepts>
#include <cstdint>
#include <cstring>
#include <format>
#include <iomanip>
#include <sstream>
#include <string>

#pragma once

namespace shared::i2c {

enum class MessageType {
    Read,
    Write,
};

struct Message {
    // Pure virtual template method that derived classes must implement
    template <size_t N>
    Message(uint8_t address, uint8_t (&data)[N], MessageType type)
        : address_(address), data_length_(N), data_(data), type_(type) {}

    // Delete default constructor
    Message() = delete;

private:
    uint8_t address_;
    uint8_t data_length_;
    uint8_t* data_;
    MessageType type_;

public:
    MessageType Type() const {
        return type_;
    }
    uint8_t Address() const {
        return address_;
    }

    size_t DataLength() const {
        return data_length_;
    }

    uint8_t* Data() const {
        return data_;
    }

    // Pure virtual template method that derived classes must implement
    template <size_t N>
    bool SetData(const uint8_t (&new_data)[N]) {
        std::memcpy(data_, new_data, N);
        return true;
    }

    // Convert to string method
    std::string ToString() const {
        std::stringstream ss;

        // Format address
        ss << "[" << std::uppercase << std::hex << std::setw(2)
           << std::setfill('0') << static_cast<int>(address_) << "]";

        // Format message type
        if (type_ == MessageType::Write) {
            ss << " WR -> ";
        } else {
            ss << " RD <- ";
        }

        // Format data length
        ss << "(" << static_cast<int>(data_length_) << ")";

        // Format data
        for (size_t i = 0; i < data_length_; ++i) {
            ss << " " << std::uppercase << std::hex << std::setw(2)
               << std::setfill('0') << static_cast<int>(data_[i]);
        }

        return ss.str();
    }
};

}  // namespace shared::i2c

// Operator overload for stream insertion
inline std::ostream& operator<<(std::ostream& os,
                                const shared::i2c::Message& msg) {
    return os << msg.ToString();
}
