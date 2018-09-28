#ifndef ACCEL_H_
#define ACCEL_H_

#include "stm32l4xx_hal.h"

#define __ACCEL_ADDR_WRITE							0x30
#define __ACCEL_ADDR_READ								0x31
#define __ACCEL_REG_ID_ADDR 						0x0F
#define __ACCEL_ID 											0x33

#define __ACCEL_NORMAL_MODE     				((uint8_t)0x00)
#define __ACCEL_LOWPOWER_MODE   				((uint8_t)0x08)

#define __ACCEL_ODR_1_HZ                ((uint8_t)0x10)  // Output Data Rate = 1 Hz 
#define __ACCEL_ODR_10_HZ               ((uint8_t)0x20)  // Output Data Rate = 10 Hz 
#define __ACCEL_ODR_25_HZ               ((uint8_t)0x30)  // Output Data Rate = 25 Hz 
#define __ACCEL_ODR_50_HZ               ((uint8_t)0x40)  // Output Data Rate = 50 Hz 
#define __ACCEL_ODR_100_HZ              ((uint8_t)0x50)  // Output Data Rate = 100 Hz 
#define __ACCEL_ODR_200_HZ              ((uint8_t)0x60)  // Output Data Rate = 200 Hz 
#define __ACCEL_ODR_400_HZ              ((uint8_t)0x70)  // Output Data Rate = 400 Hz 
#define __ACCEL_ODR_1620_HZ_LP          ((uint8_t)0x80)  // Output Data Rate = 1620 Hz only in Low Power Mode 
#define __ACCEL_ODR_1344_HZ             ((uint8_t)0x90)  // Output Data Rate = 1344 Hz in Normal mode and 5376 Hz

#define __ACCEL_X_ENABLE                ((uint8_t)0x01)
#define __ACCEL_Y_ENABLE                ((uint8_t)0x02)
#define __ACCEL_Z_ENABLE                ((uint8_t)0x04)
#define __ACCEL_AXES_ENABLE             ((uint8_t)0x07)
#define __ACCEL_AXES_DISABLE            ((uint8_t)0x00)

#define __ACCEL_HR_ENABLE               ((uint8_t)0x08)
#define __ACCEL_HR_DISABLE              ((uint8_t)0x00)

#define __ACCEL_FULLSCALE_2G            ((uint8_t)0x00)  // ±2 g 
#define __ACCEL_FULLSCALE_4G            ((uint8_t)0x10)  // ±4 g 
#define __ACCEL_FULLSCALE_8G            ((uint8_t)0x20)  // ±8 g 
#define __ACCEL_FULLSCALE_16G           ((uint8_t)0x30)  // ±16 g 

#define __ACCEL_BlockUpdate_Continous   ((uint8_t)0x00) // Continuos Update 
#define __ACCEL_BlockUpdate_Single      ((uint8_t)0x80) // Single Update: output registers not updated until MSB and LSB reading 

#define __ACCEL_BLE_LSB                 ((uint8_t)0x00) // Little Endian: data LSB @ lower address 
#define __ACCEL_BLE_MSB                 ((uint8_t)0x40) // Big Endian: data MSB @ lower address 

#define __ACCEL_HPM_NORMAL_MODE_RES     ((uint8_t)0x00) //High-Pass Filter mode selection
#define __ACCEL_HPM_REF_SIGNAL          ((uint8_t)0x40)
#define __ACCEL_HPM_NORMAL_MODE         ((uint8_t)0x80)
#define __ACCEL_HPM_AUTORESET_INT       ((uint8_t)0xC0)

#define __ACCEL_HPFCF_8                 ((uint8_t)0x00) // High-pass filter cutoff frequency selection
#define __ACCEL_HPFCF_16                ((uint8_t)0x10)
#define __ACCEL_HPFCF_32                ((uint8_t)0x20)
#define __ACCEL_HPFCF_64                ((uint8_t)0x30)

#define __ACCEL_HPF_AOI1_DISABLE        ((uint8_t)0x00) //AND-OR-Invert function on Interrupt 1 
#define __ACCEL_HPF_AOI1_ENABLE         ((uint8_t)0x01)
#define __ACCEL_HPF_AOI2_DISABLE        ((uint8_t)0x00) //AND-OR-Invert function on Interrupt 2 
#define __ACCEL_HPF_AOI2_ENABLE         ((uint8_t)0x02)

#define __ACCEL_CTRL_REG1								0x20  // Control register 1 acceleration 
#define __ACCEL_CTRL_REG2								0x21  // Control register 2 acceleration 
#define __ACCEL_CTRL_REG3								0x22  // Control register 3 acceleration 
#define __ACCEL_CTRL_REG4								0x23  // Control register 4 acceleration 
#define __ACCEL_CTRL_REG5								0x24  // Control register 5 acceleration 
#define __ACCEL_CTRL_REG6								0x25  // Control register 5 acceleration 

#define __ACCEL_ACC_SENSITIVITY_2G			((uint8_t)0x01) // accelerometer sensitivity with 2 g full scale [mg/LSB] 
#define __ACCEL_ACC_SENSITIVITY_4G			((uint8_t)0x02) // accelerometer sensitivity with 4 g full scale [mg/LSB] 
#define __ACCEL_ACC_SENSITIVITY_8G			((uint8_t)0x04) // accelerometer sensitivity with 8 g full scale [mg/LSB] 
#define __ACCEL_ACC_SENSITIVITY_16G			((uint8_t)0x0C) // accelerometer sensitivity with 12 g full scale [mg/LSB] 

#define __ACCEL_OUT_X_L_A               0x28  // Output Register X acceleration (The value is expressed in two’s complement)*/
#define __ACCEL_OUT_X_H_A               0x29  // Output Register X acceleration 
#define __ACCEL_OUT_Y_L_A               0x2A  // Output Register Y acceleration 
#define __ACCEL_OUT_Y_H_A               0x2B  // Output Register Y acceleration 
#define __ACCEL_OUT_Z_L_A               0x2C  // Output Register Z acceleration 
#define __ACCEL_OUT_Z_H_A               0x2D  // Output Register Z acceleration 

extern int16_t xval;
extern int16_t yval;
extern int16_t zval;

void Accel_Global_Init(void);
void Accel_ReadAcc(void);

#endif
