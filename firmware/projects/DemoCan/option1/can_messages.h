/// @author Samuel Parent
/// @date 2023-01-17



class tms_broadcast : public shared::comms::can::CanMsg {
private:
    void Pack(uint8_t* data) {

    }

   void Unpack(uint8_t* data) {

    }
public:
    uint8_t ThermModuleNum;
    int8_t LowThermValue;
    int8_t HighThermValue;
    int8_t AvgThermValue;
    uint8_t HighThermId;
    uint8_t LowThermId;
    uint8_t Checksum;

    friend class BusManager;
};

struct status_indicator{
    uint8_t ImdStatus;
    uint8_t BspdStatus;
    uint8_t BppsStatus;
};


