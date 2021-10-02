#pragma once

#include <Arduino.h>
#include <JC_Button.h>

#include "scheduler.h"
#include "hw_map.h"

class StateMachine : public RelaxedTask<100> {
 public:
    static void initialize() {       
        button_plus.begin();
        button_minus.begin();
        button_select.begin();
    }

    static void process() {
        button_plus.read();
        button_minus.read();
        button_select.read();
        
        // if(button_select.pressedFor(500) && !long_plus) {
        //     long_plus = true;
        //     led_state = !led_state;
        //     buzzer.ack(2);
        //     if(led_state) {
        //     //display.led_on();
        //         buzzer.set_tick_tock(TickTockSound::BitoneClick);
        //     } else {
        //         //display.led_off();
        //         buzzer.set_tick_tock(TickTockSound::None);
        //     }
        // }

        // if(button_select.wasReleased()) {
        //     long_plus = false;
        // }

        // if(button_select.wasPressed()) {
        //     current_state++;
        //     current_state = current_state % states;  
        //     buzzer.ack();
        //     wait_timer = 0;
        // }

        // if(button_minus.wasPressed() && current_state == Time) {
        //     increment_minutes();
        //     buzzer.ack();
        // }

        // if(button_plus.wasPressed() && current_state == Time) {
        //     increment_hours();
        //     buzzer.ack();
        // }

        // switch (current_state) {
        //     case Time:
        //         if(wait_timer == 0)
        //             Display::BufferControl::show_time(current_time());
        //     break;

        //     case Temperature: {
        //         if(wait_timer == 0){
        //             float temperature = RTC.temperature() / 4.0f;
        //             Display::BufferControl::show_number(temperature);
        //         }
        //     }    
        //     break;

        //     case LightMeter: {
        //         if(wait_timer == 0) {
        //             OPT3001 result = opt3001.readResult();  
        //             Display::BufferControl::show_number(result.lux);
        //         }
        //     }
        //     break;

        //     default:
        //         Display::BufferControl::clear();
        //     break;
        // }
    }

 private: 
    static Button button_plus;
    static Button button_minus;
    static Button button_select;
};

Button StateMachine::button_plus(Pins::Buttons[0]);
Button StateMachine::button_minus(Pins::Buttons[1]);
Button StateMachine::button_select(Pins::Buttons[2]);