#include "msg.h"

namespace shared::can {

RawMessage::RawMessage(uint32_t id, uint8_t data_length, const uint8_t data[8])
    : id_(id), data_length(data_length) {
    std::memcpy(data_, data, data_length);
}

}  // namespace shared::can