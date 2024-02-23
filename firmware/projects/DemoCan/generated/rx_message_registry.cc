/// @author Samuel Parent
/// @date 2023-02-23

#include "etl/unordered_map.h"

#include "generated/can_messages.h"

namespace generated::can {

etl::unordered_map rx_message_registry{
    etl::pair{ 0x645, DebugLedOverride{} },
};

} // namespace generated::can