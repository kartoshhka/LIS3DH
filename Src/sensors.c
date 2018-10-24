
#include "stm32l4xx.h"
#include "sensors.h"

uint16_t Sonar_Reading;
uint32_t Sonar_Value;
uint8_t Signal_Count;


void Sonar_Get(void)
{
	// 33100 - Sound speed (cm/sec)
	// 1/1000000 - timer freq
	// Result = cm
	Sonar_Value = (331/2) * (uint32_t)Sonar_Reading/10000;
	//if (Sonar > 4000) Sonar = 4000;
	//if (Sonar < 20) Sonar = 20;
}
