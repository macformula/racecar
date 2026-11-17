package main

// Telemetry is sent to InfluxDB for Grafana dashboarding

import (
	"fmt"
	"math"
	"strconv"
	"strings"

	"github.com/macformula/hil/canlink"
)

// Extended IDs use the 29-bit frame ID without the DBC bit-31 flag.
var targetIDs = map[uint32]bool{
	202:       true, // FcAlerts
	211:       true, // LvStatus
	212:       true, // LvAlerts
	214:       true, // LvDcdc
	230:       true, // DashCommand
	300:       true, // Accumulator_Soc
	310:       true, // SuspensionTravel34
	401:       true, // AppsDebug
	402:       true, // BppsSteerDebug
	643:       true, // Inv1_ActualValues1
	644:       true, // Inv2_ActualValues1
	645:       true, // Inv1_ActualValues2
	646:       true, // Inv2_ActualValues2
	773:       true, // GnssAttitude
	777:       true, // GnssImu
	1572:      true, // Pack_State
	1573:      true, // Pack_SOC
	406451072: true, // BmsBroadcast
	419361278: true, // ThermistorBroadcast
}

// Signal names written to InfluxDB must match DBC signal names exactly.
var importantSignals = map[string]bool{
	"ActualVelocity":    true, // motor RPM
	"Apps1Percent":      true, // APPS sensor 1 position (%)
	"Apps2Percent":      true, // APPS sensor 2 position (%)
	"AvgThermValue":     true, // segment avg temp (°C)
	"BppsPercent":       true, // brake pedal position (%)
	"BusCurrent":        true, // DCDC bus current (A)
	"BusVoltage":        true, // DCDC bus voltage (V)
	"HighThermValue":    true, // segment max temp (°C)
	"LowThermValue":     true, // segment min temp (°C)
	"LvBatteryVoltage":  true, // LV battery voltage (V)
	"Pack_Current":      true, // HV pack current (A)
	"Pack_Inst_Voltage": true, // HV battery voltage (V)
	"Pack_SOC":          true, // state of charge (%)
	"Speed":             true, // vehicle speed (mph)
	"TempIGBT":          true, // IGBT temperature (°C)
	"TempInverter":      true, // inverter temperature (°C)
	"TempMotor":         true, // motor temperature (°C)
}

type faultInfo struct{ system, description, severity string }

// Boolean fault signals from FcAlerts (202) and LvAlerts (212)
var faultSignals = map[string]faultInfo{
	"AccumulatorContactorWrongState": {"BMS", "Contactor Wrong State", "CRITICAL"},
	"AccumulatorLowSoc":              {"BMS", "Low State of Charge", "WARNING"},
	"AppsImplausible":                {"APPS", "Implausible Pedal Signal", "CRITICAL"},
	"BmsFault":                       {"BMS", "BMS Fault", "CRITICAL"},
	"CanTxError":                     {"CAN", "TX Error", "WARNING"},
	"DashboardBootTimeout":           {"Dashboard", "Boot Timeout", "WARNING"},
	"EV47Active":                     {"Safety", "EV4.7 Rule Active", "CRITICAL"},
	"HsdOvercurrent":                 {"LV", "HSD Overcurrent", "CRITICAL"},
	"ImdFault":                       {"IMD", "Isolation Fault", "CRITICAL"},
	"LeftMotorRunningError":          {"Motor", "Left Motor Running Error", "CRITICAL"},
	"LeftMotorStartingError":         {"Motor", "Left Motor Start Error", "CRITICAL"},
	"MotorRetriesExceeded":           {"Motor", "Retries Exceeded", "CRITICAL"},
	"NoInv1Can":                      {"Inverter", "No INV1 CAN Comm", "CRITICAL"},
	"NoInv2Can":                      {"Inverter", "No INV2 CAN Comm", "CRITICAL"},
	"RightMotorRunningError":         {"Motor", "Right Motor Running Error", "CRITICAL"},
	"RightMotorStartingError":        {"Motor", "Right Motor Start Error", "CRITICAL"},
}

