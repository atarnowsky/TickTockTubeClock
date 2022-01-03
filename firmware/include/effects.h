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

      static void display(uint8_t hours, uint8_t minutes, const array<bool, 4>& dots = {false, false, false, false});
      static void set_effect(NumberTransition transition, uint8_t duration_shift = 9);
      static uint16_t current_number();
   };

   class Ambient : public RelaxedTask<10> {
    public:
      static void initialize();
      static void process();
      
      static void set_effect(AmbientEffect effect);

    private:
      static constexpr uint8_t max_variation = 230;
   };

}