extern uint8_t sound_playing;
extern uint8_t stop_sound;
extern uint8_t sound_mode;

#define Stop_sound() stop_sound = 1	   

void Sound_startup(void);									
			
void Play_from_flash(const uint8_t * buffer, uint16_t length, uint16_t times, uint8_t volume);

uint8_t Play_from_SD(uint32_t Start_addr, uint32_t length, uint8_t volume);
void Sound_Callback(void);
void Sound_SD_Loader(void);



