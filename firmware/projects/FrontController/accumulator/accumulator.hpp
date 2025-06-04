#pragma once

#include <optional>

#include "../enums.hpp"

struct ContactorFeedback {
    enum class State : bool {
        // WARNING: feedback is inverted relative to command
        IS_OPEN = true,
        IS_CLOSED = false,
    };
    State precharge;
    State negative;
    State positive;
};

struct ContactorCommand {
    enum class State {
        OPEN = false,
        CLOSE = true,
    };
    State precharge;
    State positive;
    State negative;
};

class Accumulator {
public:
    struct Input {
        AccCmd cmd;
        ContactorFeedback feedback;
        float pack_soc;
    };
    struct Output {
        AccSts status;
        ContactorCommand command;
    };

    Accumulator();
    Output Update(const Input& input, int time_ms);

private:
    std::optional<AccSts> TransitionStatus(const Input& input, int time_ms);
    ContactorCommand SelectContactorCmd(AccSts status);

    // State machine variables (BmUpdate)
    std::optional<AccSts> current_status_;
    int status_snapshot_time_ms_;
};
