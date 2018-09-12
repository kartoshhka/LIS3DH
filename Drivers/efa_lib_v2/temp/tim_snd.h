#include "stdint.h"
#define __STM32F1
extern uint8_t sound_playing;
extern uint8_t stop_sound;
extern uint8_t sound_mode;

#define Stop_sound() stop_sound = 1	   

void Sound_startup(void);									
#ifdef __STM32F1			
	void Play_from_flash(const uint8_t * buffer, uint16_t length, uint16_t times, uint8_t volume); 
#endif
#ifdef __STM32F2
	void Play_from_flash(const uint16_t * buffer, uint16_t length, uint16_t times, uint8_t volume);
#endif
uint8_t Play_from_SD(uint32_t Start_addr, uint32_t length, uint8_t volume);
void Sound_Callback(void);
void Sound_SD_Loader(void);



