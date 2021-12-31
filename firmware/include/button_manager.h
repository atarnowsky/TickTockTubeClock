#pragma once

#include "hw_map.h"
#include "io_tools.h"
#include "etl_fix.h"

class ButtonManager {
public:
    typedef void (*button_callback_t)(); 

    static void configure(button_callback_t timeout_reset) {        
        // Enable pullups
        // Mandatory, since there is not enough space for external pullups
        IO::configure<INPUT_PULLUP>(Pins::Buttons);
        callback_always = timeout_reset;
        current = 255;
        timer = millis();
    }

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

    static void update() {
        // No button pressed, yet
        if(current == 255) {
            for(uint8_t i = 0; i < 3; i++) {
                if(digitalRead(Pins::Buttons[i]) == LOW) {
                    current = i;
                    timer = millis();
                    break;
                }
            }
            return;
        }

        // Long pressed finished, wait till no buttons are pressed anymore
        if(current == 254) {
            bool clear = true;
            for(uint8_t i = 0; i < 3; i++) {
                if(digitalRead(Pins::Buttons[i]) == LOW) {
                    clear = false;
                    break;
                }
            }
            if(clear) {
                current = 255;
            }
            return;
        }
    
        if(digitalRead(Pins::Buttons[current]) == HIGH) {
            uint32_t elapsed = millis() - timer;

            if((current == 2) && (elapsed > Timings::button_reset_press)) {
                callback_reset();
            } else {
                if(elapsed > Timings::button_long_press) {
                    callbacks_long[current]();
                } else {
                    callbacks_short[current]();
                }
            }

            callback_always();
            current = 255;
        } else {
            uint32_t elapsed = millis() - timer;
            if((current == 2) && (elapsed > Timings::button_reset_press)) {
                callback_reset();
                callback_always();
                current = 254;
            } else {
                if(elapsed > Timings::button_long_press) {
                    callbacks_long[current]();
                    callback_always();
                    current = 254;
                } 
            }
        }
    }

private:
    static array<button_callback_t, 3> callbacks_short;
    static array<button_callback_t, 3> callbacks_long;
    static button_callback_t callback_reset;
    static button_callback_t callback_always;
    static uint32_t timer;
    static uint8_t current;
};

array<ButtonManager::button_callback_t, 3> ButtonManager::callbacks_short{nullptr, nullptr, nullptr};
array<ButtonManager::button_callback_t, 3> ButtonManager::callbacks_long{nullptr, nullptr, nullptr};
ButtonManager::button_callback_t ButtonManager::callback_reset{nullptr};
ButtonManager::button_callback_t ButtonManager::callback_always{nullptr};
uint32_t ButtonManager::timer{0};
uint8_t ButtonManager::current{255};