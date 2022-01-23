#include "state_clock.h"
#include "state_settings.h"
#include "state_service.h"


uint8_t SettingLEDBrightness::brightness{10};
uint8_t SettingTubeBrightness::brightness{10};

uint8_t SettingTickSound::sound_id{0};
uint16_t SettingTickSound::timer{0};


int16_t ServiceMenu::acp_countdown{-1};
uint8_t ServiceMenu::delay_counter{0};

uint16_t Clock::timer{0};
bool Clock::dot{false};