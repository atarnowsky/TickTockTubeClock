#pragma once

#include "etl_fix.h"
#include <TimerOne.h>


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
            #ifdef BENCHMARK
                uint32_t now = micros();
            #endif
            
            auto step_critical = []<typename T>(const T&) constexpr {
                if constexpr (T::critical) {                    
                    uint8_t& task_state = state_critical[auto_index_of<T, Tasks...>()];
                    T::process(task_state++);
                    if(task_state >= T::cycle_count) {
                        task_state = 0;
                    }
                }
            };            

            (void)(step_critical(Tasks()), ...);

            #ifdef BENCHMARK
                benchmark_micros_total += (micros() - now);
                benchmark_counts++;
            #endif
        });   
    }

    // TODO: Allow to specify a return code for each process method
    // and implement some kind of global error display/handling
    static void start_relaxed() {
        // Now run main loop with non-critical tasks
        for(;;){            
            auto step_noncritical = []<typename T>(const T&) constexpr {
                if constexpr (!T::critical) {
                    uint32_t& task_time = state_relaxed[auto_index_of<T, Tasks...>()];
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


#ifdef BENCHMARK
    static volatile uint32_t benchmark_micros_total;
    static volatile uint32_t benchmark_counts;
#endif

 private:
    
    // Stores the internal "state" of each task.
    // For critical task: number of subsequent task calls, will be reset after 'cycle_count' has been reached
    // For relaxed task: hold the timestamp of the last call procession, so the task is only called ever 'minimal_delay' milliseconds
    static array<uint8_t, sizeof_critical<Tasks...>()> state_critical;
    static array<uint32_t, sizeof_relaxed<Tasks...>()> state_relaxed;
};

template<int UpdateRateHerz, typename... Tasks>
array<uint8_t, sizeof_critical<Tasks...>()> Scheduler<UpdateRateHerz, Tasks...>::state_critical = {};

template<int UpdateRateHerz, typename... Tasks>
array<uint32_t, sizeof_relaxed<Tasks...>()> Scheduler<UpdateRateHerz, Tasks...>::state_relaxed = {};



#ifdef BENCHMARK
    template<int UpdateRateHerz, typename... Tasks>
    volatile uint32_t Scheduler<UpdateRateHerz, Tasks...>::benchmark_micros_total = 0;

    template<int UpdateRateHerz, typename... Tasks>
    volatile uint32_t Scheduler<UpdateRateHerz, Tasks...>::benchmark_counts = 0;
#endif


template<unsigned char round_trip>
class CriticalTask{    
  public:
    static constexpr bool critical = true;
    static constexpr uint8_t cycle_count = round_trip;
};

template<unsigned long desired_delay_ms>
class RelaxedTask{
  public:
    static constexpr bool critical = false;
    static constexpr uint32_t minimal_delay = desired_delay_ms;
};