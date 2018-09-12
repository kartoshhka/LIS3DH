#define __DMA_TIM 				TIM2
#define __DMA_TIM_SRC			TIM_DMA_Update
#define __CARRY_TIM				TIM4
#define __CARRY_CCR				CCR2
#define __DMA_TIM_Channel		DMA1_Channel2
#define __CARRIER_CCR_ADDRESS	(uint32_t)&TIM4->CCR2	   //T4C2
#define __DMA_TIM_GLIF DMA_FLAG_GL2

#define Set_volume(lvl) __CARRY_TIM->ARR = 255+(700-lvl*100)

#define __STM32F1

/* CCR Address help 
BASE 0x40000000
+
TIM2 0x00000000
TIM3 0x00000400
TIM4 0x00000800
TIM5 0x00000C00
+
CCR1 0x00000034
CCR2 0x00000038
CCR3 0x0000003C
CCR4 0x00000040
= ...
****************** */

/* DMA Help 
DMA2
ch1	 TIM5_CC4
ch2	 TIM5_CC3 / TIM5_Upd
ch3	 TIM6_Upd
ch4	 TIM5_CC2
ch5	 TIM5_CC1
******************* */

//#define __STOP_IMMIDIATELY 
#define __STOP_NORMALIZED 

