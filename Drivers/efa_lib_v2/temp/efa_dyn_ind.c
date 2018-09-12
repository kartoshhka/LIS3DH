#define __STM32F1
/*#ifdef __STM32F1
	#include "stm32f10x.h"
#endif
#ifdef __STM32F2
	#include "stm32f2xx.h"
#endif*/
//#include "efa_dyn_conf.h"
#include "efa_dyn_ind.h"
#include "efa_dyn_ind_letters.h"
#include "platform_dyn_ind.h"

uint32_t DI_Buttons = 0;
uint32_t buttons_buf = 0;
uint32_t INV_Buttons = 0;
uint32_t inv_buttons_buf = 0;
uint32_t buttons_buf_prev = 0;
uint8_t anode_cnt = 0;
uint32_t ind_data[A_QTY];
uint32_t b_ind_data[A_QTY];
uint8_t data_lock = 0;

void DI_write(DigitTypeDef * where, uint8_t * what) {
	uint8_t i;
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[i]] &=~(0xFF<<(where->Kn[i])); 	
		ind_data[where->An[i]] |= (DI_letters[what[i]]<<(where->Kn[i])); 
	}
	data_lock = 0;
}

void DI_write_point(DigitTypeDef * where, uint8_t * what, uint8_t point_pos) {
uint8_t i;
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[i]] &=~(0xFF<<(where->Kn[i])); 	
		ind_data[where->An[i]] |= (DI_letters[what[i]]<<(where->Kn[i])); 
	}
	
	if (point_pos <= where->size-1) {
		ind_data[where->An[where->size-1-point_pos]] |= 1<<(where->Kn[where->size-1-point_pos]);	
	}
	data_lock = 0;
}

void DI_write_pointfix(DigitTypeDef * where, uint8_t * what) {
uint8_t i;
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[i]] &=~(0xFE<<(where->Kn[i])); 	
		ind_data[where->An[i]] |= (DI_letters[what[i]]<<(where->Kn[i])); 
	}
	data_lock = 0;
}

void DI_set(DigitTypeDef * where, uint32_t what) {
uint8_t i;
uint16_t div = 1; 
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[where->size-1-i]] &=~(0xFF<<(where->Kn[where->size-1-i])); 	
		if ((what/div)%10 || (what/div) || (what/div/100)%10 || i==0) 
			ind_data[where->An[where->size-1-i]] |= (DI_digits[(what/div)%10]<<(where->Kn[where->size-1-i])); 
		div *= 10; 
	}	
	data_lock = 0;
}

void DI_set_point(DigitTypeDef * where, uint32_t what, uint8_t point_pos) {
uint8_t i;
uint16_t div = 1; 
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[where->size-1-i]] &=~(0xFF<<(where->Kn[where->size-1-i])); 	
		if ((what/div)%10 || (what/div) || i==0) ind_data[where->An[where->size-1-i]] |= (DI_digits[(what/div)%10]<<(where->Kn[where->size-1-i]));
		div *= 10; 
	}
	if (point_pos <= where->size-1) {
		ind_data[where->An[where->size-1-point_pos]] |= 1<<(where->Kn[where->size-1-point_pos]);	
	}
	data_lock = 0;
}

void DI_set_pointfix(DigitTypeDef * where, uint32_t what) {
uint8_t i;
uint16_t div = 1; 
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[where->size-1-i]] &=~(0xFE<<(where->Kn[where->size-1-i])); 	
		if ((what/div)%10 || (what/div) || i==0) ind_data[where->An[where->size-1-i]] |= (DI_digits[(what/div)%10]<<(where->Kn[where->size-1-i]));
		div *= 10; 
	}	
	data_lock = 0;
}

void DI_set_zero(DigitTypeDef * where, uint32_t what) {
uint8_t i;
uint16_t div = 1; 
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[where->size-1-i]] &=~(0xFF<<(where->Kn[where->size-1-i])); 	
		ind_data[where->An[where->size-1-i]] |= (DI_digits[(what/div)%10]<<(where->Kn[where->size-1-i]));
		div *= 10; 
	}	
	data_lock = 0;
}

