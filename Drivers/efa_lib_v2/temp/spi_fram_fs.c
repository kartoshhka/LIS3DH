#ifdef __STM32F1
	#include "stm32f10x.h"
#endif
#ifdef __STM32F2
	#include "stm32f2xx.h"
#endif
#include "platform_spi_fram.h"
#include "spi_fram_fs.h"

void FRAM_Write(uint16_t addr, uint8_t * data, uint16_t length);
void FRAM_Read(uint16_t addr, uint8_t * buffer, uint16_t length);
uint16_t FRAM_Verify(uint16_t address, uint8_t * buffer_from, uint8_t * buffer_to, uint16_t length);
void FRAM_Wr_En(void);


uint16_t fram_err = 0;
uint8_t fram_req = 0;

uint32_t fram_wdt = __FRAM_MALF_WDT;
uint8_t fram_malf = 0;

#define __DMA_SHIFT 3

#define __QUEUE_BUFF_DEPTH 	8
#define __MAX_SPI_BUFFER_SIZE 4096

uint8_t fram_state = 0;
#define __fsIDLE 	0
#define __fsCHECK 1
#define __fsREAD 	2
#define __fsRDV 	3
#define __fsWRITE 4
#define __fsWREN 	5
#define __fsRDS 	6

uint8_t read_buffer[__MAX_SPI_BUFFER_SIZE];
uint8_t fram_buffer[__MAX_SPI_BUFFER_SIZE];
uint8_t temp_buffer[1];	 

uint8_t q_buff_pos = 0;
uint8_t q_last_pos = 0;
uint8_t q_buff_check_pos = 0;

#define __MEM_WRITE 1
#define __MEM_READ	2
#define __MEM_SYSREAD	3

uint8_t  q_buff_type [__QUEUE_BUFF_DEPTH];
uint16_t q_buff_addr [__QUEUE_BUFF_DEPTH];
uint16_t q_buff_len  [__QUEUE_BUFF_DEPTH];
uint8_t *q_buff_ptr  [__QUEUE_BUFF_DEPTH];
uint8_t  q_buff_ready[__QUEUE_BUFF_DEPTH];

uint8_t  queue_type;
uint16_t queue_addr;
uint16_t queue_len;
uint8_t *queue_ptr;

DMA_InitTypeDef FRAMDMA_InitStructure;  
uint16_t fram_i;	  
uint16_t fram_result = 0;	

void MEM_Q_Handler(void) {
														 
}

uint8_t FRAM_Handler(void) {			//return 0 if transaction in progress, 1 if transaction complete
	switch (fram_state) { 
		case __fsCHECK:
			fram_state = __fsIDLE;
		case __fsIDLE:
			if (q_buff_ready[q_buff_check_pos] == 0x55) {
				queue_ptr = q_buff_ptr[q_buff_check_pos];
				queue_addr = q_buff_addr[q_buff_check_pos];
				queue_len = q_buff_len[q_buff_check_pos];
				queue_type = q_buff_type[q_buff_check_pos];
							
				q_buff_ready[q_buff_check_pos] = 0;	
				q_buff_check_pos++;	
				if (q_buff_check_pos >= __QUEUE_BUFF_DEPTH) q_buff_check_pos = 0;
			
				switch (queue_type) {
					case __MEM_WRITE:
						FRAM_Wr_En();
						fram_state = __fsWREN;
						fram_wdt = __FRAM_MALF_WDT;
					return 0;
					case __MEM_READ:
						FRAM_Read(queue_addr*2,read_buffer,queue_len*2);
						fram_state = __fsRDV;	
						fram_wdt = __FRAM_MALF_WDT;
					return 0;	
					case __MEM_SYSREAD:
						FRAM_Read(queue_addr,queue_ptr,queue_len);
						fram_state = __fsRDS;	
						fram_wdt = __FRAM_MALF_WDT;
					return 0;					 
				} 
			} else {
				fram_req = 0;
			}
		return 1;	  
		case __fsREAD:
			if ((!(_FRAM_SPI->SR&SPI_I2S_FLAG_BSY)) && (_FRAM_SPI->SR&SPI_I2S_FLAG_TXE)) {
				_FRAM_SS_OFF();
				fram_state = __fsCHECK;
				return 0;
			}
		break;
		case __fsRDV:
			if ((!(_FRAM_SPI->SR&SPI_I2S_FLAG_BSY)) && (_FRAM_SPI->SR&SPI_I2S_FLAG_TXE)) {
				_FRAM_SS_OFF();
				fram_err += FRAM_Verify(queue_addr,read_buffer,queue_ptr,queue_len);
				fram_state = __fsCHECK; 
				return 0;
			}
		break;
		case __fsRDS:
			if ((!(_FRAM_SPI->SR&SPI_I2S_FLAG_BSY)) && (_FRAM_SPI->SR&SPI_I2S_FLAG_TXE)) {
				_FRAM_SS_OFF();
				fram_state = __fsCHECK;	
				return 0;
			}
		break;
		case __fsWRITE:
			if ((!(_FRAM_SPI->SR&SPI_I2S_FLAG_BSY)) && (_FRAM_SPI->SR&SPI_I2S_FLAG_TXE)) {
				_FRAM_SS_OFF();
				fram_state = __fsCHECK;	
				return 0;
			}
		break;
		case __fsWREN:
			if ((!(_FRAM_SPI->SR&SPI_I2S_FLAG_BSY)) && (_FRAM_SPI->SR&SPI_I2S_FLAG_TXE)) {
				_FRAM_SS_OFF();
				FRAM_Write(queue_addr,queue_ptr,queue_len);
				fram_state = __fsWRITE;
				return 0;	
			}
		break;
		default:
			fram_state = __fsIDLE;
		return 1;
	}
	return 0;
}

