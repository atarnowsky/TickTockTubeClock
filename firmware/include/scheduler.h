#pragma once

// Someone forgot to include Atmega168P...
#define TIMER1_A_PIN   9
#define TIMER1_B_PIN   10
#define TIMER1_ICP_PIN 8
#define TIMER1_CLK_PIN 5
#include <TimerOne.h>

#include "etl_fix.h"

template<int UpdateRateHerz, typename... Tasks>
class Scheduler {
 public:    

    // TODO: Collect error codes from all initialize methods and return as list
    static void initialize() {
        // Initialize all tasks

        // Critical tasks are passed the update rate on initialization
        // non-critical ones do not need these, but are allowed to
        // specify a 'desired' update_rate
        auto initialize_conditional = []<typename T>(const T&) constexpr {
                if constexpr (T::critical) {
                    T::initialize(UpdateRateHerz);
                } else {
                    T::initialize();
                }
            };
        (void)(initialize_conditional(Tasks()), ...);
    }

    static void start_critical() {
        // Run critical tasks within ISR context
        Timer1.initialize(1'000'000 / UpdateRateHerz);
        Timer1.attachInterrupt([](){
            auto step_critical = []<typename T>(const T&) constexpr {
                if constexpr (T::critical) {
                    // TODO: Using an 32bit value here is pretty inefficient, since 8bit normally would do it
                    // This however, would make the state lookup way more complicated
                    // (would need some 'count_critical' and 'index_of_critical' functions)
                    uint32_t& task_state = state[index_of<T, Tasks...>()];
                    T::process(task_state++);
                    if(task_state >= T::cycle_count) {
                        task_state = 0;
                    }
                }
            };

            (void)(step_critical(Tasks()), ...);
        });   
    }

    // TODO: Allow to specify a return code for each process method
    // and implement some kind of global error display/handling
    static void start_relaxed() {
        // Now run main loop with non-critical tasks
        // TODO: incorporate update_rate
        for(;;){            
            auto step_noncritical = []<typename T>(const T&) constexpr {
                if constexpr (!T::critical) {
                    uint32_t& task_time = state[index_of<T, Tasks...>()];
                    uint32_t current_time = millis();                    
                    if(current_time - task_time >= T::minimal_delay) {
                        task_time = current_time;
                        T::process();
                    }                    
                }
            };

            (void)(step_noncritical(Tasks()), ...);
        }        
    }
    
    // TODO: Add on_error signal

 private:
    // Stores the internal "state" of each task.
    // Depending whether the task is critical or not, the corresponding integer
    // is interpreted differently:
    //  - critical task: number of subsequent task calls, will be reset after 'cycle_count' has been reached
    //  - non-critical task: hold the timestamp of the last call procession, so the task is only called ever 'minimal_delay' milliseconds
    static array<uint32_t, sizeof...(Tasks)> state;
};

template<int UpdateRateHerz, typename... Tasks>
array<uint32_t, sizeof...(Tasks)> Scheduler<UpdateRateHerz, Tasks...>::state = {};


template<unsigned int round_trip>
class CriticalTask{
  private:
    template<int UpdateRateHerz, typename... Tasks>
    friend class Scheduler;
    static constexpr bool critical = true;
    static constexpr uint16_t cycle_count = round_trip;
};

template<unsigned int desired_delay_ms>
class RelaxedTask{
  private:
    template<int UpdateRateHerz, typename... Tasks>
    friend class Scheduler;
    static constexpr bool critical = false;
    static constexpr uint16_t minimal_delay = desired_delay_ms;
};