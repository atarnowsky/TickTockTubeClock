#pragma once

#include "scheduler.h"

namespace Safety {
    class TemperatureCheck : public RelaxedTask<1000> {
    public:
      static constexpr uint8_t shutdown_temperature = 40;
      static void initialize();
      static void process();
   };
}