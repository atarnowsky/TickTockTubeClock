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
#include "effects.h"
#include "settings.h"
#include "safety.h"

#include "simple_i2c.h"

#include "state_clock.h"
#include "state_timeset.h"
#include "state_settings.h"
#include "state_service.h"

#include "configuration.h"


using sched = Scheduler<3840, 
  Display::ShiftPWMProcessor, // @3840 Hz critical clock and 5 bit pwm: 120 Hz update rate
  Display::AntiCathodePoisoning,
  Effects::Transition,
  Effects::Ambient,
  Safety::TemperatureCheck,
  BaseLightDimmer,    
  RTCSync,
  //AmbientLight,
  UI
  //TimingBenchmark  
>;

void setup() {    
  Pins::setup();  
  I2C::initialize();
  
  BaseLightDimmer::set_fade_speed(2);
  BaseLightDimmer::set_brightness(0);

  sched::initialize();
  

  if(Settings::on_first_start()) {
    pause_critical();
    SoundGenerator::play(Note{659, 100, 75*3});
    SoundGenerator::play(Note{523, 100, 75});
    SoundGenerator::play(Note{659, 100, 125});
    SoundGenerator::play(Note{880, 200, 75});
    delay(250);
    resume_critical();
  }  


  sched::start_critical();
  sched::start_relaxed();
}

void loop() {
  // Will never be called
  // TODO: use traditional main() function instead of this Arduino stuff...
}
