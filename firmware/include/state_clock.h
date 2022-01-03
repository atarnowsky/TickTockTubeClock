#pragma once

#include "state.h"
#include "timing.h"


class Clock : public State<0>{
public:
  static void initialize() {
    // Load settings from EEPROM
    // Set non-dynamic modules
    //      - Tick Sound
    //      - Effects (Crossfade, Stationary)
    //      - Base light brightness
    SoundGenerator::set_tick_tock(TickTockSound::ClickSilent);
    Effects::Transition::set_effect(Effects::NumberTransition::FADE_BLACK, 9);
    Effects::Ambient::set_effect(Effects::AmbientEffect::VARIANT);  
    
    Display::ShiftPWMProcessor::set_brightness(255); // Move to loop        
  }

  static void process() {
    // TODO: Remove int16 conversion as soon as new DS3232 header is verified
    time_pair time = RTCSync::current_time();
    Effects::Transition::display(time.hours, time.minutes);    
  }

  // Increment hours by one (summer-/wintertime)
  static void on_plus_long() {
    SoundGenerator::ack_long();
    RTCSync::increment_hours(1);
  }

  static void on_minus_long() {
    SoundGenerator::ack_long();
    RTCSync::increment_hours(-1);
  }

  static void on_select_reset() {
    SoundGenerator::ack_reset();
    // TODO: Initiate reset
  }

  static void on_select_short() {
    UI::next<TimeSet>();
  }

  static void on_select_long() {
    //UI::next<Setup>();
  }
};