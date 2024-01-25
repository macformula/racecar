/// @author Samuel Parent
/// @date 2023-01-23

#include "can_messages.h"

class BusManager {
private:
    const int MAX_MSG_Q_LEN = 100;
    // Rx messages
    tms_broadcast tms_broadcast_;
    status_indicator status_indicator_;

    status_indicator Unpack(uint8_t* data);
    
    raw_can_msg Pack(const tms_broadcast& tms_broadcast);

    std::map<

public:
    void Send(const tms_broadcast& msg) {
        raw_can_msg raw_msg;

        raw_msg = Pack(msg);
        CanBase.Send(raw_msg);
    }

    void Read(status_indicator& msg) {
        msg = status_indicator_;
    }

    void Update() {
        raw_can_msg message_queue[MAX_MSG_Q_LEN];

        can_base.ReadQueue(message_queue);

        for(const auto raw_msg : message_queue) {
            
        }
    }
}