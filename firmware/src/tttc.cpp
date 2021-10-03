#include <Arduino.h>

#define BENCHMARK

#include "io_tools.h"
#include "hw_map.h"

#include "sound.h"
#include "display.h"
#include "ambient_light.h"
#include "timing.h"
#include "base_light.h"

#include "state_machine.h" // TODO: Incomplete

#include "scheduler.h"
#include "benchmark.h"

// TODO: Add temperature watchdog that shuts down the display in case of over temperature

// TODO: Add some EEPROM management base class for tasks

using sched = Scheduler<4000, 
  Display::ShiftPWMProcessor,
  BaseLightDimmer,
  //Display::SeparatorDot,
  //StateMachine,
  //RTCSync,
  //AmbientLight,
  //SoundGenerator,
  TimingBenchmark
>;

void setup() {    
  Pins::setup();  

  TimingBenchmark::setup(&sched::benchmark_micros_total, &sched::benchmark_counts);

  sched::initialize();

  SoundGenerator::set_tick_tock(TickTockSound::MonotonousClickLoud);
  //Display::BufferControl::show_number(1.234f);

  sched::start_critical();
  sched::start_relaxed();
}

void loop() {
  // Will never be called
  // TODO: use traditional main() function instead of this Arduino stuff...
}
