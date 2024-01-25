

TmsBroadcast CanAL::Unpack(uint8_t* data) {
    TmsBroadcast tms_broadcast;

    tms_broadcast.ThermModuleNum = (
                    (uint64_t)(data[5] & 00001111) << 6 |
                    (uint64_t)(data[6] & 11111100) >> 2 
    );

    return tms_broadcast;
}

void CanAL::Pack(TmsBroadcast tms_broadcast, uint8_t* packed_data) {
    // Do packing
    return;
}