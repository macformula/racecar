/// @author Blake Freer
/// @date 2024-03-14

#pragma once

#include "controller_autogen.h"

typedef controller_autogen::ExtU SimulinkInput;
typedef controller_autogen::ExtY SimulinkOutput;

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
    ControlSystem() : controller_autogen_(controller_autogen{}) {}
    void Initialize() {
        controller_autogen_.initialize();
    }

    SimulinkOutput Update(SimulinkInput* input) {
        controller_autogen_.setExternalInputs(input);
        controller_autogen_.step();
        return controller_autogen_.getExternalOutputs();
    }

private:
    controller_autogen controller_autogen_;
};