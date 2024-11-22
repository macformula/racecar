#include <cstring>

#include "msg.h"

namespace shared::can {

RawMessage::RawMessage(uint32_t id_, uint8_t data_length_,
                       const uint8_t data_[8])
    : id(id_), data_length(data_length_) {
    std::memcpy(data, data_, data_length);
}

}  // namespace shared::can