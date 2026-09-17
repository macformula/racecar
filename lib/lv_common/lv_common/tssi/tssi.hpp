#pragma once

/// @brief Tractive System Active Indicator
/// A light on the car that indicates if the car can drive or is faulted.
namespace tssi {

void SetEnabled(bool enable);

bool GetImdFault(void);
bool GetBmsFault(void);

void task_10hz(void);

}  // namespace tssi
