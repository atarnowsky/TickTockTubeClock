#include <Arduino.h>

#include "io_tools.h"
#include "hw_map.h"

#include "sound.h"
#include "display.h"
#include "ambient_light.h"
#include "timing.h"

#include "state_machine.h" // TODO: Incomplete

#include "scheduler.h"

// TODO: Add temperature watchdog that shuts down the display in case of over temperature

// TODO: Add some EEPROM management base class for tasks

using sched = Scheduler<4000, 
  Display::ShiftPWMProcessor,
  Display::SeparatorDot,
  StateMachine,
  RTCSync,
  AmbientLight,
  SoundGenerator
>;

void setup() {    
  // TODO: Is this needed anymore?
  pinMode(PIN_PB0, OUTPUT);
  digitalWrite(PIN_PB0, LOW);


  Pins::setup();  

  sched::initialize();

  SoundGenerator::set_tick_tock(TickTockSound::MonotonousBeep);

  sched::start_critical();
  sched::start_relaxed();
}

void loop() {
  // Will never be called
  // TODO: use traditional main() function instead of this Arduino stuff...
}
