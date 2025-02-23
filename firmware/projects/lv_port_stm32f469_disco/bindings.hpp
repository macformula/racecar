#include <cstdint>
namespace bindings {

extern void Initialize();

extern void DelayMS(uint32_t ms);

extern int readSelect();

extern int readScroll();

}  // namespace bindings