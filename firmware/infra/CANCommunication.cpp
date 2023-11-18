#include "communication.hpp"

namespace util {
    util::CANCommunication <CANMessage> CANCommunicationObject(hcan);

    CANCommunication::CANCommunication(CAN_HandleTypeDef* hcan) {
        //constructor implementation 
    }

    /*
    // maybe don't need since initialization is done in main.c
    void CANCommunication::Init() {
        if(HAL_CAN_Init(hcan) != HAL_OK) {
            //initialization failed
            Error_Handler();
        }
        //initalize transmission header
        txHeader.StdId = message->std_id;
        txHeader.RTR = CAN_RTR_DATA;
        txHeader.IDE = CAN_ID_STD;
        txHeader.DLC = message->dlc;
    }
    */

    void CANCommunication::Transmit(CANMessage& message, uint8_t size) {        
        // initialize Tx header
        CAN_TxHeaderTypeDef TxHeader;
        uint8_t TxData[8] = {0};
        uint32_t TxMailbox = 0;


        TxHeader.StdId = message.std_id;
        TxHeader.DLC = size; 
        

        // if mailbox isn't available, wait until it is
        while(HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

        //HAL_CAN_AddTxMessage: adds a message to the Tx mailboxes and activate the corresponding transmission request
        if(HAL_CAN_AddTxMessage(hcan, &TxHeader, message.data, &Tx_Mailbox) != HAL_OK) {
            // CAN message failed to send
            Error_Handler();
        }
        // CAN message sent
    }

    void CANCommunication::Recv(CAN_HandleTypeDef *hcan, uint32_t CAN_RX_FIFO) {
        CAN_RxHeaderTypeDef RxHeader;
        uint8_t RxData[8];
        
        if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO, &RxHeader, RxData) != HAL_OK) {
            // no message received
            Error_Handler();
            return;
        }
        // received data
        // create queue for received data to be stored
        CANMessage receivedMessage(RxHeader, RxData);
        receivedQueue.push(receivedMessage);
    } 
}