void MEM_Write(uint16_t addr, uint16_t len, uint8_t * what) {
	q_buff_ptr  [q_buff_pos] = what;	
	q_buff_len  [q_buff_pos] = len;	
	q_buff_addr [q_buff_pos] = addr;	
	q_buff_type [q_buff_pos] = __MEM_WRITE;	
	q_buff_ready[q_buff_pos] = 0x55;	//ready
	
	q_buff_pos++;
	if (q_buff_pos==__QUEUE_BUFF_DEPTH) q_buff_pos = 0; 
			
	fram_wdt = __FRAM_MALF_WDT;	
	fram_req = 1;	
}

void MEM_QWrite(uint16_t addr, uint16_t len, uint8_t * what) {
	q_buff_ptr  [q_buff_pos] = what;	
	q_buff_len  [q_buff_pos] = len;	
	q_buff_addr [q_buff_pos] = addr;	
	q_buff_type [q_buff_pos] = __MEM_WRITE;	
	q_buff_ready[q_buff_pos] = 0x55;	//ready
	
	q_buff_pos++;
	if (q_buff_pos==__QUEUE_BUFF_DEPTH) q_buff_pos = 0; 
}

void MEM_Read(uint16_t addr, uint16_t len, uint8_t * where) {
	q_buff_ptr  [q_buff_pos] = where;	
	q_buff_len  [q_buff_pos] = len;	
	q_buff_addr [q_buff_pos] = addr;	
	q_buff_type [q_buff_pos] = __MEM_READ;	
	q_buff_ready[q_buff_pos] = 0x55;	//ready
	
	q_buff_pos++;
	if (q_buff_pos==__QUEUE_BUFF_DEPTH) q_buff_pos = 0; 
	
	fram_wdt = __FRAM_MALF_WDT;
	fram_req = 1;	
}	 

void MEM_QRead(uint16_t addr, uint16_t len, uint8_t * where) {
	q_buff_ptr  [q_buff_pos] = where;	
	q_buff_len  [q_buff_pos] = len;	
	q_buff_addr [q_buff_pos] = addr;	
	q_buff_type [q_buff_pos] = __MEM_READ;	
	q_buff_ready[q_buff_pos] = 0x55;	//ready
	
	q_buff_pos++;
	if (q_buff_pos==__QUEUE_BUFF_DEPTH) q_buff_pos = 0; 
}

void MEM_SysRead(uint16_t addr, uint16_t len, uint8_t * where) {
	q_buff_ptr  [q_buff_pos] = where;	
	q_buff_len  [q_buff_pos] = len;	
	q_buff_addr [q_buff_pos] = addr;	
	q_buff_type [q_buff_pos] = __MEM_SYSREAD;	
	q_buff_ready[q_buff_pos] = 0x55;	//ready
	
	q_buff_pos++;
	if (q_buff_pos==__QUEUE_BUFF_DEPTH) q_buff_pos = 0;
		
	fram_wdt = __FRAM_MALF_WDT;
	fram_req = 1;
}

void MEM_QSysRead(uint16_t addr, uint16_t len, uint8_t * where) {
	q_buff_ptr  [q_buff_pos] = where;	
	q_buff_len  [q_buff_pos] = len;	
	q_buff_addr [q_buff_pos] = addr;	
	q_buff_type [q_buff_pos] = __MEM_SYSREAD;	
	q_buff_ready[q_buff_pos] = 0x55;	//ready
	
	q_buff_pos++;
	if (q_buff_pos==__QUEUE_BUFF_DEPTH) q_buff_pos = 0;
}

