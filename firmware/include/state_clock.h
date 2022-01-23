#pragma once

#include "configuration.h"
#include "display.h"
#include "state.h"
#include "timing.h"
#include "settings.h"
#include "sound.h"


class Clock : public State<0>{
public:
  static void initialize() {
    BaseLightDimmer::set_fade_target(Settings::get(Setting::LED_BRIGHTNESS));
    uint8_t tube_brightness = Settings::get(Setting::TUBE_BRIGHTNESS);
    Display::ShiftPWMProcessor::set_brightness(tube_brightness); // Move to loop  
    Effects::Transition::set_max_brightness(tube_brightness);
    Effects::Ambient::set_max_brightness(tube_brightness);    
    SoundGenerator::set_tick_tock(static_cast<TickTockSound>(Settings::get(Setting::TICK_SOUND)));    

    // Load settings from EEPROM
    // Set non-dynamic modules    
    //      - Effects (Crossfade, Stationary)
    //      - Base light brightness    
    Effects::Transition::set_effect(Effects::NumberTransition::FADE_BLACK, 11);
    Effects::Ambient::set_effect(Effects::AmbientEffect::CANDLE);  
  }

  static void process() {    
    constexpr static uint16_t second_timeout = 1000/state_update_rate;    
    // TODO: Remove int16 conversion as soon as new DS3232 header is verified
    time_pair time = RTCSync::current_time();
    Effects::Transition::display(time.hours, time.minutes);    
    
    // TODO: Implement special transition for dots
    Display::BufferControl::show_dots({false, false, dot, false});

    if(timer++ >= second_timeout) {
      timer = 0;
      dot = !dot;
      SoundGenerator::tick();
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
};
