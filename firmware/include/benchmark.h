#pragma once

#include "scheduler.h"
#include "display.h"

class TimingBenchmark : public RelaxedTask<1000> {
 public:
    static void initialize() {

    }

    static void process() {
        float microseconds = float(*accu)/float(*count);
        *accu = 0;
        *count = 0;

        Display::BufferControl::show_number(microseconds);
    }

    static void setup(volatile uint32_t* a, volatile uint32_t* c) {
        accu = a;
    }

 private:
    static volatile uint32_t* accu;
    static volatile uint32_t* count;   
};

volatile uint32_t* TimingBenchmark::accu = nullptr;
volatile uint32_t* TimingBenchmark::count = nullptr;   