void FRAM_Write(uint16_t addr, uint8_t * data, uint16_t length) { 
  fram_buffer[0] = __WRITE;
  fram_buffer[1] = (addr*2)>>8;
  fram_buffer[2] = (addr*2)&0xFF;
  for (fram_i=0; fram_i<length; fram_i++) {
  	fram_buffer[3+fram_i*2]   =  data[fram_i];	
  	fram_buffer[3+fram_i*2+1] = ~data[fram_i];	
  }
  
  #ifdef __STM32F1
	  DMA_DeInit(_FRAM_DMA_RC);
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)read_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	  FRAMDMA_InitStructure.DMA_BufferSize = length*2+3;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	  FRAMDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(_FRAM_DMA_RC, &FRAMDMA_InitStructure);
	  DMA_Cmd(_FRAM_DMA_RC, ENABLE);
  #endif
  #ifdef __STM32F2
	  DMA_DeInit(__FRAM_Stream_Rx);
	  FRAMDMA_InitStructure.DMA_Channel = __FRAM_Channel_Rx;  
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)read_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  FRAMDMA_InitStructure.DMA_BufferSize = length*2+3;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	  FRAMDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  FRAMDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	  FRAMDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  FRAMDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(__FRAM_Stream_Rx, &FRAMDMA_InitStructure);
	  DMA_Cmd(__FRAM_Stream_Rx, ENABLE);
  #endif
  
  #ifdef __STM32F1	  
	  DMA_DeInit(_FRAM_DMA_TX);
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)fram_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	  FRAMDMA_InitStructure.DMA_BufferSize = length*2+3;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  FRAMDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  SPI_I2S_DMACmd(_FRAM_SPI, SPI_I2S_DMAReq_Rx|SPI_I2S_DMAReq_Tx, ENABLE);
	  DMA_Init(_FRAM_DMA_TX, &FRAMDMA_InitStructure); 
	#endif
  #ifdef __STM32F2	  
	  DMA_DeInit(__FRAM_Stream_Tx);
	  FRAMDMA_InitStructure.DMA_Channel = __FRAM_Channel_Tx;  
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)fram_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	  FRAMDMA_InitStructure.DMA_BufferSize = length*2+3;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  FRAMDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  FRAMDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	  FRAMDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  FRAMDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  SPI_I2S_DMACmd(_FRAM_SPI, SPI_I2S_DMAReq_Rx|SPI_I2S_DMAReq_Tx, ENABLE);
	  DMA_Init(__FRAM_Stream_Tx, &FRAMDMA_InitStructure); 
	#endif
	
  _FRAM_SS_ON();
	
	#ifdef __STM32F1
  	DMA_Cmd(_FRAM_DMA_TX, ENABLE);
  #endif
  #ifdef __STM32F2
  	DMA_Cmd(__FRAM_Stream_Tx, ENABLE);
  #endif
}

void FRAM_Read(uint16_t addr, uint8_t * buffer, uint16_t length) { 
  fram_buffer[0] = __READ;
  fram_buffer[1] = addr>>8;
  fram_buffer[2] = addr&0xFF;

	#ifdef __STM32F1
		DMA_DeInit(_FRAM_DMA_RC);
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	  FRAMDMA_InitStructure.DMA_BufferSize = length+3;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	  FRAMDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(_FRAM_DMA_RC, &FRAMDMA_InitStructure);
	  DMA_Cmd(_FRAM_DMA_RC, ENABLE);
	#endif
	#ifdef __STM32F2
	  DMA_DeInit(__FRAM_Stream_Rx);
	  FRAMDMA_InitStructure.DMA_Channel = __FRAM_Channel_Rx;  
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  FRAMDMA_InitStructure.DMA_BufferSize = length+3;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	  FRAMDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  FRAMDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	  FRAMDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  FRAMDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(__FRAM_Stream_Rx, &FRAMDMA_InitStructure);
	  DMA_Cmd(__FRAM_Stream_Rx, ENABLE);
  #endif
  
  #ifdef __STM32F1
		DMA_DeInit(_FRAM_DMA_TX);	
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)fram_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	  FRAMDMA_InitStructure.DMA_BufferSize = length+3;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  FRAMDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  SPI_I2S_DMACmd(_FRAM_SPI, SPI_I2S_DMAReq_Rx|SPI_I2S_DMAReq_Tx, ENABLE);
	  DMA_Init(_FRAM_DMA_TX, &FRAMDMA_InitStructure); 
	#endif
  #ifdef __STM32F2	  
	  DMA_DeInit(__FRAM_Stream_Tx);
	  FRAMDMA_InitStructure.DMA_Channel = __FRAM_Channel_Tx;  
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)fram_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	  FRAMDMA_InitStructure.DMA_BufferSize = length+3;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  FRAMDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  FRAMDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	  FRAMDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  FRAMDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  SPI_I2S_DMACmd(_FRAM_SPI, SPI_I2S_DMAReq_Rx|SPI_I2S_DMAReq_Tx, ENABLE);
	  DMA_Init(__FRAM_Stream_Tx, &FRAMDMA_InitStructure); 
  #endif

  _FRAM_SS_ON();
  
  #ifdef __STM32F1
		DMA_Cmd(_FRAM_DMA_TX, ENABLE);
	#endif
	#ifdef __STM32F2
  	DMA_Cmd(__FRAM_Stream_Tx, ENABLE);
  #endif
}


