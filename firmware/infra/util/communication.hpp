/// @author Blake Freer, Clara Wong
/// @date 2023-10-23
/// @note Taken from Christopher Kormanyos

#ifndef UTIL_COMMUNICATION_HPP
#define UTIL_COMMUNICATION_HPP

#include "..\util\noncopyable.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace util {

/// @brief Base class for all communication protocols (SPI, CAN, ...)
    class CommunicationBase : private util::noncopyable {
        public:
            virtual ~CommunicationBase() = default;

            //receive data
            virtual auto Recv(std::uint8_t& byte_to_recv) -> bool = 0;

            //select communication channel
            virtual auto Select() -> void = 0;
            //deselect communication channel
            virtual auto Deselect() -> void = 0;

            //select a specific channel (w/ a default implementation)
            virtual auto SelectChannel(const std::size_t) -> bool {
                return true;
            }

            //send data using iterators
            template<typename SendIteratorType>
            auto SendN(SendIteratorType first, SendIteratorType last) noexcept -> bool {
                auto SendResult = true;
                while((first != last) && SendResult) {
                    using SendValueType = type name std::iterator_traits<SendIteratorType>::value_type;

                    SendResult = (this->Send(static_cast<std::uint8_t>(SendValueType(*first++))) && SendResult);
                }
                return SendResult;
            }

            //send data
            virtual auto Send(const std::uint8_t byte_to_send) noexcept -> bool = 0;
        
        protected:
            CommunicationBase() = default;
        
        private:
            template<const std::size_t channel_count>
            friend class CommunicationMultiChannel;
    };
    
    class CommunicationBufferDepthOneByte : public CommunicationBase {
    // communication channel with a one-byte buffer for receiving data. Recv function retrieves data from the buffer
        public:
            using BufferType = std::uint8_t;

            ~CommunicationBufferDepthOneByte() override = default;
        
        protected:
            BufferType recv_buffer { };
            CommunicationBufferDepthOneByte() = default;

        private:
            auto Recv(std::uint8_t& byte_to_recv) -> bool override {
                byte_to_recv = recv_buffer;
                return true;
            }
    };

    template<const std::size_t channel_count>
    class CommunicationMultiChannel : public CommunicationBase {
    // creation of multi-channel communication objects. Manages an array of pointers to communication channels and provides methods to send and receive data, select a channel, and handle multiple channels
        public:
            explicit CommunicationMultiChannel(CommunicationBase** p_com_channels) {
                std::copy(p_com_channels, p_com_channels + channel_count, my_com_channels.begin());
            }
            ~CommunicationMultiChannel() override = default;

            auto Send(const std::uint8_t byte_to_send) noexcept -> bool override {
                return my_com_channels[my_index] -> Send(byte_to_send);
            }
            auto Recv(std::uint8_t& byte_to_recv) -> bool override {
                return my_com_channels[my_index] -> recv(byte_to_recv);
            }

            auto Select() -> void override {
                my_com_channels[my_index] -> Select();
            }
            auto Deselect() -> void override {
                my_com_channels[my_index] -> Deselect();
            }

            auto SelectChannel(const std::size_t index) -> bool override {
                const bool select_channel_is_ok = (index < channel_count);
                if(SelectChannelIsOk) {
                    my_index = index;
                }
                return SelectChannelIsOk;
            }

        private:
            std::array<CommunicationBase*, channel_count>my_com_channels { };
            std::size_t    my_index { };

            CommunicationMultiChannel() = delete;

            static_assert(channel_count > 0U, "Error, channel_count must be greater than zero.");
    };

} 

#endif