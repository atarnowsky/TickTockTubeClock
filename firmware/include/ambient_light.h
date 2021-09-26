#include <Wire.h>
#include <ClosedCube_OPT3001.h>

ClosedCube_OPT3001 opt3001;
#define OPT3001_ADDRESS 0x44

void init_luxmeter()
{
  opt3001.begin(OPT3001_ADDRESS);
  OPT3001_Config newConfig;  
  newConfig.RangeNumber = B1100;  
  newConfig.ConvertionTime = B1;
  newConfig.Latch = B0;
  newConfig.ModeOfConversionOperation = B11;

  OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
  if (errorConfig != NO_ERROR)
  {
    buzzer.error(0b0111);
  }    
  else {
    buzzer.error(0b1100);
  }  
}

