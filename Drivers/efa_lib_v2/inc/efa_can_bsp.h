#if __ENDOIRR
#define __CAN_DEVICE_ID 0x40 //7 bit
#else
#define __CAN_DEVICE_ID 0x70 //7 bit
#endif

#define __CAN_ACCEPT_CASTS	1
#define __CAN_INSTANCE	hcan

#define __CAN_MAX_SETTINGS	10
#define __CAN_MAX_COMMANDS	10

//define subscription name-idx ?
//#define 

#define __CAN_LED_ON()		//__LED_LINK_ON()
#define __CAN_LED_OFF()		//__LED_LINK_OFF()


