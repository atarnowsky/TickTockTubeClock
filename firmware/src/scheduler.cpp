#include "scheduler.h"
#include "hw_map.h"

uint32_t current_time = 0;//millis();

uint32_t time_millis() {
    return current_time;
}

void time_update() {
    current_time = 0;//millis();
}

void pause_critical() {            
    Timer1.stop();
    IO::low(Pins::Anode::MuxA, Pins::Anode::MuxB);
}

void resume_critical() {        
    Timer1.start();
}