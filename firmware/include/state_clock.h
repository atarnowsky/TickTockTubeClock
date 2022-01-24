#pragma once

#include "configuration.h"
#include "display.h"
#include "state.h"
#include "timing.h"
#include "settings.h"
#include "sound.h"
#include "ambient_light.h"

class Clock : public State<0>{
public:
  static void initialize() {
    settings = Settings::get();

    uint8_t brightness = settings.led_brightness;
    if(settings.night_mode_threshold != 0)
      brightness = 0;

    BaseLightDimmer::set_fade_target(brightness);

    Display::ShiftPWMProcessor::set_brightness(settings.tube_brightness);
    Effects::Transition::set_max_brightness(settings.tube_brightness);
    Effects::Ambient::set_max_brightness(settings.tube_brightness);    

    SoundGenerator::set_tick_tock(static_cast<TickTockSound>(settings.tick_sound));    

    Effects::NumberTransition transition = static_cast<Effects::NumberTransition>(settings.transition_effect);
    switch(transition) {
      case Effects::NumberTransition::NONE:
        Effects::Transition::set_effect(Effects::NumberTransition::NONE, 0);
        break;
      case Effects::NumberTransition::FADE_BLACK:
        Effects::Transition::set_effect(Effects::NumberTransition::FADE_BLACK, 11);
        break;
      case Effects::NumberTransition::FADE_CROSS:
        Effects::Transition::set_effect(Effects::NumberTransition::FADE_CROSS, 10);
        break;
      case Effects::NumberTransition::FLICKER:
        Effects::Transition::set_effect(Effects::NumberTransition::FLICKER, 13);
        break;
    }
    
    Effects::Ambient::set_effect(static_cast<Effects::AmbientEffect>(settings.ambient_effect));  
  }

  static void process() {    
    static bool nightmode_active = false;
    static bool shutdown_active = false;
    constexpr static uint16_t second_timeout = 1000/state_update_rate;   

    if(timer++ >= second_timeout) {
      timer = 0;
      dot = !dot;
      SoundGenerator::tick();

      time_pair time = RTCSync::current_time();
      if(!shutdown_active) {
        Effects::Transition::display(time.hours, time.minutes, {false, false, dot, false});  

        // Night mode ===
        // In case the ambient light goes below a specified threshold,
        // the brightness of the tubes is set to the minimum and all effects,
        // ticks and LEDs are turned off.
        uint32_t lux = AmbientLight::milli_lux()/10;
        if(lux < settings.night_mode_threshold) {
          nightmode_active = true;

          BaseLightDimmer::set_fade_target(0);
          Display::ShiftPWMProcessor::set_brightness(8);
          Effects::Transition::set_max_brightness(8);
          Effects::Ambient::set_max_brightness(8);    
          SoundGenerator::set_tick_tock(TickTockSound::None);
          Effects::Transition::set_effect(Effects::NumberTransition::NONE, 0);
          Effects::Ambient::set_effect(Effects::AmbientEffect::NONE);

        } else if(nightmode_active) {
          nightmode_active = false;
          initialize();
        }
      }

      // Tube saving mode ===
      // Automatically shuts down the whole clock within
      // a specified timespan in order to prolong the lifespan
      // of the clock.

      // Disabled if timespan is less than one hour
      shutdown_active = false;
      if(settings.shutdown_begin == settings.shutdown_end) return;

      if(settings.shutdown_begin < settings.shutdown_end) {
        shutdown_active = (time.hours >= settings.shutdown_begin) && (time.hours <= settings.shutdown_end);
      } else {
        shutdown_active = ((time.hours <= settings.shutdown_end) && (time.hours >= 0)) ||
                          ((time.hours >= settings.shutdown_begin) && (time.hours <= 23));
      }

      if(shutdown_active) {
        Effects::Transition::display(255, 255, {false, false, false, false});  
      }
    }
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
    UI::next<ServiceMenu>();    
  }

  static void on_select_short() {
    UI::next<TimeSet>();
  }

  static void on_select_long() {
    UI::next<SettingInit>();
  }

private:
  static uint16_t timer;    
  static bool dot;
  static SettingSet settings;
};
