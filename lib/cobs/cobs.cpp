#include "cobs.hpp"

#include <cstdint>
#include <cstdlib>

namespace amber::cobs {

Decoder::Decoder(uint8_t* buffer)
    : buffer(buffer), length(0), block_remaining_(0), code_(0xff) {}

bool Decoder::Decode(const uint8_t* encoded, size_t encoded_length) {
    // adapted from
    // https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
    // differences: supports procedural decoding / does not require `encoded` to
    // contain the entire message

    const uint8_t* input_cursor = encoded;

    for (; input_cursor < encoded + encoded_length; --block_remaining_) {
        if (block_remaining_ > 0) {
            buffer[length++] = *input_cursor++;
        } else {
            block_remaining_ = *input_cursor++;
            if (block_remaining_ == 0) {
                return true;
            }
            if (code_ != 0xff) {
                buffer[length++] = 0;
            }
            code_ = block_remaining_;
        }
    }
    return false;
}

constexpr size_t MaxEncodedLength(size_t raw_length) {
    constexpr size_t LEADING_ZERO = 1;
    constexpr size_t TERMINATING_ZERO = 1;
    size_t MAX_STUFF_BYTES = (raw_length + 253) / 254;
    return LEADING_ZERO + raw_length + MAX_STUFF_BYTES + TERMINATING_ZERO;
}

size_t Encode(const uint8_t* raw, size_t length, uint8_t* output) {
    uint8_t* encode_cursor = output;

    uint8_t zero_offset = 1;
    uint8_t* zero_offset_p = encode_cursor++;

    for (const uint8_t* byte = raw; length--; ++byte) {
        if (*byte != 0) {
            *encode_cursor++ = *byte;
            ++zero_offset;
        }
        if ((*byte == 0) || (zero_offset == 0xff)) {
            *zero_offset_p = zero_offset;
            zero_offset = 1;
            zero_offset_p = encode_cursor;
            if ((*byte == 0) || (length > 0)) {
                ++encode_cursor;
            }
        }
    }
    *zero_offset_p = zero_offset;  // write the final zero_offset value
    *encode_cursor++ = 0;          // write delimiter

    return size_t(encode_cursor - output);
}

}  // namespace amber::cobs
