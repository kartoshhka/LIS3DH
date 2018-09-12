/* Op Codes */
#define __WREN  0x06
#define __WRDI  0x04
#define __RDSR  0x05
#define __WRSR  0x01
#define __READ  0x03
#define __FSTRD 0x0B
#define __WRITE 0x02
#define __SLEEP 0xB9
#define __RDID  0x3F
#define __SNR   0xC3	

#define __MAX_ADDR 0x7FFF


#ifndef __FRAM_MALF_WDT
	#define __FRAM_MALF_WDT  		0xFFFF
#endif

extern uint16_t fram_err;
extern uint8_t fram_req;
extern uint32_t fram_wdt;
extern uint8_t fram_malf;

/* Function prototypes */
void MEM_SysRead(uint16_t addr, uint16_t len, uint8_t * where);
void MEM_QSysRead(uint16_t addr, uint16_t len, uint8_t * where);
void MEM_Write(uint16_t addr, uint16_t len, uint8_t * what);
void MEM_Read(uint16_t addr, uint16_t len, uint8_t * where);	 
void MEM_QWrite(uint16_t addr, uint16_t len, uint8_t * what);
void MEM_QRead(uint16_t addr, uint16_t len, uint8_t * where);
uint8_t FRAM_Handler(void);
void MEM_Q_Handler(void);

#define MEM_Trigger()	fram_wdt = __FRAM_MALF_WDT; fram_req = 1

#define MEM_Wait()		while(fram_req) {			\
												if (fram_wdt) {			\
													fram_wdt--; 			\
												} else {						\
													fram_malf = 1;		\
													break;						\
												}										\
											}											\

#define MEM_WaitForever()		while(fram_req)	

