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
#include "settings.h"

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


class Demo : public RelaxedTask<4000> {
  public:
    static void initialize() {
      Display::ShiftPWMProcessor::set_brightness(255);
      Effects::Transition::set_effect(Effects::NumberTransition::FLICKER, 9);
      Effects::Ambient::set_effect(Effects::AmbientEffect::DEFECTIVE);
      Effects::Transition::display(fastrand(9999));
    }

    static void process() {
      //static uint16_t cnt = 0;
      Effects::Transition::display(fastrand(9999));
      //Effects::Transition::display(cnt++);
    }
};


class Clock;
class TimeSet;

using UI = StateMachine<
  Clock,       // Index 0 automatically denotes the initial State to run
  TimeSet
>;

class Clock : public State<50>{
public:
  static uint16_t cnt;
  static void on_plus_short() {
    SoundGenerator::ack_short();
    cnt += 1;
  }
  static void on_plus_long() {
    SoundGenerator::ack_long();
    cnt += 100;
  }

  static void on_select_reset() {
    SoundGenerator::ack_reset();
    cnt = 0;
  }

  static void on_select_short() {
    UI::next<TimeSet>();
  }

  static void initialize() {
    cnt = 1234;
    Display::ShiftPWMProcessor::set_brightness(255);
    Effects::Transition::set_effect(Effects::NumberTransition::FLICKER, 9);
    Effects::Ambient::set_effect(Effects::AmbientEffect::DEFECTIVE);      
  }

  static void process() {
    Effects::Transition::display(cnt);
  }

  static void on_timeout() {
    cnt = 8888;
  }

};

uint16_t Clock::cnt{0};

class TimeSet : public State<0>{
public:
  static uint16_t cnt;

  static void initialize() {
    cnt = 0;
    Display::ShiftPWMProcessor::set_brightness(64);
    Effects::Transition::set_effect(Effects::NumberTransition::FADE_CROSS, 9);
    Effects::Ambient::set_effect(Effects::AmbientEffect::CANDLE);      
  }

  static void process() {
    Effects::Transition::display(cnt);
    cnt++;
    cnt *= 1.1;
    if(cnt >= 9999) {
      UI::next<Clock>();
    }
  }
};

uint16_t TimeSet::cnt{0};


using sched = Scheduler<8000, 
  Display::ShiftPWMProcessor,
  BaseLightDimmer,
  UI,
  //Display::SeparatorDot,
  //RTCSync,
  //AmbientLight,
  SoundGenerator,
  //TimingBenchmark,
  //Display::AntiCathodePoisoning,
  Effects::Transition,
  Effects::Ambient
  //Demo
>;



void setup() {    
  Pins::setup();  

  TimingBenchmark::setup(&sched::benchmark_micros_total, &sched::benchmark_counts);

  sched::initialize();

  // Time setup menu
  // UI::timeout<TimeSetMinutes, MainLoop>(); // Defaults to first entry = MainLoop
  // UI::timeout<TimeSetMinutes, MainLoop>();
  // UI::select_long<MainLoop, TimeSetMinutes>();
  // UI::select_long<TimeSetMinutes, TimeSetHours>();
  // UI::select_short<TimeSetMinutes, TimeSetHours>();
  // UI::select_short<TimeSetHours, MainLoop>();
  
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
