#include "main.h"
#include "efa_can.h"
#include "efa_can_bsp.h"
#include "stdint.h"
#include "stm32l4xx_hal.h"

/*
 *  Identifier format
 *  \HB - Type  - id
 *   _    _ _ _   _ _ _ _ _ _ _
 *
 *  Types:
 *  0 x x x self_id  = heartbeat
 *  1 0 0 0   id     = command to id
 *  1 0 0 1 self_id  = confirm from id
 *  1 0 1 0   id     = request to id
 *  1 0 1 1 self_id  = reply from id
 *  1 1 0 0 self_id  = cast from id
 *  1 1 0 1   id     = settings to id
 *  1 1 1 1   id     = reserved        ---- should we need an error type? or use this one to solve confirm conflict
 *
 * 	7 bit for ID
 *
 *	id format
 *
 *	 APP  - \INT|EXT - MODULE
 *	_ _ _      _       _ _ _ 
 *
 *  0 CAM
 *	1 LED
 *	2 INS
 *	3 ESU
 *	4 GIST
 *	5 AQUA
 *	6 EVAC //maybe aqua+evac+gas should be all marked as assist blocks to get more app types
 *	7 GAS
 *
 *
 */
#define __CAN_TYPE_CAST			0x600
#define __CAN_TYPE_COMMAND		0x400
#define __CAN_TYPE_CONFIRM		0x480
#define __CAN_TYPE_REQUEST		0x500
#define __CAN_TYPE_REPLY		0x580
#define __CAN_TYPE_SETTINGS		0x680

#define __GET_SENDER_ID(XXX) (XXX&0x7F)

#define __WDT_VALUE	1000
#define __CAN_RETRY_TIMEOUT	3

#define __HB_PERIOD	800
#define __HB_LED	100

void EFA_CAN_Cast(uint8_t what);
void EFA_CAN_Reply(uint8_t what);
void EFA_CAN_Confirm(uint8_t what);

#define __CAN_OK (!(__CAN_INSTANCE.Instance->ESR&CAN_ESR_TEC))

#define EFA_CAN_Send_Heartbeat() __CAN_INSTANCE.Instance->sTxMailBox[2].TIR |= CAN_TI2R_TXRQ


#ifndef __CAN_ACCEPT_CASTS 
	#error "CAN_ACCEPT_CASTS is undefined"
#endif	

uint16_t hb_cnt;

uint32_t CAN_Online_Status;

fCommand fCommands[__CAN_MAX_COMMANDS];

uint16_t  CAN_Online_WDT   [__CAN_MAX_SUBSCRIPTIONS];
uint8_t  CAN_Valid		   [__CAN_MAX_SUBSCRIPTIONS];
uint32_t CAN_Subs_Cast_Dict[__CAN_MAX_SUBSCRIPTIONS][__CAN_CAST_DICTIONARY_SIZE];
uint32_t CAN_Subs_Std_Dict [__CAN_MAX_SUBSCRIPTIONS][__CAN_STD_DICTIONARY_SIZE];
uint8_t  CAN_Subscription  [__CAN_MAX_SUBSCRIPTIONS];

uint32_t CAN_Request_Queue[__CAN_MAX_SUBSCRIPTIONS];
uint32_t CAN_Command_Queue[__CAN_MAX_SUBSCRIPTIONS];
uint32_t CAN_Setting_Queue[__CAN_MAX_SUBSCRIPTIONS];
uint8_t CAN_Request_Timeout;
uint8_t CAN_Command_Timeout;
uint8_t CAN_Setting_Timeout;

uint32_t CAN_Own_Cast_Dict [__CAN_CAST_DICTIONARY_SIZE];
uint32_t CAN_Settings_Dict [__CAN_MAX_SETTINGS];
uint32_t CAN_Own_Std_Dict  [__CAN_STD_DICTIONARY_SIZE];

