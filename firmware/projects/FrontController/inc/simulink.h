/// @author Blake Freer
/// @date 2024-03-14

#pragma once

extern "C" {  // required since simulink generates c, not c++
#include "controller_autogen.h"
}

typedef ExtU SimulinkInput;
typedef ExtY SimulinkOutput;

/**
 * @brief Provides an interface to the simulink model.
 * @todo Can this become a shared component? Maybe templated off of the i/o
 * struct types. Such an implementation would also need the initialize() and
 * step() functions bound in the project, since they could not be referenced
 * without a project-specific include.
 * For now, since there is only one model, this non-shared implemenation is
 * fine.
 */
class ControlSystem {
public:
    static void Initialize() {
        controller_autogen_initialize();
    }

    static SimulinkOutput Update(SimulinkInput input) {
        rtU = input;
        controller_autogen_step();
        return rtY;
    }
};