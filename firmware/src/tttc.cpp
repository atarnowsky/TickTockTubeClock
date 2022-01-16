//#define BENCHMARK
#include <Arduino.h>

#include "io_tools.h"
#include "hw_map.h"

#include "sound.h"
#include "display.h"
#include "ambient_light.h"
#include "timing.h"
#include "base_light.h"

#include "state.h"
#include "state_machine.h"  
#include "scheduler.h"
//#include "benchmark.h"
#include "effects.h"
#include "settings.h"

// Features that will be implemented in future releases:
//  - Allow Display to control multiplexing (Alternating, AOnly, BOnly, Off)
//    (needed for cathode poisoning prevention)
//  - Add blink feature to BufferControl
//  - Better ghosting prevention
//  - Temperature watchdog that shuts down the display in case of over temperature
//  - Cathode poisoning prevention with customizable time(-span)
//  - Various display-change effects

class Clock;
class TimeSet;
class SettingInit;
class SettingLEDBrightness;
class SettingTubeBrightness;

using UI = StateMachine<
  Clock,       // Index 0 automatically denotes the initial State to run
  TimeSet,
  SettingInit,
  SettingLEDBrightness,
  SettingTubeBrightness
>;

// Yes, its pretty ugly to use UI within the following header files.
// This seriously needs some cleanup...
#include "state_clock.h"
#include "state_timeset.h"
#include "state_settings.h"



using sched = Scheduler<8000, 
  Display::ShiftPWMProcessor,
  Display::SeparatorDot,
  Display::AntiCathodePoisoning,
  Effects::Transition,
  Effects::Ambient,
  BaseLightDimmer,    
  RTCSync,
  AmbientLight,
  SoundGenerator,
  UI
  //TimingBenchmark  
>;


void setup() {    
  Pins::setup();  

  // SoundGenerator::play_sequence(
  //     Note{659, 100, 75*3},
  //     Note{523, 100, 75},
  //     Note{659, 100, 75},
  //     Note{880, 200, 75}
  // );
  
  BaseLightDimmer::set_fade_speed(2);
  BaseLightDimmer::set_brightness(0);

  //TimingBenchmark::setup(&sched::benchmark_micros_total, &sched::benchmark_counts);

  sched::initialize();
  
  //SoundGenerator::set_tick_tock(TickTockSound::ClickSilent);
  //Display::BufferControl::show_number(1.234f);


  sched::start_critical();
  sched::start_relaxed();
}

void loop() {
  // Will never be called
  // TODO: use traditional main() function instead of this Arduino stuff...
}
