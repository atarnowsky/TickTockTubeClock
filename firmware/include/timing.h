#include <Wire.h>
#include <DS3232RTC.h>


void init_rtc()
{

  bool needs_init = RTC.oscStopped(true);
  
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) 
  {
    buzzer.error(0b0000);
    return;
  }

  if(needs_init)
  {
    buzzer.error(0b1010);    
    time_t t;
    
    tmElements_t tm;

    tm.Year = CalendarYrToTm(2021);
    tm.Month = 1;
    tm.Day = 1;
    tm.Hour = 0;
    tm.Minute = 0;
    tm.Second = 0;
    t = makeTime(tm);
    RTC.set(t);        // use the time_t value to ensure correct weekday is set
    setTime(t);
  }  

  setSyncInterval(10);
}

uint16_t current_time() {
  time_t t = now();  
  int minutes = minute(t);
  int hours = hour(t);
  return hours * 100 + minutes;
}

void increment_minutes() {
  time_t t = now();  
  int minutes = minute(t) + 1;  
  int hours = hour(t);
  if(minutes > 59){
    minutes = 0;
    hours = hours + 1;
  }
  if(hours > 23)
    hours = 0;
    
  tmElements_t tm;

  tm.Year = CalendarYrToTm(2021);
  tm.Month = 1;
  tm.Day = 1;
  tm.Hour = hours;
  tm.Minute = minutes;
  tm.Second = 0;
  t = makeTime(tm);
  RTC.set(t);        // use the time_t value to ensure correct weekday is set
  setTime(t);
}

void increment_hours() {
  time_t t = now();  
  int minutes = minute(t);  
  int hours = hour(t) + 1;  
  if(hours > 23)
    hours = 0;
    
  tmElements_t tm;

  tm.Year = CalendarYrToTm(2021);
  tm.Month = 1;
  tm.Day = 1;
  tm.Hour = hours;
  tm.Minute = minutes;
  tm.Second = 0;
  t = makeTime(tm);
  RTC.set(t);        // use the time_t value to ensure correct weekday is set
  setTime(t);
}
