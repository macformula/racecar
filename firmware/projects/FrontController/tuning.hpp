namespace tuning {

/*
To measure, open BusMaster, load the veh.dbc, and read the `FC_apps_debug`
message. https://macformula.github.io/racecar/tutorials/busmaster/

Read the `raw_volt` signals when the pedal is released (pos_0) and fully
depressed (pos_100). Set these value accordingly.

To verify, recompile and flash FrontController and look at the same message.
The `apps1_pos` and `apps2_pos` signals should both be 0.0 when the pedal
is released and 100.0 when fully depressed. +- 0.5% is acceptable.
*/
const float apps1_volt_pos_0 = 2.781;  // aim for 2.8V
const float apps1_volt_pos_100 = 1.809;
const float apps2_volt_pos_0 = 0.536;  // aim for 0.55V
const float apps2_volt_pos_100 = 1.480;

}  // namespace tuning