type signalThreshold struct {
	low, high float64
	fault     faultInfo
}

// Values outside these limits produce faults
var signalThresholds = map[string]signalThreshold{
	"HighThermValue":    {math.Inf(-1), 60.0, faultInfo{"BMS", "Battery Segment Overtemperature", "CRITICAL"}},
	"LvBatteryVoltage":  {18.0, 29.0, faultInfo{"LV", "LV Battery Voltage Out of Range", "CRITICAL"}},
	"Pack_Current":      {math.Inf(-1), 59.0, faultInfo{"BMS", "Pack Overcurrent", "CRITICAL"}}, // uses absolute value
	"Pack_Inst_Voltage": {360.0, 600.0, faultInfo{"BMS", "Pack Voltage Out of Range", "CRITICAL"}},
	"TempIGBT":          {math.Inf(-1), 60.0, faultInfo{"Inverter", "IGBT Overtemperature", "CRITICAL"}},
	"TempInverter":      {math.Inf(-1), 60.0, faultInfo{"Inverter", "Inverter Overtemperature", "CRITICAL"}},
	"TempMotor":         {math.Inf(-1), 70.0, faultInfo{"Motor", "Motor Overtemperature", "CRITICAL"}},
}

var tagEscaper = strings.NewReplacer("\\", "\\\\", ",", "\\,", "=", "\\=", " ", "\\ ")

// telemetryLines formats InfluxDB lines with tags for inverters and battery modules.
func telemetryLines(frame canlink.TimestampedFrame, signals []decodedSignal) string {
	timestamp := frame.Time.UnixNano()
	tags := ""
	// Shared signal names need tags to distinguish inverters and battery modules
	switch frame.Frame.ID {
	case 643, 645:
		tags = ",inverter=inv1"
	case 644, 646:
		tags = ",inverter=inv2"
	case 406451072:
		for _, signal := range signals {
			if signal.name == "ThermModuleNum" {
				tags = ",module=" + strconv.Itoa(int(signal.value))
			}
		}
	}
	// Keep measurement names, field types, and fault tags compatible with Grafana
	var lines []string
	fault := func(info faultInfo) {
		lines = append(lines, fmt.Sprintf("faults,system=%s,fault=%s,severity=%s active=1i %d", tagEscaper.Replace(info.system), tagEscaper.Replace(info.description), tagEscaper.Replace(info.severity), timestamp))
	}
	for _, signal := range signals {
		// Faults are active events; cleared states are not recorded.
		if info, ok := faultSignals[signal.name]; ok && signal.value == 1 {
			fault(info)
		}
		// GnssSpeed also has a Speed signal, but it uses m/s instead of mph.
		if signal.name == "Speed" && frame.Frame.ID != 230 {
			continue
		}
		if !importantSignals[signal.name] || math.IsNaN(signal.value) || math.IsInf(signal.value, 0) {
			continue
		}
		number := strconv.FormatFloat(signal.value, 'g', -1, 64)
		lines = append(lines, fmt.Sprintf("car_data,signal=%s%s value=%s %d", tagEscaper.Replace(signal.name), tags, number, timestamp))
		// Preserve the LV signal name queried by the unchanged Grafana export
		if signal.name == "LvBatteryVoltage" {
			lines = append(lines, fmt.Sprintf("car_data,signal=lv_battery_voltage%s value=%s %d", tags, number, timestamp))
		}
		if threshold, ok := signalThresholds[signal.name]; ok {
			value := signal.value
			// Apply the current limit to both charging and discharging readings
			if signal.name == "Pack_Current" {
				value = math.Abs(value)
			}
			if value < threshold.low || value > threshold.high {
				fault(threshold.fault)
			}
		}
	}
	return strings.Join(lines, "\n")
}
