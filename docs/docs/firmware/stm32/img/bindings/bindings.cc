// WARNING: The bindings.md article is heavily dependent on the exact line
// numbers. If you edit this file, you must update that document.

// ============================== CubeMX Includes ==============================
#include "main.h"
#include "stm32f7xx_hal.h"


extern "C" {
// This requires extern since it is not declared in a header, only defined in
// cubemx/../main.c
void SystemClock_Config();
}

// ============================= Firmware Includes =============================
#include "../../bindings.hpp"

// =============================== MCAL Namespace ==============================
namespace mcal {
using namespace mcal::periph::stm32f767;
// --------- Define C++ peripheral objects from CubeMX generated code ----------

}  // namespace mcal

// ============================ Bindings Namespace =============================
namespace bindings {
// ------------- Bind mcal peripherals to the bindings.hpp handles -------------

// ---------------- Implement other platform-specific functions ----------------
void Initialize() {
    HAL_Init();            // from stm32f7xx_hal.h
    SystemClock_Config();  // from cubemx/src/main.c

    // ------------------ Add additional initialization code -------------------
}

}  // namespace bindings
