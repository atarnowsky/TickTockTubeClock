#pragma once

#include "state.h"
#include "timing.h"
#include "sound.h"
#include "settings.h"
#include "base_light.h"

class SettingLEDBrightness;
class SettingTubeBrightness;
class SettingTransition;
class SettingAmbient;
class SettingTickTock;

constexpr uint8_t settings_timeout = 15;

constexpr uint8_t light_map[] = {
  0,
  8,
  16,
  32,
  48,
  64,
  80,
  96,
  128,
  192,
  255 
};

class SettingInit : public State<settings_timeout> {
public: 
  static void initialize() {    
    reset_environment();
    SoundGenerator::ack_long();
    UI::next<SettingLEDBrightness>();     
  }
};


class SettingLEDBrightness : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 1;

  static void initialize() {    
    uint8_t target = Settings::get(Setting::LED_BRIGHTNESS);
    for(uint8_t i = 0; i <= 10; i++) {
      if(light_map[i] == target) {
        brightness = i;
        break;
      }
    }    
  }

  static void finish() {     
      Settings::set(Setting::LED_BRIGHTNESS, light_map[brightness]);
      BaseLightDimmer::set_fade_target(0);
  }

  static void process() {
      BaseLightDimmer::set_fade_target(light_map[brightness]);
      BaseLightDimmer::set_brightness(light_map[brightness]);
      Effects::Transition::display(setting_id, brightness, {false, false, true, false});
  }

  static void on_plus_short() {
      if(brightness >= 10) return;
      SoundGenerator::ack_short();
      brightness++;
  }

  static void on_minus_short() {
      if(brightness == 0) return;
      SoundGenerator::ack_short();      
      brightness--;
  }

  static void on_select_short() {
      UI::next<SettingTubeBrightness>();
  }

  static void on_timeout() {
      UI::next<Clock>();
  }


private:
  static uint8_t brightness;
};

uint8_t SettingLEDBrightness::brightness{10};


class SettingTubeBrightness : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 2;

  static void initialize() {    
    uint8_t target = Settings::get(Setting::TUBE_BRIGHTNESS);
    for(uint8_t i = 0; i <= 10; i++) {
      if(light_map[i] == target) {
        brightness = i;
        break;
      }
    }    
  }

  static void finish() {     
      Settings::set(Setting::TUBE_BRIGHTNESS, light_map[brightness]);
      Display::ShiftPWMProcessor::set_brightness(255);
  }

  static void process() {
      Display::ShiftPWMProcessor::set_brightness(light_map[brightness]);
      Effects::Transition::display(setting_id, brightness, {false, false, true, false});
  }

  static void on_plus_short() {
      if(brightness >= 10) return;
      SoundGenerator::ack_short();
      brightness++;
  }

  static void on_minus_short() {
      if(brightness <= 1) return;
      SoundGenerator::ack_short();      
      brightness--;
  }

  static void on_select_short() {
      UI::next<SettingTickSound>();
  }

  static void on_timeout() {
      UI::next<Clock>();
  }


private:
  static uint8_t brightness;
};

uint8_t SettingTubeBrightness::brightness{10};


class SettingTickSound : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 3;

  static void initialize() {    
    sound_id = Settings::get(Setting::TICK_SOUND);
    SoundGenerator::set_tick_tock(static_cast<TickTockSound>(sound_id));
  }

  static void finish() {     
      Settings::set(Setting::TICK_SOUND, sound_id);
      SoundGenerator::set_tick_tock(TickTockSound::None);
  }

  static void process() {      
      Effects::Transition::display(setting_id, sound_id, {false, false, true, false});
  }

  static void on_plus_short() {
      if(sound_id >= 5) return;
      SoundGenerator::ack_short();
      sound_id++;
      SoundGenerator::set_tick_tock(static_cast<TickTockSound>(sound_id));
  }

  static void on_minus_short() {
      if(sound_id <= 0) return;
      SoundGenerator::ack_short();      
      sound_id--;
      SoundGenerator::set_tick_tock(static_cast<TickTockSound>(sound_id));
  }

  static void on_select_short() {
      UI::next<Clock>();
  }

  static void on_timeout() {
      UI::next<Clock>();
  }


private:
  static uint8_t sound_id;
};

uint8_t SettingTickSound::sound_id{0};

