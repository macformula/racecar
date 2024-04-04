/// @author Clara Wong
/// @date 2024-04-02

#ifndef MCAL_STM32F767_PERIPH_CAN_H_
#define MCAL_STM32F767_PERIPH_CAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <queue>
#include <thread>

#include "shared/comms/can/raw_can_msg.h"
#include "shared/periph/can.h"
#include "stm32f7xx_hal.h"

namespace mcal::periph {

class CanBase : public shared::periph::CanBase {
private:
    using RawCanMsg = shared::comms::can::RawCanMsg;

    CAN_HandleTypeDef hcan1;
    std::queue<RawCanMsg> can_queue;

public:
    CanBase() {
        hcan1.Instance = CAN1;
    }

    void Setup() {
        hcan1.Instance = CAN1;
        hcan1.Init.Prescaler = 6;
        hcan1.Init.Mode = CAN_MODE_LOOPBACK;
        hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
        hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
        hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
        hcan1.Init.TimeTriggeredMode = DISABLE;
        hcan1.Init.AutoBusOff = ENABLE;
        hcan1.Init.AutoWakeUp = DISABLE;
        hcan1.Init.AutoRetransmission = DISABLE;
        hcan1.Init.ReceiveFifoLocked = DISABLE;
        hcan1.Init.TransmitFifoPriority = DISABLE;
        if (HAL_CAN_Init(&hcan1) != HAL_OK) {
            Error_Handler();
        }
    }

    void Send(const shared::comms::can::RawCanMsg& can_tx_msg) {
        CAN_TxHeaderTypeDef TxHeader;
        TxHeader.StdId = can_tx_msg.header.id;
        TxHeader.ExtId = 0;
        TxHeader.IDE = CAN_ID_STD;
        TxHeader.RTR = CAN_RTR_DATA;
        TxHeader.DLC = can_tx_msg.header.data_len;

        uint32_t TxMailbox;
        if (HAL_CAN_AddTxMesssage(&hcan1, &TxHeader, can_tx_msg.data,
                                  &TxMailbox) != HAL_OK) {
            Error_Handler();
        }
    }

    void Read(shared::comms::can::RawCanMsg can_rx_msgs[], size_t len) {
        CAN_RxHeaderTypeDef RxHeader;
        uint8_t data[8];

        // Read the message
        if (HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFOO, &RxHeader, data) ==
            HAL_OK) {
            RawCanMsg rawMsg;
            rawMsg.header.id = RxHeader.StdId;
            rawMsg.header.len = RxHeader.DLC;
            std::copy(data, data + RxHeader.DLC, rawMsg.data);

            // Add to the queue if there's space
            if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFOO) > 0) {
                if (can_queue.size() < len) {
                    can_queue.push(rawMsg);
                    else {
                        can_queue.pop();
                    }
                }
            }
        }
    }
}

}  // namespace mcal::periph

#endif