/*
CanTxMsgTypeDef txMsg;
CanRxMsgTypeDef rxMsg;

CanTxMsgTypeDef EFA_CAN_Command;
CanTxMsgTypeDef EFA_CAN_Request;
CanTxMsgTypeDef EFA_CAN_Heartbeat;
CanTxMsgTypeDef EFA_CAN_DataOut;
CanTxMsgTypeDef EFA_CAN_SystemOut;

CanRxMsgTypeDef EFA_CAN_Confirm;
CanRxMsgTypeDef EFA_CAN_DataIn;
CanRxMsgTypeDef EFA_CAN_SystemIn;
*/
CAN_FilterConfTypeDef  sFilterConfig;

static __inline int8_t get_SubscriptionIdx(uint8_t id) {
	int8_t i;
	
	for (i=0;i<__CAN_MAX_SUBSCRIPTIONS;i++) {
		if (id == CAN_Subscription[i]) return i;
	}
	
	return -1;
}

void EFA_CAN_Init(void) {
//	__CAN_INSTANCE.pTxMsg = &txMsg;
//	__CAN_INSTANCE.pRxMsg = &rxMsg;
	
	__CAN_INSTANCE.Instance->FMR |= CAN_FMR_FINIT;
	__CAN_INSTANCE.Instance->FM1R = CAN_FM1R_FBM2;
	__CAN_INSTANCE.Instance->FS1R = 0;
	__CAN_INSTANCE.Instance->FFA1R = 0;
	
	// id | mask<<16
	// id = stdid + rtr + ide + exid[17:15]
	__CAN_INSTANCE.Instance->sFilterRegister[0].FR1 = 0x0000 | ((uint32_t)0x8018<<16);             // 0:heartbeat
	__CAN_INSTANCE.Instance->sFilterRegister[0].FR2 = __CAN_TYPE_CAST<<5 | ((uint32_t)0xF018<<16);             // 1:cast
	__CAN_INSTANCE.Instance->sFilterRegister[1].FR1 = __CAN_TYPE_REPLY<<5 | ((uint32_t)0xF018<<16);             // 2:reply
	__CAN_INSTANCE.Instance->sFilterRegister[1].FR2 = __CAN_TYPE_CONFIRM<<5 | ((uint32_t)0xF018<<16);             // 3:confirm
	__CAN_INSTANCE.Instance->sFilterRegister[2].FR1 =((__CAN_TYPE_COMMAND+__CAN_DEVICE_ID)<<5) |     // 4:command
			                              	        ((((uint32_t)__CAN_TYPE_REQUEST+__CAN_DEVICE_ID)<<5)<<16); // 5:request
	__CAN_INSTANCE.Instance->sFilterRegister[2].FR2 = ((__CAN_TYPE_SETTINGS+__CAN_DEVICE_ID)<<5);       // 6:setting
	
	__CAN_INSTANCE.Instance->FA1R = CAN_FA1R_FACT0|(CAN_FA1R_FACT1*__CAN_ACCEPT_CASTS)|CAN_FA1R_FACT2|CAN_FA1R_FACT3|CAN_FA1R_FACT4|CAN_FA1R_FACT5|CAN_FA1R_FACT6;
	__CAN_INSTANCE.Instance->FMR &= ~CAN_FMR_FINIT;

	__CAN_INSTANCE.Instance->IER |= CAN_IER_FMPIE0;
	
	//init 3rd transmit buffer as heartbeat
	__CAN_INSTANCE.Instance->sTxMailBox[2].TIR = ((uint32_t)__CAN_DEVICE_ID<<21);
	__CAN_INSTANCE.Instance->sTxMailBox[2].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);

}


