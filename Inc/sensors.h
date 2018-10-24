#ifndef SENSORS_H_
#define SENSORS_H_

#include "stm32l4xx_hal.h"

extern uint16_t Sonar_Reading;
extern uint32_t Sonar_Value;
extern uint8_t Signal_Count;

void Sonar_Get(void);

#endif
