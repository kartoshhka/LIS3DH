#ifndef __PLATFORM_DYN_IND
#define __PLATFORM_DYN_IND

#include "stm32f1xx.h"

/*****************/

/* COMMON IND */
extern DigitTypeDef 	SingleD;
extern DigitTypeDef 	DoubleD;

/* LARGE IND */
extern LEDTypeDef 		LED_NE_rd;  
extern LEDTypeDef 		LED_NE_gr; 
extern LEDTypeDef 		LED_LF_leak;	
extern LEDTypeDef 		LED_ped_mono;
extern LEDTypeDef 		LED_ped_bi;

extern LEDTypeDef 		LED_mix1;	  
extern LEDTypeDef 		LED_mix2;
extern LEDTypeDef 		LED_mix3;

extern LEDTypeDef 		LED_cm1;
extern LEDTypeDef 		LED_cm2;
extern LEDTypeDef 		LED_cm3;

extern LEDTypeDef 		LED_gm1;
extern LEDTypeDef 		LED_gm2;
extern LEDTypeDef 		LED_gm3;

extern LEDTypeDef 		LED_bi1;
extern LEDTypeDef 		LED_bi2;
extern LEDTypeDef 		LED_bi3;

extern LEDTypeDef 		LED_cut;
extern LEDTypeDef 		LED_coag;
extern LEDTypeDef 		LED_bi;


extern DigitTypeDef 	ProgD;
extern DigitTypeDef 	CutD;
extern DigitTypeDef 	CoagD;
extern DigitTypeDef 	BiD;	

extern DigitTypeDef 	C1D;	
extern DigitTypeDef 	C2D;	
extern DigitTypeDef 	C3D;	

/* VIRIDIS IND */
extern LEDTypeDef 		LEDv_NE_rd;  
extern LEDTypeDef 		LEDv_NE_gr; 
extern LEDTypeDef 		LEDv_LF_leak;	
											 
extern LEDTypeDef 		LEDv_cm1;
extern LEDTypeDef 		LEDv_cm2;
extern LEDTypeDef 		LEDv_cm3;

extern LEDTypeDef 		LEDv_gm1;
extern LEDTypeDef 		LEDv_gm2;
extern LEDTypeDef 		LEDv_gm3;

extern LEDTypeDef 		LEDv_cut;
extern LEDTypeDef 		LEDv_coag;

extern DigitTypeDef 	CutDv;
extern DigitTypeDef 	CoagDv;

extern DigitTypeDef 	CutDv9;
extern DigitTypeDef 	CoagDv9;

extern DigitTypeDef 	CvD;

/* APK IND */
extern LEDTypeDef 		LED_gas_rd;
extern LEDTypeDef 		LED_expulse;	   
extern LEDTypeDef 		LED_gas_on;
extern LEDTypeDef 		LED_no_instr;
extern LEDTypeDef 		LED_low_tr;
						   
extern DigitTypeDef 	FlowD;
extern DigitTypeDef 	PowD;

#endif
