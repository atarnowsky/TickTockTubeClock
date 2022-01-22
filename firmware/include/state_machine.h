#pragma once

#include "button_manager.h"

#include "scheduler.h"
#include "hw_map.h"
#include "etl_fix.h"

static constexpr unsigned long state_update_rate = 50;

template<typename... States>
class StateMachine : public RelaxedTask<state_update_rate> {
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