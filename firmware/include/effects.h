#pragma once

#include "scheduler.h"

namespace Effects {

   enum class NumberTransition : uint8_t {
      NONE,
      FADE_BLACK,
      FADE_CROSS,
      FLICKER
   };

   enum class AmbientEffect : uint8_t {
      NONE,
      VARIANT,
      CANDLE,
      DEFECTIVE
   };

   class Transition : public RelaxedTask<> {
    public:
      static void initialize();
      static void process();

      static void clear();
      static void display_option(uint8_t number, uint8_t value);
      static void display(uint8_t hours, uint8_t minutes, const array<bool, 4>& dots = {false, false, false, false});
      static void display(uint16_t value);
      static void set_effect(NumberTransition transition, uint8_t duration_shift = 9);
      static void set_max_brightness(uint8_t brightness = 255);
      static uint16_t current_number();

   private:
      static uint8_t max_brightness;
   };

   class Ambient : public RelaxedTask<10> {
    public:
      static void initialize();
      static void process();
      
      static void set_effect(AmbientEffect effect);
      static void set_max_brightness(uint8_t brightness = 255);

    private:
      static constexpr uint8_t max_variation = 230;
      static uint8_t max_brightness;
   };

}