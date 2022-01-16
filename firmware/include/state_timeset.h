#pragma once

#include "state.h"
#include "timing.h"
#include "sound.h"

class TimeSet : public State<15>{
public:
  static uint32_t timer;
  static bool hours;

  static void initialize() {    
    SoundGenerator::set_tick_tock(TickTockSound::None);
    Display::ShiftPWMProcessor::set_brightness(255);
    Effects::Transition::set_effect(Effects::NumberTransition::FADE_CROSS, 7);
    Effects::Ambient::set_effect(Effects::AmbientEffect::NONE);    
    Display::SeparatorDot::disable();

    timer = millis();  
    hours = false;
  }

  static void process() {
    constexpr uint16_t blank = 500;
    time_pair time = RTCSync::current_time();
    
    if(millis() - timer > blank) {
      if(millis() - timer >= blank * 2) timer = millis();
      if(hours) {
        time.hours = 255;
      } else {
        time.minutes = 255;
      }
    } 

    Effects::Transition::display(time.hours, time.minutes);
  }

  static void on_plus_short() {
    SoundGenerator::ack_short();
    if(hours)
        RTCSync::increment_hours(1);
    else
        RTCSync::increment_minutes(1);
  }

  static void on_minus_short() {
    SoundGenerator::ack_short();
    if(hours)
        RTCSync::increment_hours(-1);
    else
        RTCSync::increment_minutes(-1);
  }

  static void on_plus_long() {
    SoundGenerator::ack_long();
    if(hours)
        RTCSync::increment_hours(10);
    else
        RTCSync::increment_minutes(10);
  }

  static void on_minus_long() {
    SoundGenerator::ack_long();
    if(hours)
        RTCSync::increment_hours(-10);
    else
        RTCSync::increment_minutes(-10);
  }

  static void on_timeout() {
    UI::next<Clock>();
  }

  static void on_select_long() {
    UI::next<Clock>();
  }

  static void on_select_short() {
    if(!hours){
        hours = true;
        return;
    }
    
    UI::next<Clock>();
  }
};

uint32_t TimeSet::timer{millis()};
bool TimeSet::hours{false};