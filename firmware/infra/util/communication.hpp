/// @author Blake Freer
/// @date 2023-10-14

#ifndef UTIL_COMMUNICATION_HPP
#define UTIL_COMMUNICATION_HPP

#include "util/noncopyable.hpp"

namespace util {

/**
 * @brief Base class for all communication protocols (SPI, CAN, ...)
*/
class communication_base : private util::noncopyable{

};

}

#endif