void EFA_CAN_Rx_Handler(void) {
	uint16_t tmp_id;
	uint8_t pending;
	int8_t n;
	//read FMP
	pending = __CAN_INSTANCE.Instance->RF0R & CAN_RF0R_FMP0;

	while (pending) {
	//check which filter number worked
		tmp_id = (__CAN_INSTANCE.Instance->sFIFOMailBox[0].RDTR & CAN_RDT0R_FMI) >> CAN_RDT0R_FMI_Pos;
		switch (tmp_id) {
		//execute commands
		case 0: //heartbeat
			n = get_SubscriptionIdx(__GET_SENDER_ID(__CAN_INSTANCE.Instance->sFIFOMailBox[0].RIR>>21));
			if (n != -1) {
				CAN_Online_WDT[n] = __WDT_VALUE;
			}
			break;
		case 1: //cast -- check if we are subscribed for this ID
			n = get_SubscriptionIdx(__GET_SENDER_ID(__CAN_INSTANCE.Instance->sFIFOMailBox[0].RIR>>21));
			if (n != -1) {
				CAN_Subs_Cast_Dict[n][(__CAN_INSTANCE.Instance->sFIFOMailBox[0].RDHR)&0xFF] = __CAN_INSTANCE.Instance->sFIFOMailBox[0].RDLR;
			}
			break;
		case 2: //reply -- check if we are waiting reply from this ID
			n = get_SubscriptionIdx(__GET_SENDER_ID(__CAN_INSTANCE.Instance->sFIFOMailBox[0].RIR>>21));
			if (n != -1) {
				tmp_id = __CAN_INSTANCE.Instance->sFIFOMailBox[0].RDHR&0xFFFF;
				CAN_Request_Queue[n] &=~(1<<tmp_id);
				CAN_Subs_Std_Dict[n][tmp_id] = __CAN_INSTANCE.Instance->sFIFOMailBox[0].RDLR;
			}
			break;
		case 3: //confirm -- check if we are waiting confirmation from this ID
			n = get_SubscriptionIdx(__GET_SENDER_ID(__CAN_INSTANCE.Instance->sFIFOMailBox[0].RIR>>21));
			if (n != -1) {
				tmp_id = __CAN_INSTANCE.Instance->sFIFOMailBox[0].RDLR&0xFFFF;
				CAN_Command_Queue[n] &=~(1<<tmp_id);
				CAN_Setting_Queue[n] &=~(1<<tmp_id); //that's wrong but no ideas how to solve this right now
			}
			break;
		case 4: //command -- this is our command - execute&confirm
			tmp_id = __CAN_INSTANCE.Instance->sFIFOMailBox[0].RDLR&0xFFFF;
			if (tmp_id < __CAN_MAX_COMMANDS) {
				if (fCommands[tmp_id]) (*fCommands[tmp_id])();
				EFA_CAN_Confirm(tmp_id);
			}
			break;
		case 5: //request -- this is our request - send reply
			tmp_id = __CAN_INSTANCE.Instance->sFIFOMailBox[0].RDLR&0xFFFF;
			if (tmp_id < __CAN_STD_DICTIONARY_SIZE) {
				EFA_CAN_Reply(tmp_id);
			}
			break;
		case 6: //setting -- this is our settings - send confirmation and apply
			tmp_id = __CAN_INSTANCE.Instance->sFIFOMailBox[0].RDHR&0xFFFF;
			if  (tmp_id < __CAN_MAX_SETTINGS) {
				CAN_Settings_Dict[tmp_id] = __CAN_INSTANCE.Instance->sFIFOMailBox[0].RDLR;
				EFA_CAN_Confirm(tmp_id);
			}
			break;
		default: //just free buffer - unknown message format
			break;
		}

	//release FIFO
		__CAN_INSTANCE.Instance->RF0R |= CAN_RF0R_RFOM0;
	//if FMP read more
		pending = __CAN_INSTANCE.Instance->RF0R & CAN_RF0R_FMP0;
	}
}

void EFA_CAN_Periodic_Handler(void) {
	uint8_t i,j;
	
	//LED activity
	if (hb_cnt >= __HB_PERIOD) {
		hb_cnt = 0;
		EFA_CAN_Send_Heartbeat();
	}
	if (__CAN_OK) {
		if (hb_cnt < __HB_LED) __CAN_LED_ON(); else __CAN_LED_OFF();
	} else {
		__CAN_LED_OFF();
	}
	hb_cnt++;
	
	//CAN validity timeout
	for (i=0;i<__CAN_MAX_SUBSCRIPTIONS;i++) {
		if (CAN_Online_WDT[i]) {
			CAN_Valid[i] = 1;
			CAN_Online_WDT[i]--; 
		} else {
			CAN_Valid[i] = 0;
		}			
	}
	
	//CAN cast time-triggered params
	//????
	
	//CAN retry commands and requests
	if (!--CAN_Command_Timeout) {
		for (j=0;j<__CAN_MAX_SUBSCRIPTIONS;j++) {
			if (CAN_Command_Queue[j]) {
				for (i=0;i<__CAN_MAX_COMMANDS;i++) {
					if (CAN_Command_Queue[j]&(1<<i)) {
						EFA_CAN_Command(i,CAN_Subscription[j]);
						return;
					}
				}
			}
		}
	}
}

