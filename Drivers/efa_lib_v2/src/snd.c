#include "stm32f1xx_hal.h"
#include "snd.h"
#include "platform_tim_snd.h"
#include "tim_snd.h"
#include "snd_buttons.h"

uint8_t state_Sound;
uint16_t sound_flags;
uint8_t sound_volume = 5;

void Sound_IT_Handler(void) 
{
	switch (state_Sound) 
	{
		case 0:
			if (sound_flags&(1<<0)) 
			{  
				Stop_sound();
				if (!sound_playing) 
				{
					state_Sound = 1;
					Play_from_flash(snd_top, 6720, 1, sound_volume);
					sound_flags&=~0xFF;
				}
			}
			if (sound_flags&(1<<1)) 
			{
				Stop_sound();
				if (!sound_playing) 
				{
					state_Sound = 1;
					Play_from_flash(snd_up, 2560, 1, sound_volume);
					sound_flags&=~0xFF;
				}
			}
			if (sound_flags&(1<<2)) 
			{	   
				Stop_sound();
				if (!sound_playing) 
				{
					state_Sound = 1;
					Play_from_flash(snd_but, 2578, 1, sound_volume);
					sound_flags&=~0xFF;
				}
			}  
			if (sound_flags&(1<<3)) 
			{	   
				Stop_sound();
				if (!sound_playing) 
				{
					state_Sound = 1;
					Play_from_flash(snd_down, 2579, 1, sound_volume);
					sound_flags&=~0xFF;
				}
			}
			if (sound_flags&(1<<4)) 
			{	   
				Stop_sound();
				if (!sound_playing) 
				{
					state_Sound = 1;
					Play_from_flash(snd_bottom, 6720, 1, sound_volume);
					sound_flags&=~0xFF;
				}
			}
			if (sound_flags&(1<<5)) 
			{	   
				Stop_sound();
				if (!sound_playing) 
				{
					state_Sound = 1;
					Play_from_flash(snd_enter, 12784, 1, sound_volume);
					sound_flags&=~0xFF;
				}
			}
			if (sound_flags&(1<<6)) 
			{	   
				Stop_sound();
				if (!sound_playing) 
				{
					state_Sound = 1;
					Play_from_flash(snd_locked, 7315, 1, sound_volume);
					sound_flags&=~0xFF;
				}
			}  

		break;
		case 1:
			if (!sound_playing)	state_Sound = 0;
		break;
	}
}


