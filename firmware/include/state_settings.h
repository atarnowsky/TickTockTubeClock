#pragma once

#include "configuration.h"
#include "state.h"
#include "timing.h"
#include "sound.h"
#include "settings.h"
#include "base_light.h"
#include "display.h"

class SettingLEDBrightness;
class SettingTubeBrightness;
class SettingTransition;
class SettingAmbient;
class SettingTickTock;

static constexpr uint8_t settings_timeout = 15;

class SettingInit : public State<settings_timeout> {
public: 
  static void initialize();
};

class SettingLEDBrightness : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 1;

  static void initialize();
  static void finish();
  static void process();
  static void on_plus_short();
  static void on_minus_short();
  static void on_select_short();
  static void on_timeout();

private:
  static uint8_t brightness;
};


class SettingTubeBrightness : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 2;

  static void initialize();
  static void finish();
  static void process();
  static void on_plus_short();
  static void on_minus_short();
  static void on_select_short();
  static void on_timeout();

private:
  static uint8_t brightness;
};


class SettingTickSound : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 3;

  static void initialize();
  static void finish();
  static void process();
  static void on_plus_short();
  static void on_minus_short();
  static void on_select_short();
  static void on_timeout();

private:
  static uint8_t sound_id;
  static uint16_t timer;
};


class SettingTransitionEffect : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 4;

  static void initialize();
  static void finish();
  static void process();
  static void on_plus_short();
  static void on_minus_short();
  static void on_select_short();
  static void on_timeout();

private:
  static uint8_t effect_id;
};


class SettingAmbientEffect : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 5;

  static void initialize();
  static void finish();
  static void process();
  static void on_plus_short();
  static void on_minus_short();
  static void on_select_short();
  static void on_timeout();

private:
  static uint8_t effect_id;
};


class SettingNightMode : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 6;

  static void initialize();
  static void finish();
  static void process();
  static void on_plus_short();
  static void on_minus_short();
  static void on_select_short();
  static void on_timeout();

private:
  static uint8_t threshold;
};


class SettingOffBegin : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 7;

  static void initialize();
  static void finish();
  static void process();
  static void on_plus_short();
  static void on_minus_short();
  static void on_select_short();
  static void on_timeout();

private:
  static uint8_t hour;
};


class SettingOffEnd : public State<settings_timeout> {
public: 
  static constexpr uint8_t setting_id = 8;

  static void initialize();
  static void finish();
  static void process();
  static void on_plus_short();
  static void on_minus_short();
  static void on_select_short();
  static void on_timeout();

private:
  static uint8_t hour;
};