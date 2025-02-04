#pragma once

#include "shared/device/i2c_mux.hpp"
#include "shared/periph/i2c.hpp"

namespace shared::device::ti {

class TCA9548A : public I2CMultiplexer {
private:
    enum class ChannelStatus : uint8_t {
        UNSELECTED = 0,
        SELECTED = 1,
    };

public:
    TCA9548A(shared::periph::I2CBus& i2c_bus, uint8_t mux_address,
             bool force_select = false)
        : I2CMultiplexer(i2c_bus, mux_address),
          channel_status_(ChannelStatus::UNSELECTED),
          channel_selected_(0xFF) {}

    void WriteToChannel(uint8_t channel,
                        const shared::i2c::Message& msg) override {
        if (force_select_) {
            ForceSelectChannel(channel);
        } else {
            SoftSelectChannel(channel);
        }

        // Forward the message to selected channel
        i2c_bus_.Write(msg);
    }

    void ReadFromChannel(uint8_t channel, shared::i2c::Message& msg) override {
        if (force_select_) {
            ForceSelectChannel(channel);
        } else {
            SoftSelectChannel(channel);
        }

        // Read from selected channel
        i2c_bus_.Read(msg);
    }

private:
    void ForceSelectChannel(uint8_t channel) {
        uint8_t channel_select = 1 << channel;
        uint8_t select_data[] = {channel_select};
        shared::i2c::Message select_msg(mux_address_, select_data,
                                        shared::i2c::MessageType::Write);
        i2c_bus_.Write(select_msg);

        channel_status_ = ChannelStatus::SELECTED;
        channel_selected_ = channel;
    }

    void SoftSelectChannel(uint8_t channel) {
        if (channel_selected_ != channel ||
            channel_status_ == ChannelStatus::UNSELECTED) {
            uint8_t channel_select = 1 << channel;
            uint8_t select_data[] = {channel_select};
            shared::i2c::Message select_msg(mux_address_, select_data,
                                            shared::i2c::MessageType::Write);
            i2c_bus_.Write(select_msg);

            channel_status_ = ChannelStatus::SELECTED;
            channel_selected_ = channel;
        }
    }

    void DeselectChannel() {
        channel_status_ = ChannelStatus::UNSELECTED;
        channel_selected_ = 0xFF;
    }

    uint8_t channel_selected_;
    ChannelStatus channel_status_;
    bool force_select_;
};

}  // namespace shared::device::ti
