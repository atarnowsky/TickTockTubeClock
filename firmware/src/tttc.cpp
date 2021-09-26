#include <Arduino.h>
#include <JC_Button.h>

#include "sound.h"
static SoundGenerator buzzer(PIN_PC1);

#include "display.h"
static Display display;


#include "ambient_light.h"
#include "timing.h"

uint16_t cnt = 0;

constexpr uint8_t states = 4;
enum State {
  Time = 0,
  Temperature = 1,
  LightMeter = 2,
  Off = 3
};

uint8_t current_state = Time;

Button button_plus(PIN_PD1);
Button button_minus(PIN_PD2);
Button button_select(PIN_PD0);

void setup() {

  pinMode(SDA, INPUT_PULLUP);  
  pinMode(SCL, INPUT_PULLUP); 
 
  pinMode(PIN_PB0, OUTPUT);
  digitalWrite(PIN_PB0, LOW);

  button_plus.begin();
  button_minus.begin();
  button_select.begin();
  
  display.initialize();
  display.led_off();     
  buzzer.set_tick_tock(TickTockSound::None);
  init_luxmeter();
  init_rtc();
}

bool led_state = false;
bool long_plus = false;

uint16_t wait_timer = 0;

void loop() {
  wait_timer++;
  if(wait_timer > 1000) wait_timer = 0;

  button_plus.read();
  button_minus.read();
  button_select.read();
  buzzer.process();
  display.process();

  if(button_select.pressedFor(500) && !long_plus)
  {
    long_plus = true;
    led_state = !led_state;
    buzzer.ack(2);
    if(led_state) 
    {
      display.led_on();
      buzzer.set_tick_tock(TickTockSound::BitoneClick);
    }
    else 
    {
      display.led_off();
      buzzer.set_tick_tock(TickTockSound::None);
    }
  }

   if(button_select.wasReleased())
   {
    long_plus = false;
   }

  if(button_select.wasPressed())
  {
    current_state++;
    current_state = current_state % states;  
    buzzer.ack();
    wait_timer = 0;
  }

  if(button_minus.wasPressed() && current_state == Time)
  {
    increment_minutes();
    buzzer.ack();
  }

  if(button_plus.wasPressed() && current_state == Time)
  {
    increment_hours();
    buzzer.ack();
  }

  switch (current_state)
  {
  case Time:
      if(wait_timer == 0)
        display.time(current_time());
    break;

    case Temperature:
    {
      if(wait_timer == 0){
      float temperature = RTC.temperature() / 4.0f;
      display.number(temperature);
      }
    }    
    break;

    case LightMeter:
    {
      if(wait_timer == 0) {
      OPT3001 result = opt3001.readResult();  
      display.number(result.lux);
      }
    }
    break;
  
  default:
      display.clear();
    break;
  }

}
