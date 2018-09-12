/* platform */

#define _FRAM_SS_ON()  GPIOA->BRR = GPIO_Pin_11
#define _FRAM_SS_OFF() GPIOA->BSRR = GPIO_Pin_11

/* SPI1 
#define _FRAM_DMA_RC DMA1_Channel2
#define _FRAM_DMA_TX DMA1_Channel3
#define _FRAM_SPI	SPI1
#define _FRAM_SPI_DR_ADDR 0x4001300C			  
*/
/* SPI2 */
#define _FRAM_DMA_RC DMA1_Channel4
#define _FRAM_DMA_TX DMA1_Channel5
#define _FRAM_SPI	SPI2
#define _FRAM_SPI_DR_ADDR 0x4000380C


/* MEMORY MAP */
#define __ADDR_SETTINGS		  0
#define __ADDR_PROG_OFFSET	  4
#define __ADDR_PROG_LEN		  25
#define __ADDR_FLOW_ARG		  254  
#define __ADDR_FLOW_ON		  257
#define __ADDR_OB_FAIL		  267
#define __ADDR_MEMTEST		  268 
#define __ADDR_MEMSEC		  269
#define __ADDR_CALIB_DONE	  270
#define __ADDR_BRIGHTNESS		280
#define __ADDR_STATISTICS	  300



