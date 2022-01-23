#pragma once

#include "state_machine.h"

class Clock;
class TimeSet;
class SettingInit;
class SettingLEDBrightness;
class SettingTubeBrightness;
class SettingTickSound;
class ServiceMenu;

using UI = StateMachine<
  Clock,       // Index 0 automatically denotes the initial State to run
  TimeSet,
  SettingInit,
  SettingLEDBrightness,
  SettingTubeBrightness,
  SettingTickSound,
  ServiceMenu
>;