void EFA_CAN_Cast(uint8_t what) {
	if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME0) {
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR = ((uint32_t)(__CAN_TYPE_CAST|__CAN_DEVICE_ID)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR |= 5;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDLR = CAN_Own_Cast_Dict[what];
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDHR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	} else if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME1) {
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR = ((uint32_t)(__CAN_TYPE_CAST|__CAN_DEVICE_ID)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR |= 5;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDLR = CAN_Own_Cast_Dict[what];
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDHR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
	} else {
		//all is lost -- no empty buffers
	}
}

void EFA_CAN_Reply(uint8_t what) {
	if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME0) {
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR = ((uint32_t)(__CAN_TYPE_REPLY|__CAN_DEVICE_ID)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR |= 5;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDLR = CAN_Own_Std_Dict[what];
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDHR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	} else if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME1) {
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR = ((uint32_t)(__CAN_TYPE_REPLY|__CAN_DEVICE_ID)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR |= 5;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDLR = CAN_Own_Std_Dict[what];
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDHR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
	} else {
		//all is lost -- no empty buffers
	}
}

void EFA_CAN_Confirm(uint8_t what) {
	if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME0) {
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR = ((uint32_t)(__CAN_TYPE_CONFIRM|__CAN_DEVICE_ID)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR |= 1;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDLR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	} else if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME1) {
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR = ((uint32_t)(__CAN_TYPE_CONFIRM|__CAN_DEVICE_ID)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR |= 1;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDLR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
	} else {
		//all is lost -- no empty buffers
	}
}

void EFA_CAN_Setting(uint8_t what, uint32_t value, uint8_t whom) {
	int8_t n;
	if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME0) {
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR = ((uint32_t)(__CAN_TYPE_SETTINGS|whom)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR |= 5;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDLR = value;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDHR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	} else if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME1) {
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR = ((uint32_t)(__CAN_TYPE_SETTINGS|whom)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR |= 5;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDLR = value;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDHR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
	} else {
		//all is lost -- no empty buffers
	}
	n = get_SubscriptionIdx(whom);
	if (n != -1) CAN_Setting_Queue[n] |= (1<<what);
	CAN_Setting_Timeout = __CAN_RETRY_TIMEOUT;
}

void EFA_CAN_Request(uint8_t what, uint8_t whom) {
	int8_t n;
	if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME0) {
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR = ((uint32_t)(__CAN_TYPE_REQUEST|whom)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR |= 1;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDLR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	} else if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME1) {
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR = ((uint32_t)(__CAN_TYPE_REQUEST|whom)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR |= 1;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDLR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
	} else {
		//all is lost -- no empty buffers
	}
	n = get_SubscriptionIdx(whom);
	if (n != -1) CAN_Request_Queue[n] |= (1<<what);
	CAN_Request_Timeout = __CAN_RETRY_TIMEOUT;
}

void EFA_CAN_Command(uint8_t what, uint8_t whom) {
	int8_t n;
	if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME0) {
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR = ((uint32_t)(__CAN_TYPE_COMMAND|whom)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDTR |= 1;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TDLR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	} else if (__CAN_INSTANCE.Instance->TSR & CAN_TSR_TME1) {
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR = ((uint32_t)(__CAN_TYPE_COMMAND|whom)<<21);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR &= ~((uint32_t)CAN_TDT0R_DLC);
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDTR |= 1;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TDLR = what;
		__CAN_INSTANCE.Instance->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ;
	} else {
		//all is lost -- no empty buffers
	}
	n = get_SubscriptionIdx(whom);
	if (n != -1) CAN_Command_Queue[n] |= (1<<what);
	CAN_Command_Timeout = __CAN_RETRY_TIMEOUT;
}



void EFA_CAN_DataGet(void);
void EFA_CAN_DataSet(void);
//void EFA_CAN_
