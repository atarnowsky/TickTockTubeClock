#pragma once

#include "state.h"
#include "timing.h"
#include "settings.h"


class Clock : public State<0>{
public:
  static void initialize() {
    BaseLightDimmer::set_fade_target(Settings::get(Setting::LED_BRIGHTNESS));
    uint8_t tube_brightness = Settings::get(Setting::TUBE_BRIGHTNESS);
    Display::ShiftPWMProcessor::set_brightness(tube_brightness); // Move to loop  
    Effects::Transition::set_max_brightness(tube_brightness);
    Effects::Ambient::set_max_brightness(tube_brightness);    
    SoundGenerator::set_tick_tock(static_cast<TickTockSound>(Settings::get(Setting::TICK_SOUND)));
    Display::SeparatorDot::enable();

    // Load settings from EEPROM
    // Set non-dynamic modules    
    //      - Effects (Crossfade, Stationary)
    //      - Base light brightness    
    Effects::Transition::set_effect(Effects::NumberTransition::FADE_BLACK, 11);
    Effects::Ambient::set_effect(Effects::AmbientEffect::CANDLE);  
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
    Settings::reset();
    Display::SeparatorDot::disable();
    SoundGenerator::set_tick_tock(TickTockSound::None);
    RTCSync::reset();
    
    // Stop program completely:
    cli();
    IO::low(Pins::Anode::MuxA, Pins::Anode::MuxB); 
    for(;;);
  }

  static void on_select_short() {
    UI::next<TimeSet>();
  }

  static void on_select_long() {
    UI::next<SettingInit>();
  }
};