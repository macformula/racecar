/// @author Samuel Parent
/// @date 2023-01-12

#pragma once

#include <bits/ranges_base.h>
#include <sys/_stdint.h>

#include <cstddef>

#include "shared/comms/can/raw_can_msg.h"
#include "shared/periph/can.h"
#include "shared/util/data_structures/circular_queue.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_can.h"

namespace mcal::stm32f767::periph {

class CanBase : public shared::periph::CanBase {
public:
    CanBase(CAN_HandleTypeDef* hcan) : hcan_(hcan){};

    void Setup() {
        ConfigFilters();
        HAL_CAN_Start(hcan_);
        HAL_CAN_ActivateNotification(hcan_, kCanRxActiveInterruptFifo0);
    }

    void Send(const shared::can::RawCanMsg& can_tx_msg) override {
        uint32_t tx_mailboxes_free_level =
            HAL_CAN_GetTxMailboxesFreeLevel(hcan_);
        if (tx_mailboxes_free_level < 1) {
            dropped_tx_frames_ += 1;
            return;
        }

        CAN_TxHeaderTypeDef stm_tx_header =
            pack_stm_tx_header(can_tx_msg.header);

        HAL_CAN_AddTxMessage(hcan_, &stm_tx_header, can_tx_msg.data,
                             &tx_mailbox_addr_);
    }

    void SendDebugMsg() {
        shared::can::CanHeader hdr = {
            .id=0x123, 
            .data_len=8, 
            .is_extended_frame=false
        };

        uint32_t time = get_tick_ms();

        shared::can::RawCanMsg raw_msg{
            .header=hdr,  
             .data = {
            0xDE,
            0xAD,
            0xBE,
            0xEF,
            static_cast<uint8_t>((time>>3)),
            static_cast<uint8_t>((time>>2)),
            static_cast<uint8_t>((time>>1)),
            static_cast<uint8_t>((time>>0)),
            },
            .tick_timestamp=get_tick_ms(),
        };
        
        Send(raw_msg);
    }

    void ReadQueue(shared::can::RawCanMsg can_rx_msgs[], size_t len) override {
        uint16_t msg_idx = 0;
        {
            while (!can_queue_.is_empty() && (msg_idx < len)) {
                can_rx_msgs[msg_idx] = can_queue_.Dequeue();
                msg_idx++;
            }
        }
    }

    void AddRxMessageToQueue() {
        if (can_queue_.is_full()) {
            dropped_rx_frames_ += 1;
            return;
        }

        uint32_t fifo0_fill_level =
            HAL_CAN_GetRxFifoFillLevel(hcan_, kCanRxFifo0);
        if (fifo0_fill_level == 0) {
            false_add_rx_msg_to_queue_ += 1;
            return;
        }

        CAN_RxHeaderTypeDef stm_rx_header;
        shared::can::RawCanMsg raw_rx_msg;

        raw_rx_msg.tick_timestamp = get_tick_ms();

        HAL_CAN_GetRxMessage(hcan_, kCanRxFifo0, &stm_rx_header,
                             raw_rx_msg.data);

        raw_rx_msg.header = unpack_stm_rx_header(stm_rx_header);

        can_queue_.Enqueue(raw_rx_msg);
    }

private:
    static constexpr uint8_t kMaxMsgBytes = 8;
    static constexpr size_t kMsgQueueLen = 20;
    static constexpr uint32_t kCanRxFifo0 = CAN_RX_FIFO0;
    static constexpr uint32_t kCanRxActiveInterruptFifo0 =
        CAN_IT_RX_FIFO0_MSG_PENDING;

    static constexpr CAN_TxHeaderTypeDef pack_stm_tx_header(
        const shared::can::CanHeader& header) {
        CAN_TxHeaderTypeDef ret_stm_tx_header;

        // Set frame id based on standard or extended frame id
        if (header.is_extended_frame) {
            ret_stm_tx_header.IDE = CAN_ID_EXT;
            ret_stm_tx_header.ExtId = header.id;
        } else {
            ret_stm_tx_header.IDE = CAN_ID_STD;
            ret_stm_tx_header.StdId = header.id;
        }

        // Sending a data frame
        ret_stm_tx_header.RTR = CAN_RTR_DATA;

        // Set data length
        ret_stm_tx_header.DLC = header.data_len;

        return ret_stm_tx_header;
    }

    static constexpr shared::can::CanHeader unpack_stm_rx_header(
        const CAN_RxHeaderTypeDef& stm_rx_header) {
        shared::can::CanHeader ret_rx_header;

        if (stm_rx_header.IDE == CAN_ID_EXT) {
            ret_rx_header.is_extended_frame = true;
            ret_rx_header.id = stm_rx_header.ExtId;
        } else {  // Otherwise assume standard ID
            ret_rx_header.is_extended_frame = false;
            ret_rx_header.id = stm_rx_header.StdId;
        }

        ret_rx_header.data_len = static_cast<uint8_t>(stm_rx_header.DLC);

        return ret_rx_header;
    }

    inline uint32_t get_tick_ms() {
        return HAL_GetTick() * HAL_GetTickFreq();
    }

    void ConfigFilters() {
        CAN_FilterTypeDef filter_config;

        filter_config.FilterFIFOAssignment=CAN_RX_FIFO0;
        filter_config.FilterIdHigh = 0x0000;
        filter_config.FilterIdLow = 0x0000;
        filter_config.FilterMaskIdHigh = 0x0000;
        filter_config.FilterMaskIdLow = 0x0000;
        filter_config.FilterScale=CAN_FILTERSCALE_32BIT;
        filter_config.FilterActivation=ENABLE;
        filter_config.FilterBank = 0;
        filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
        filter_config.SlaveStartFilterBank = 14;

        HAL_CAN_ConfigFilter(hcan_, &filter_config);
    }

    /// @todo broadcast these over a can message
    uint32_t dropped_tx_frames_ = 0;
    uint32_t dropped_rx_frames_ = 0;
    uint32_t false_add_rx_msg_to_queue_ = 0;

    CAN_HandleTypeDef* hcan_;
    uint32_t tx_mailbox_addr_ = 0;

    /// @todo make this an etl lockable queue.
    shared::util::CircularQueue<shared::can::RawCanMsg, kMsgQueueLen>
        can_queue_{};
};

}  // namespace mcal::stm32f767::periph
