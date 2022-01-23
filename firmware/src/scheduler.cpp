#include "scheduler.h"

uint32_t current_time = 0;//millis();

uint32_t time_millis() {
    return current_time;
}

void time_update() {
    current_time = 0;//millis();
}