#pragma once

#include "button_manager.h"

#include "scheduler.h"
#include "hw_map.h"
#include "etl_fix.h"


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

template<typename... States>
class StateMachine : public RelaxedTask<50> {
 public:    
    static void initialize() {    
        ButtonManager::configure([](){
            timer = millis();
        });

        current_state = 0;
        next_state = 0;
        timer = millis();

        // Always initialize first task
        call_nth<States...>(0, [](auto T){
            using State = decltype(T);
            ButtonManager::set_callbacks<State>();
            State::initialize();
        });
    }

    static void process() {
        ButtonManager::update();
        
        // Select and initialize next task, if change occured
        if(current_state != next_state) {
            call_nth<States...>(current_state, [](auto T){
                decltype(T)::finish();
            });

            // Reset timeout
            timer = millis();

            current_state = next_state;

            call_nth<States...>(current_state, [](auto T){
                using State = decltype(T);
                ButtonManager::set_callbacks<State>();
                State::initialize();
            });
        }

        // Process current state
        call_nth<States...>(current_state, [](auto T){
            decltype(T)::process();
        });

        // Check for timeout
        if(timer > 0) {
            uint32_t duration = millis() - timer;
            call_nth<States...>(current_state, [&](auto T){
                using State = decltype(T);
                if(duration >= State::timeout) {
                    State::on_timeout();
                    timer = 0; // Only trigger once
                }
            });
        }
    }

    template<class STATE>
    static void next() {
        static constexpr bool valid {(std::is_same<STATE, States>::value || ...)};
        static_assert(valid, "State does not belong to current Statemachine configuration.");

        next_state = uint8_t(index_of<STATE, States...>());
    }

 private: 
    static uint8_t current_state;
    static uint8_t next_state;
    static uint32_t timer;
};

template<typename... States>
uint8_t StateMachine<States...>::current_state(0);

template<typename... States>
uint8_t StateMachine<States...>::next_state(0);

template<typename... States>
uint32_t StateMachine<States...>::timer(millis());