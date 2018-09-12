#define __BUTTON_UP()	   sound_flags |= (1<<0)
#define __BUTTON_U()	   sound_flags |= (1<<1)
#define __BUTTON()		   sound_flags |= (1<<2)
#define __BUTTON_D()	   sound_flags |= (1<<3) 
#define __BUTTON_DN()	   sound_flags |= (1<<4) 
#define __BUTTON_ENTER()   sound_flags |= (1<<5)
#define __BUTTON_LOCKED()  sound_flags |= (1<<6) 

void Sound_IT_Handler(void);

extern uint16_t sound_flags;
