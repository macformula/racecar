#pragma once

#include "amk.hpp"

namespace amk {

enum class StarterState {
    OFF,
    STARTING,
    ERROR_DETECTED,
    RESET_ERROR,
    SUCCESS,
    FAILED
};

class Starter {
public:
    void Reset(void);
    Command Update(State state);

    // ---------- Accessors ----------
    bool HasErroredOut(void) const;
    bool Success(void) const;

private:
    static const uint32_t RETRY_ATTEMPTS = 5;
    uint32_t current_attempt = 0;

    bool on_enter;
    StarterState state;
};

}  // namespace amk