#include "stm32f1xx.h"
#include "efa_dyn_ind.h"
#include "platform_dyn_ind.h"

GPIO_TypeDef * 	A_GPIO[A_QTY] = {
	GPIOD,
	GPIOB,
	GPIOB,
	GPIOB,
	GPIOB,
	GPIOB
};  

uint16_t 		A_Pins[A_QTY] = {
	GPIO_PIN_2,
	GPIO_PIN_3,
	GPIO_PIN_4,
	GPIO_PIN_6,
	GPIO_PIN_8,
	GPIO_PIN_9
};

GPIO_TypeDef * 	K_GPIO[K_QTY] = {
	GPIOB,
	GPIOA,
	GPIOA,
	GPIOC,
	GPIOC,
	GPIOC, 
	GPIOC,
	GPIOA,
	GPIOC,
	GPIOC,
	GPIOC,
	GPIOC,
	GPIOC,
	GPIOA,
	GPIOB,
	GPIOB,
	GPIOC,
	GPIOC
}; 
 	  
uint16_t 		K_Pins[K_QTY] = {
	GPIO_PIN_14,
	GPIO_PIN_10,
	GPIO_PIN_9,
	GPIO_PIN_8,
	GPIO_PIN_7,
	GPIO_PIN_6,
	GPIO_PIN_9,
	GPIO_PIN_8,
	GPIO_PIN_10,	 
	GPIO_PIN_1,
	GPIO_PIN_0,
	GPIO_PIN_2,
	GPIO_PIN_3,
	GPIO_PIN_15,
	GPIO_PIN_2,
	GPIO_PIN_1,
	GPIO_PIN_11,
	GPIO_PIN_12
};
 
GPIO_TypeDef * 	B_GPIO[B_QTY] = {
	GPIOA,
	GPIOA,
	GPIOA,
	//GPIOB
};
	  
uint16_t 		B_Pins[B_QTY] = {
	GPIO_PIN_1,
	GPIO_PIN_2,	
	GPIO_PIN_3,
	//GPIO_PIN_8,
};

/*****************/

/* COMMON IND */
DigitTypeDef 	SingleD = {1,{4},{0}};
DigitTypeDef 	DoubleD = {2,{1,2},{0,0}};

/* LARGE IND */
LEDTypeDef 		LED_NE_rd = {1,{0},{17}};  
LEDTypeDef 		LED_NE_gr = {1,{1},{17}}; 
LEDTypeDef 		LED_LF_leak = {1,{2},{17}};	
LEDTypeDef 		LED_ped_mono = {1,{3},{17}};
LEDTypeDef 		LED_ped_bi = {1,{4},{17}};

LEDTypeDef 		LED_mix1 = {1,{5},{0}};	  
LEDTypeDef 		LED_mix2 = {1,{5},{1}};
LEDTypeDef 		LED_mix3 = {1,{5},{2}};

LEDTypeDef 		LED_cm1 = {1,{5},{3}};
LEDTypeDef 		LED_cm2 = {1,{5},{4}};
LEDTypeDef 		LED_cm3 = {1,{5},{5}};

LEDTypeDef 		LED_gm1 = {1,{5},{6}};
LEDTypeDef 		LED_gm2 = {1,{5},{7}};
LEDTypeDef 		LED_gm3 = {1,{5},{8}};

LEDTypeDef 		LED_bi1 = {1,{5},{9}};
LEDTypeDef 		LED_bi2 = {1,{5},{10}};
LEDTypeDef 		LED_bi3 = {1,{5},{11}};

LEDTypeDef 		LED_cut = 	{2,{0,1},{16,16}};
LEDTypeDef 		LED_coag = 	{2,{2,3},{16,16}};
LEDTypeDef 		LED_bi = 	{2,{4,5},{16,16}};


DigitTypeDef 	ProgD = {1,{0},{0}};
DigitTypeDef 	CutD = {3,{1,2,3},{0,0,0}};
DigitTypeDef 	CoagD = {3,{4,0,1},{0,8,8}};
DigitTypeDef 	BiD = {3,{2,3,4},{8,8,8}};	

DigitTypeDef 	C1D = {1,{2},{8}};	
DigitTypeDef 	C2D = {1,{3},{8}};	
DigitTypeDef 	C3D = {1,{4},{8}};	

/* VIRIDIS IND */
LEDTypeDef 		LEDv_NE_rd = {1,{6},{2}};  
LEDTypeDef 		LEDv_NE_gr = {1,{7},{2}}; 
LEDTypeDef 		LEDv_LF_leak = {1,{6},{3}};	
											 
LEDTypeDef 		LEDv_cm1 = {1,{6},{4}};
LEDTypeDef 		LEDv_cm2 = {1,{6},{5}};
LEDTypeDef 		LEDv_cm3 = {1,{6},{6}};

LEDTypeDef 		LEDv_gm1 = {1,{7},{6}};
LEDTypeDef 		LEDv_gm2 = {1,{7},{7}};
LEDTypeDef 		LEDv_gm3 = {1,{7},{4}};

LEDTypeDef 		LEDv_cut = 	{2,{6,7},{0,0}};
LEDTypeDef 		LEDv_coag = {2,{6,7},{1,1}};

DigitTypeDef 	CutDv = {3,{3,4,5},{0,0,0}};
DigitTypeDef 	CoagDv = {3,{0,1,2},{0,0,0}};

DigitTypeDef 	CutDv9 = {2,{4,5},{0,0}};
DigitTypeDef 	CoagDv9 = {2,{1,2},{0,0}};

DigitTypeDef 	CvD = {1,{1},{0}};

/* APK IND */
LEDTypeDef 		LED_gas_rd = {1,{6},{4}};
LEDTypeDef 		LED_expulse = {1,{6},{5}};	   
LEDTypeDef 		LED_gas_on = {1,{6},{6}};
LEDTypeDef 		LED_no_instr = {1,{7},{5}};
LEDTypeDef 		LED_low_tr = {1,{7},{3}};
						   
DigitTypeDef 	FlowD = {2,{4,5},{0,0}};
DigitTypeDef 	PowD = {3,{0,1,2},{0,0,0}};
