#pragma once

#include <concepts>
#include <cstdint>

namespace amk {

template <typename MsgType>
concept ActualValues1 = requires(MsgType msg) {
    { msg.bSystemReady() } -> std::same_as<bool>;
    { msg.bError() } -> std::same_as<bool>;
    { msg.bWarn() } -> std::same_as<bool>;
    { msg.bQuitDcOn() } -> std::same_as<bool>;
    { msg.bDcOn() } -> std::same_as<bool>;
    { msg.bQuitInverterOn() } -> std::same_as<bool>;
    { msg.bInverterOn() } -> std::same_as<bool>;
    { msg.bDerating() } -> std::same_as<bool>;
    { msg.ActualVelocity() } -> std::same_as<int16_t>;
    { msg.TorqueCurrent() } -> std::same_as<int16_t>;
    { msg.MagnetizingCurrent() } -> std::same_as<int16_t>;
};

template <typename MsgType>
concept ActualValues2 = requires(MsgType msg) {
    { msg.TempMotor() } -> std::same_as<float>;
    { msg.TempInverter() } -> std::same_as<float>;
    { msg.ErrorInfo() } -> std::same_as<uint16_t>;
    { msg.TempIGBT() } -> std::same_as<float>;
};

template <typename MsgType>
concept Setpoints1 = requires(MsgType msg) {
    { msg.b_inverter_on } -> std::convertible_to<bool>;
    { msg.b_dc_on } -> std::convertible_to<bool>;
    { msg.b_enable } -> std::convertible_to<bool>;
    { msg.b_error_reset } -> std::convertible_to<bool>;
    { msg.target_velocity } -> std::convertible_to<int16_t>;
    { msg.torque_limit_positiv } -> std::convertible_to<int16_t>;
    { msg.torque_limit_negativ } -> std::convertible_to<int16_t>;
};

enum class State {
    OFF,
    SYSTEM_READY,
    STARTUP_bDCON,
    STARTUP_bENABLE,
    STARTUP_bINVERTER,
    STARTUP_X140,
    RUNNING,
    ERROR,
    ERROR_RESET,
};

enum class Command {
    OFF,
    ENABLED,
    ERROR_RESET
};

struct Request {
    float speed_request;
    float torque_limit_positive;
    float torque_limit_negative;
};

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
class Amk {
public:
    Amk(void);

    // ---------- Accessors ----------
    SP GetSetpoints(void) const;
    bool HasError(void) const;
    uint16_t GetErrorCode(void) const;
    bool IsReadyForInverter(void) const;
    bool IsRunning(void) const;
    State GetState(void) const;

    // ---------- Modifiers ----------
    void SetRequest(Request request);
    void SetCommand(Command command);

    // Used to synchronize the two motors which share the X140 signal
    void ProceedFromInverter(void);

    // ---------- Behaviour ----------
    void Init(void);
    void Update_100Hz(AV1 av1, AV2 av2);

private:
    State state;
    Command command;
    Request request;

    uint32_t elapsed;
    bool inverter_is_set_flag;

    uint16_t error_code;
    bool ready_for_inverter;
    SP setpoints;
};

}  // namespace amk

#include "amk.tpp"
