#include "stm32f0xx_hal.h"
#include "platform_tim_snd.h"
#include "tim_snd.h"

#define __NO_SND	 0
#define __MULTIPLE	 1
#define __CONTINOUS	 2
#define __FROM_SD_START	 3
#define __FROM_SD	 4

#define __SD_LENGTH 1024

uint8_t sound_playing = 0;
uint16_t circle_length = 0;
uint32_t sound_cnt = 0;
uint8_t play_from_SD = 0;
uint8_t stop_sound = 0;
uint16_t play_times = 0;
uint8_t sound_mode = 0;


DMA_InitTypeDef SND_DMA_InitStructure;

#ifdef __STM32F1
	uint8_t  SD_buffer_play[__SD_LENGTH  ];
	uint32_t SD_buffer_read[__SD_LENGTH/4];
	
	uint8_t sound_startup[128];
#endif
#ifdef __STM32F2
	uint16_t  SD_buffer_play[__SD_LENGTH  ];
	uint8_t SD_buffer_read[__SD_LENGTH];
	
	uint16_t sound_startup[128];
#endif

uint8_t sound_flag_readSD = 0;
uint8_t sound_flag_loadL = 0;
uint8_t sound_flag_loadH = 0;

uint32_t sound_SD_addr = 0;
uint32_t sound_SD_pos = 0;
uint32_t sound_SD_cnt = 0;
uint32_t sound_SD_len = 0;

uint8_t sound_buff_posL = 0;
uint8_t sound_buff_posH = 0;

uint8_t current_snd_vol;

void Sound_startup(void) {
	uint16_t i;

	for (i=0; i<128; i++) {
		sound_startup[i] = i;
	}

	Play_from_flash(sound_startup, 128, 1, 0);
}


