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
      Display::AntiCathodePoisoning::enable();  
    }

    static void process() {
      static uint8_t cnt = 0;
      if(cnt == 0) {
        Display::AntiCathodePoisoning::disable();
        Display::BufferControl::show_number(1.234f);  
      }
      else if(cnt == 1)
        Display::AntiCathodePoisoning::enable();        
      
      cnt++;
      if(cnt > 1) cnt = 0;
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
  TimingBenchmark,
  Display::AntiCathodePoisoning,
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
