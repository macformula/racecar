#include "msg.hpp"

#include <array>
#include <cstring>

namespace macfe::can {

RawMessage::RawMessage(uint32_t id_, bool is_extended_frame_,
                       uint8_t data_length_, const uint8_t data_[8])
    : id(id_),
      is_extended_frame(is_extended_frame_),
      data_length(data_length_) {
    std::memcpy(data, data_, data_length);
}

RawMessage::RawMessage(uint32_t id_, bool is_extended_frame_,
                       uint8_t data_length_, const std::array<uint8_t, 8> data_)
    : id(id_),
      is_extended_frame(is_extended_frame_),
      data_length(data_length_) {
    std::memcpy(data, data_.data(), data_length);
}

RawMessage::RawMessage(const RawMessage& other)
    : id(other.id),
      is_extended_frame(other.is_extended_frame),
      data_length(other.data_length) {
    for (size_t i = 0; i < data_length; i++) {
        data[i] = other.data[i];
    }
}

}  // namespace macfe::can