#ifdef __STM32F1			
	void Play_from_flash(const uint8_t * buffer, uint16_t length, uint16_t times, uint8_t volume) {
#endif
#ifdef __STM32F2
	void Play_from_flash(const uint16_t * buffer, uint16_t length, uint16_t times, uint8_t volume) {
#endif
  sound_playing = 1;
  Set_volume(volume);
  current_snd_vol = volume;
  circle_length = length;
  sound_cnt = 0;
  if (times) {
  	play_times = times;
	sound_mode = __MULTIPLE; 
  } else { 
  	sound_mode = __CONTINOUS;
  }

 
	__DMA_TIM_Channel->CCR &=~DMA_CCR_EN;
  DMA1->IFCR  = __DMA_TIM_GLIF;
  __DMA_TIM_Channel->CNDTR = length;
  __DMA_TIM_Channel->CPAR = __CARRIER_CCR_ADDRESS;
  __DMA_TIM_Channel->CMAR = (uint32_t)buffer;
  __DMA_TIM_Channel->CCR |= DMA_CCR_EN;
	
	__DMA_TIM->CR1 |= TIM_CR1_CEN;
}

#ifdef __USE_SD
void Sound_SD_Loader(void) {
	if (sound_flag_readSD) {
		SD_ReadBlock(SD_buffer_read,		512*sound_SD_pos++,512);	
		SD_ReadBlock(SD_buffer_read+128,512*sound_SD_pos++,512);
		sound_flag_readSD = 0;
	}
}

void Sound_SD_Loader_FS(uint8_t file) {
	if (sound_flag_readSD) {
		SD_ReadBlock(SD_buffer_read,		512*sound_SD_pos++,512);	
		SD_ReadBlock(SD_buffer_read+128,512*sound_SD_pos++,512);
		sound_flag_readSD = 0;
	}
}


uint8_t Play_from_SD(uint32_t Start_addr, uint32_t length, uint8_t volume) {
	if (!sound_playing && (SD_state == __SD_STATE_READY)) {
		sound_SD_pos = Start_addr;
		sound_SD_addr = Start_addr;
		sound_SD_len = length;
	  sound_flag_readSD = 1;
	  
	  sound_playing = 1;
	  Set_volume(volume);
	  current_snd_vol = volume;
	  sound_mode = __FROM_SD_START;
	  
	  TIM_Cmd(__DMA_TIM, ENABLE);
	  return 1;
	} else {
		return 0;
	}
}

void Play_from_SD_launch(void) {
	TIM_Cmd(__DMA_TIM, DISABLE); 
  sound_cnt = 0;

	
	/*
	 
	__DMA_TIM_Channel->CCR &=~DMA_CCR_EN;
  DMA1->IFCR  = __DMA_TIM_GLIF;
  __DMA_TIM_Channel->CNDTR = length;
  __DMA_TIM_Channel->CPAR = __CARRIER_CCR_ADDRESS;
  __DMA_TIM_Channel->CMAR = (uint32_t)buffer;
  __DMA_TIM_Channel->CCR |= DMA_CCR_EN;
	
	__DMA_TIM->CR1 |= TIM_CR1_CEN;
	*/
}
#endif


void Sound_Callback(void) {
#ifdef __STOP_NORMALIZED
uint8_t temp = 0;
#endif
uint32_t snd_temp=0;

	switch (sound_mode) {
		case __NO_SND:

		break;
		case __MULTIPLE:
			if (sound_playing) {
				sound_cnt++;
				if (sound_cnt > circle_length) {
					sound_cnt = 0;
					play_times--; 
					if (!play_times) {
						__DMA_TIM->CR1 &=~TIM_CR1_CEN;
						stop_sound = 0;	
						sound_playing = 0; 
	  					sound_mode = __NO_SND;
					}
				}
		
			}
		break;
		case __CONTINOUS:
			if (sound_playing) {
				sound_cnt++;
				if (sound_cnt > circle_length) sound_cnt = 0; 
				
				#ifdef __STOP_IMMIDIATELY
				if (stop_sound) {
					__DMA_TIM->CR1 &=~TIM_CR1_CEN;
					stop_sound = 0;	
					sound_playing = 0;	
  				sound_mode = __NO_SND;
					sound_cnt = 0; 	 
				}
				#else
				#ifdef __STOP_NORMALIZED
				temp = (uint8_t) (*(__IO uint32_t*) __CARRIER_CCR_ADDRESS);
				if (stop_sound && ((temp == 0x80) || !sound_cnt)) {
					__DMA_TIM->CR1 &=~TIM_CR1_CEN;
					stop_sound = 0;	
					sound_playing = 0;	
  				sound_mode = __NO_SND;
					sound_cnt = 0; 	 
				}
				#else
				if (!sound_cnt && stop_sound) {
					__DMA_TIM->CR1 &=~TIM_CR1_CEN;
					stop_sound = 0;	
					sound_playing = 0;	
  				sound_mode = __NO_SND;
				}
				#endif
				#endif
		
			}
		break;
		#ifdef __USE_SD
		case __FROM_SD_START:
			if (!sound_flag_readSD) {
				for (sound_buff_posL = 0; sound_buff_posL<__SD_LENGTH/8; sound_buff_posL++) {
					snd_temp = SD_buffer_read[sound_buff_posL];
					SD_buffer_play[sound_buff_posL*4 + 0] = (snd_temp)&0xFF;
					SD_buffer_play[sound_buff_posL*4 + 1] = (snd_temp>>8)&0xFF;
					SD_buffer_play[sound_buff_posL*4 + 2] = (snd_temp>>16)&0xFF;
					SD_buffer_play[sound_buff_posL*4 + 3] = (snd_temp>>24)&0xFF;
				}
					
				sound_mode = __FROM_SD; 	
				Play_from_SD_launch();	
			}
		break;
		case __FROM_SD:
			if (sound_playing) {
				if (sound_flag_loadL) {
					snd_temp = SD_buffer_read[sound_buff_posL];
					SD_buffer_play[sound_buff_posL*4 + 0] = (snd_temp)&0xFF;
					SD_buffer_play[sound_buff_posL*4 + 1] = (snd_temp>>8)&0xFF;
					SD_buffer_play[sound_buff_posL*4 + 2] = (snd_temp>>16)&0xFF;
					SD_buffer_play[sound_buff_posL*4 + 3] = (snd_temp>>24)&0xFF;
					sound_buff_posL++;
					if (sound_buff_posL == __SD_LENGTH/8) {
						sound_buff_posL = 0;
						sound_flag_loadL = 0;
					}
				}
				 
				if (sound_flag_loadH) {	
					snd_temp = SD_buffer_read[sound_buff_posH+__SD_LENGTH/8];
					SD_buffer_play[__SD_LENGTH/2 + sound_buff_posH*4 + 0] = (snd_temp)&0xFF;
					SD_buffer_play[__SD_LENGTH/2 + sound_buff_posH*4 + 1] = (snd_temp>>8)&0xFF;
					SD_buffer_play[__SD_LENGTH/2 + sound_buff_posH*4 + 2] = (snd_temp>>16)&0xFF;
					SD_buffer_play[__SD_LENGTH/2 + sound_buff_posH*4 + 3] = (snd_temp>>24)&0xFF;
					sound_buff_posH++;
					if (sound_buff_posH == __SD_LENGTH/8) {
						sound_buff_posH = 0;
						sound_flag_loadH = 0;
						sound_flag_readSD = 1;
					}
				}
				
				sound_cnt++;
				if (sound_cnt == 512) {
					sound_flag_loadL = 1;	
					sound_SD_cnt++;
				}
				
				if (sound_cnt == 1024) {
					sound_flag_loadH = 1;
					sound_cnt = 0;
					sound_SD_cnt++;

					if (stop_sound || (sound_SD_cnt >= sound_SD_len)) {
						__DMA_TIM->CR1 &=~TIM_CR1_CEN;
						stop_sound = 0;	
						sound_playing = 0; 
	  					sound_mode = __NO_SND;	
					}
				}
			}
		break;
		#endif
	}
	
}
