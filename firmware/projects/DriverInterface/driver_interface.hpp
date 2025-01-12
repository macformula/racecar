#pragma once

enum class DiCmd {
    // fill this in (see src/enums)
};

enum class DiSts {
    // fill in
};

// The simulink block has multiple inputs and outputs. We use `struct` to bind
// them all together. Define the input and output structs

struct DiInput {
    DiCmd di_cmd;
    float brake_pedal_position;
    // fill out the rest
};

struct DiOutput {
    bool brake_light_enable;
    // fill this out
};

class DriverInterface {
public:
    // Do not change the public interface
    DiOutput Update(DiInput input, int time_ms);

private:
    // Add private variables if necessary. Prefer local variable in `Update()`
};