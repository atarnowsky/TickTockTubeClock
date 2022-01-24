#include "state_clock.h"
#include "state_settings.h"
#include "state_service.h"



int16_t ServiceMenu::acp_countdown{-1};
uint8_t ServiceMenu::delay_counter{0};

uint16_t Clock::timer{0};
bool Clock::dot{false};
SettingSet Clock::settings{};