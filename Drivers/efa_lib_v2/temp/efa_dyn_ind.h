#ifndef EFA_DYN_IND_H
#define EFA_DYN_IND_H

#include "stdint.h"
#include "stm32f1xx.h"

typedef struct {
	uint8_t size; //5 max
	uint8_t An[5];
	uint8_t Kn[5];
} DigitTypeDef;

typedef struct {
	uint8_t size; //2 max
	uint8_t An[2];
	uint8_t Kn[2];
} LEDTypeDef;

extern uint32_t DI_Buttons;
extern uint32_t INV_Buttons;

#define A_QTY 6
#define K_QTY 18
#define B_QTY 3

extern GPIO_TypeDef * 	A_GPIO[A_QTY];  
extern uint16_t 		A_Pins[A_QTY];
#ifdef K_QTY
extern GPIO_TypeDef * 	K_GPIO[K_QTY];  	  
extern uint16_t 		K_Pins[K_QTY];
#endif 
extern GPIO_TypeDef * 	B_GPIO[B_QTY];	  
extern uint16_t 		B_Pins[B_QTY];

void DI_write(DigitTypeDef * where, uint8_t * what);
void DI_write_point(DigitTypeDef * where, uint8_t * what, uint8_t point_pos);
void DI_write_pointfix(DigitTypeDef * where, uint8_t * what);
void DI_set(DigitTypeDef * where, uint32_t what);
void DI_set_point(DigitTypeDef * where, uint32_t what, uint8_t point_pos);
void DI_set_pointfix(DigitTypeDef * where, uint32_t what);
void DI_set_zero(DigitTypeDef * where, uint32_t what);
void DI_set_point_zero(DigitTypeDef * where, uint32_t what, uint8_t point_pos);
void DI_set_pointfix_zero(DigitTypeDef * where, uint32_t what);
void DI_clr(DigitTypeDef * where);
void DI_clr_pointfix(DigitTypeDef * where);
void DI_custom(DigitTypeDef * where, uint8_t * what);

void DI_LED(LEDTypeDef * where, uint8_t state); 

void DI_On_Handler(void);
void DI_Off_Handler(void); 

#endif
