#pragma once


template<uint8_t timeout_seconds = 0>
class State {
 public:
    static constexpr uint32_t timeout = uint32_t(timeout_seconds) * 1000;

    // Will be called exactly once when the State gets active
    static void initialize() {}

    // Will be called periodically as long as the active state stays
    // the same. May trigger spontaneous State transistions.
    static void process() {}

    // May be used to write to EEPROM.
    // May NOT be used to alter StateMachine!
    static void finish() {}

    // Event handlers - default to no-op
    static void on_plus_short() {}
    static void on_plus_long() {}
    static void on_minus_short() {}
    static void on_minus_long() {}
    static void on_select_short() {}
    static void on_select_long() {}

    // Mainly used for the "reset to factory defaults" task
    static void on_select_reset() {}

    // Will be called once when no buttons have been
    // pressed for the specified timeout
    static void on_timeout() {}
};