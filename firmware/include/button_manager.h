#pragma once

#include "hw_map.h"
#include "io_tools.h"
#include "etl_fix.h"

class ButtonManager {
public:
    using button_callback_t = void (*)();
    static void configure(button_callback_t timeout_reset);
    static void update();

    template<typename State>
    static void set_callbacks() {
        callbacks_short = {
            State::on_plus_short,
            State::on_minus_short,
            State::on_select_short 
        };

        callbacks_long = {
            State::on_plus_long,
            State::on_minus_long,
            State::on_select_long 
        };

        callback_reset = State::on_select_reset;
    }

private:
    static array<button_callback_t, 3> callbacks_short;
    static array<button_callback_t, 3> callbacks_long;
    static button_callback_t callback_reset;
    static button_callback_t callback_always;
    static uint32_t timer;
    static uint8_t current;
};