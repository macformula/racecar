/// @author Samuel Parent
/// @date 2024-01-06

#pragma once

#include <concepts>
#include <type_traits>

#include "shared/comms/can/raw_can_msg.h"

namespace shared::can {

class CanMsg {};

// Forward declaration of msg registry
class MsgRegistry;

class CanRxMsg : public CanMsg {
private:
    virtual void Clone(CanRxMsg&) const = 0;
    virtual void Unpack(const RawCanMsg&) = 0;
    virtual CanId Id() const = 0;

protected:
    template <typename T>
    static inline T unpack_right_shift(uint8_t value, uint8_t shift, uint8_t mask) {
        return static_cast<T>(static_cast<T>(value & mask) >> shift);
    }

    template <typename T>
    static inline T unpack_left_shift(uint8_t value, uint8_t shift, uint8_t mask)
    {
        return static_cast<T>(static_cast<T>(value & mask) << shift);
    }

    friend class MsgRegistry;
};

class CanTxMsg : public CanMsg {
private:
    virtual void Pack(RawCanMsg&) const = 0;

protected:
    template <typename T>
    static inline uint8_t pack_left_shift(T value, uint8_t shift,
                                          uint8_t mask) {
        return static_cast<uint8_t>(static_cast<uint8_t>(value << shift) &
                                    mask);
    }

    template <typename T>
    static inline uint8_t pack_right_shift(T value, uint8_t shift,
                                          uint8_t mask) {
        return static_cast<uint8_t>(static_cast<uint8_t>(value >> shift) &
                                    mask);
    }

    friend class CanBus;
};

}  // namespace shared::can
