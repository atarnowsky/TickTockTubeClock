#pragma once

#include "scheduler.h"

namespace Effects {

   enum class NumberTransition {
      NONE,
      FADE_BLACK,
      FADE_CROSS,
      FLICKER
   };

   class Transition : public RelaxedTask<> {
   public:
      static void initialize();
      static void process();

      static void display(uint16_t value, const array<bool, 4>& dots = {false, false, false, false});
      static void set_effect(NumberTransition transition, uint16_t duration = 500);
   };

}