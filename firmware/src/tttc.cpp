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
#include "effects.h"

// Features that will be implemented in future releases:
//  - Allow Display to control multiplexing (Alternating, AOnly, BOnly, Off)
//    (needed for cathode poisoning prevention)
//  - Add blink feature to BufferControl
//  - Display dimming
//  - Better ghosting prevention
//  - Temperature watchdog that shuts down the display in case of over temperature
//  - EEPROM management
//  - Cathode poisoning prevention with customizable time(-span)
//  - Various display-change effects



class Demo : public RelaxedTask<5000> {
  public:
    static void initialize() {
      Display::ShiftPWMProcessor::set_brightness(255);
      Effects::Transition::set_effect(Effects::NumberTransition::FLICKER, 2500);
    }

    static void process() {
      Effects::Transition::display(fastrand() * 9999);
    }
};

using sched = Scheduler<8000, 
  Display::ShiftPWMProcessor,
  BaseLightDimmer,
  //Display::SeparatorDot,
  //StateMachine,
  //RTCSync,
  //AmbientLight,
  SoundGenerator,
  //TimingBenchmark,
  //Display::AntiCathodePoisoning,
  Effects::Transition,
  Demo
>;

void setup() {    
  Pins::setup();  

  TimingBenchmark::setup(&sched::benchmark_micros_total, &sched::benchmark_counts);

  sched::initialize();

  SoundGenerator::set_tick_tock(TickTockSound::ClickSilent);
  //Display::BufferControl::show_number(1.234f);

  BaseLightDimmer::set_fade_speed(2);
  BaseLightDimmer::set_fade_target(64);

  sched::start_critical();
  sched::start_relaxed();
}

void loop() {
  // Will never be called
  // TODO: use traditional main() function instead of this Arduino stuff...
}