uint16_t FRAM_Verify(uint16_t address, uint8_t * buffer_from, uint8_t * buffer_to, uint16_t length) {
	fram_result = 0;
  for (fram_i=0; fram_i<length; fram_i++) {
  	if ((buffer_from[__DMA_SHIFT+fram_i*2]^buffer_from[__DMA_SHIFT+fram_i*2+1]) == 0xFF) {
  		buffer_to[fram_i] = buffer_from[__DMA_SHIFT+fram_i*2];
  	}	else {
  		fram_result++;  
			buffer_to[fram_i] = buffer_from[__DMA_SHIFT+fram_i*2];//0; //if wrong data - return 0
  	}
  }
  return fram_result;
}

void FRAM_Wr_En(void) {				 
  temp_buffer[0] = __WREN;

	#ifdef __STM32F1
		DMA_DeInit(_FRAM_DMA_RC);
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)read_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	  FRAMDMA_InitStructure.DMA_BufferSize = 1;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	  FRAMDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(_FRAM_DMA_RC, &FRAMDMA_InitStructure);
	  DMA_Cmd(_FRAM_DMA_RC, ENABLE);
	#endif
	#ifdef __STM32F2
	  DMA_DeInit(__FRAM_Stream_Rx);
	  FRAMDMA_InitStructure.DMA_Channel = __FRAM_Channel_Rx;  
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)read_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  FRAMDMA_InitStructure.DMA_BufferSize = 1;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	  FRAMDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  FRAMDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	  FRAMDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  FRAMDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(__FRAM_Stream_Rx, &FRAMDMA_InitStructure);
	  DMA_Cmd(__FRAM_Stream_Rx, ENABLE);
  #endif
  
  #ifdef __STM32F1
		DMA_DeInit(_FRAM_DMA_TX);	
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)temp_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	  FRAMDMA_InitStructure.DMA_BufferSize = 1;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  FRAMDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  SPI_I2S_DMACmd(_FRAM_SPI, SPI_I2S_DMAReq_Rx|SPI_I2S_DMAReq_Tx, ENABLE);
	  DMA_Init(_FRAM_DMA_TX, &FRAMDMA_InitStructure); 
	#endif
  #ifdef __STM32F2	  
	  DMA_DeInit(__FRAM_Stream_Tx);
	  FRAMDMA_InitStructure.DMA_Channel = __FRAM_Channel_Tx;  
	  FRAMDMA_InitStructure.DMA_PeripheralBaseAddr = _FRAM_SPI_DR_ADDR;
	  FRAMDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)temp_buffer;
	  FRAMDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	  FRAMDMA_InitStructure.DMA_BufferSize = 1;
	  FRAMDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  FRAMDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  FRAMDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  FRAMDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  FRAMDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  FRAMDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  FRAMDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	  FRAMDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  FRAMDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  SPI_I2S_DMACmd(_FRAM_SPI, SPI_I2S_DMAReq_Rx|SPI_I2S_DMAReq_Tx, ENABLE);
	  DMA_Init(__FRAM_Stream_Tx, &FRAMDMA_InitStructure); 
	#endif
	
  _FRAM_SS_ON();
  
  #ifdef __STM32F1
		DMA_Cmd(_FRAM_DMA_TX, ENABLE);
	#endif
	#ifdef __STM32F2
  	DMA_Cmd(__FRAM_Stream_Tx, ENABLE);
  #endif
}



