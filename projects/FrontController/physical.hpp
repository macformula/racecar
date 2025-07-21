// PHYSICAL PARAMETERS
//
// Do not use them to change the handling, see tuning.hpp for that!

namespace tuning {

/*
To measure, open BusMaster, load the veh.dbc, and read the `AppsDebug`
message. https://macformula.github.io/racecar/tutorials/busmaster/

Read the `raw_volt` signals when the pedal is released (pos_0) and fully
depressed (pos_100). Set these value accordingly.

To verify, recompile and flash FrontController and look at the same message.
The `apps1_pos` and `apps2_pos` signals should both be 0.0 when the pedal
is released and 100.0 when fully depressed. +- 0.5% is acceptable.
*/
constexpr float apps1_volt_pos_0 = 2.781;  // aim for 2.8V
constexpr float apps1_volt_pos_100 = 1.809;
constexpr float apps2_volt_pos_0 = 0.544;  // aim for 0.55V
constexpr float apps2_volt_pos_100 = 1.480;

// See datasheets/race_grade/RG_SPEC-0030_M_APT_G2_DTM.pdf
// Needs to be tuned
constexpr float bpps_volt_pos_0 = 0.32;
constexpr float bpps_volt_pos_100 = 0.64;  // 250 PSI

// Steering angle is based off of straight position to guarantee correct
// straightaway behaviour. Assumes that `full_left` is symmetric about
// `straight` from `full_right`
// Needs to be tuned
constexpr float steer_volt_straight = 1.6;
constexpr float steer_volt_full_right = 2.5;

// Powertrain
constexpr float gearbox_ratio = 12.9;
constexpr float motor_diam_inch = 16;

}  // namespace tuning