#include "efa_can_bsp.h"
#include "stdint.h"

#define __CAN_MAX_SUBSCRIPTIONS 4
#define __CAN_CAST_DICTIONARY_SIZE 8
#define __CAN_STD_DICTIONARY_SIZE 16

extern uint8_t  CAN_Valid[__CAN_MAX_SUBSCRIPTIONS];
extern uint32_t CAN_Settings_Dict [__CAN_MAX_SETTINGS];
extern uint8_t  CAN_Subscription  [__CAN_MAX_SUBSCRIPTIONS];
extern uint32_t CAN_Own_Cast_Dict [__CAN_CAST_DICTIONARY_SIZE];
extern uint32_t CAN_Settings_Dict [__CAN_MAX_SETTINGS];
extern uint32_t CAN_Own_Std_Dict  [__CAN_STD_DICTIONARY_SIZE];
extern uint32_t CAN_Subs_Cast_Dict[__CAN_MAX_SUBSCRIPTIONS][__CAN_CAST_DICTIONARY_SIZE];
extern uint32_t CAN_Subs_Std_Dict [__CAN_MAX_SUBSCRIPTIONS][__CAN_STD_DICTIONARY_SIZE];

typedef void(*fCommand)(void);
extern fCommand fCommands[__CAN_MAX_COMMANDS];

void EFA_CAN_Init(void);
void EFA_CAN_Rx_Handler(void);
void EFA_CAN_Periodic_Handler(void);
void EFA_CAN_DataGet(void);
void EFA_CAN_DataSet(void);

void EFA_CAN_Cast(uint8_t what); 
	
void EFA_CAN_Request(uint8_t what, uint8_t whom);
void EFA_CAN_Setting(uint8_t what, uint32_t value, uint8_t whom);
void EFA_CAN_Command(uint8_t what, uint8_t whom);


