#include "util_communication_can.hpp"

namespace util {
    CANCommunication::CANCommunication(CAN_HandleTypeDef* hcan) : hcan(hcan) {
        //constructor implementation 
    }
    
    bool CANCommunication::Init() {
        if(HAL_CAN_Init(hcan) != HAL_OK) {
            //initialization failed
            return false;
        }
        //can add additional initialization code for CAN configuration
        return true;
    }

    bool CANCommunication::Send(const uint9_t byte_to_send) noexcept {
        //send CAN message
        CAN_TxHeaderTypeDef txHeader;
        txHeader.StdId = 0x123;
        txHeader.RTR = CAN_RTR_DATA;
        txHeader.IDE = CAN_ID_STD;
        txHeader.DLC = 1; // data length = 1 byte

        //HAL_CAN_AddTxMessage: adds a message to the Tx mailboxes and activate the corresponding transmission request
        if(HAL_CAN_AddTxMessage(hcan, &txHeader, &byte_to_send, CAN_Tx_Mailboxes) != HAL_OK) {
            // CAN message failed to send
            return false;
        }
        // CAN message sent
        return true;
    }

    bool CANCommuncation::Recv(uint8_t& byte_to_recv) {
        //receive CAN message
        CAN_RxHeaderTypeDef rxHeader;
        if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, &byte_to_recv) != HAL_OK) {
            // no message received
            return false;
        }
        //received data
        return true;
    }

    void CANCommunication::Select() {

    }

    void CANCommunication::Deselect() {

    }
}