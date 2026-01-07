#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace macfe::cobs {

class Decoder {
public:
    Decoder(uint8_t* buffer);
    bool Decode(const uint8_t* encoded, size_t encoded_length);

    uint8_t* buffer;
    size_t length;

private:
    uint8_t block_remaining_;
    uint8_t code_;
};

constexpr size_t MaxEncodedLength(size_t raw_length);

size_t Encode(const uint8_t* raw, size_t length, uint8_t* output);

}  // namespace macfe::cobs