void DI_set_point_zero(DigitTypeDef * where, uint32_t what, uint8_t point_pos) {
uint8_t i;
uint16_t div = 1; 
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[where->size-1-i]] &=~(0xFF<<(where->Kn[where->size-1-i])); 	
		ind_data[where->An[where->size-1-i]] |= (DI_digits[(what/div)%10]<<(where->Kn[where->size-1-i]));
		div *= 10; 
	}
	if (point_pos <= where->size-1) {
		ind_data[where->An[where->size-1-point_pos]] |= 1<<(where->Kn[where->size-1-point_pos]);	
	}
	data_lock = 0;
}

void DI_set_pointfix_zero(DigitTypeDef * where, uint32_t what) {
uint8_t i;
uint16_t div = 1; 
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[where->size-1-i]] &=~(0xFE<<(where->Kn[where->size-1-i])); 	
		ind_data[where->An[where->size-1-i]] |= (DI_digits[(what/div)%10]<<(where->Kn[where->size-1-i]));
		div *= 10; 
	}	
	data_lock = 0;
}

void DI_clr(DigitTypeDef * where) {
uint8_t i;
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[i]] &=~(0xFF<<(where->Kn[i]));
	}	
	data_lock = 0;
}

void DI_clr_pointfix(DigitTypeDef * where) {
uint8_t i;
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[i]] &=~(0xFE<<(where->Kn[i]));
	}	
	data_lock = 0;
}

void DI_LED(LEDTypeDef * where, uint8_t state) {
uint8_t i;
	data_lock = 1;
	if (state) {
		for (i = 0; i < where->size; i++) {
			ind_data[where->An[i]] |= (1<<(where->Kn[i]));
		}	
	} else {
		for (i = 0; i < where->size; i++) {
			ind_data[where->An[i]] &=~(1<<(where->Kn[i]));
		}
	}
	data_lock = 0;
} 

void DI_custom(DigitTypeDef * where, uint8_t * what) {
uint8_t i;
	data_lock = 1;
	for (i = 0; i < where->size; i++) {
		ind_data[where->An[i]] &=~(0xFF<<(where->Kn[i])); 	
		ind_data[where->An[i]] |= (what[i]<<(where->Kn[i]));
	}
	data_lock = 0;
}

void DI_On_Handler(void) {
uint8_t i;

	#ifdef K_QTY
		for (i = 0; i < K_QTY; i++) {
			if (b_ind_data[anode_cnt]&(1<<i)) {
				K_GPIO[i] -> BSRR = K_Pins[i];	
			} else {
				K_GPIO[i] -> BRR = K_Pins[i];			
			}
		}
	#endif	

	#ifdef __STM32F1
		A_GPIO[anode_cnt] -> BRR = A_Pins[anode_cnt];
	#endif
	#ifdef __STM32F2
		A_GPIO[anode_cnt] -> BSRRH = A_Pins[anode_cnt];
	#endif
}

void DI_Off_Handler(void) {
uint8_t i;

	for (i = 0; i < B_QTY; i++) {	
		if ((B_GPIO[i] -> IDR) & B_Pins[i]) 
			buttons_buf |= 1<<(i + B_QTY*anode_cnt); 
		else inv_buttons_buf |= 1<<(i + B_QTY*anode_cnt);
	}

	#ifdef __STM32F1
		A_GPIO[anode_cnt] -> BSRR = A_Pins[anode_cnt];
	#endif
	#ifdef __STM32F2
		A_GPIO[anode_cnt] -> BSRRL = A_Pins[anode_cnt];
	#endif
	
	anode_cnt++;
	
	if (anode_cnt == A_QTY) {
		anode_cnt = 0;
		
		#ifdef __BUTTONS_AVG
			if (buttons_buf == buttons_buf_prev) {
				INV_Buttons = inv_buttons_buf;		
				DI_Buttons = buttons_buf;
			}
			buttons_buf_prev = buttons_buf;
			
			inv_buttons_buf = 0;
			buttons_buf = 0;
		#else
			INV_Buttons = inv_buttons_buf;		
			DI_Buttons = buttons_buf;
			inv_buttons_buf = 0;
			buttons_buf = 0;
		#endif
	}
	
	if (!data_lock) b_ind_data[anode_cnt] = ind_data[anode_cnt];
	
}

