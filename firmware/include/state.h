#pragma once


template<uint8_t timeout_seconds = 0>
class State {
 public:
    static constexpr uint32_t timeout = uint32_t(timeout_seconds) * 1000;

    // Will be called exactly once when the State gets active
    inline static void initialize() {}

    // Will be called periodically as long as the active state stays
    // the same. May trigger spontaneous State transistions.
    inline static void process() {}

    // May be used to write to EEPROM.
    // May NOT be used to alter StateMachine!
    inline static void finish() {}

    // Event handlers - default to no-op
    inline static void on_plus_short() {}
    inline static void on_plus_long() {}
    inline static void on_minus_short() {}
    inline static void on_minus_long() {}
    inline static void on_select_short() {}
    inline static void on_select_long() {}

    // Mainly used for the "reset to factory defaults" task
    inline static void on_select_reset() {}

    // Will be called once when no buttons have been
    // pressed for the specified timeout
    inline static void on_timeout() {}
};