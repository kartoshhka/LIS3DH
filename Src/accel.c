#include "stm32l4xx_hal.h"
#include "accel.h"
#include "i2c.h"

int16_t xval;
int16_t yval;
int16_t zval;
uint16_t connection_cnt = 1000;

static uint8_t I2Cx_ReadData(uint16_t Addr, uint8_t Reg);
static void I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value);
uint8_t Accel_IO_Read(uint16_t DeviceAddr, uint8_t RegisterAddr);
void Accel_IO_Write(uint16_t DeviceAddr, uint8_t RegisterAddr, uint8_t Value);
uint8_t Accel_ReadID(void);
void Accel_Global_Init(void);
void Accel_Config_Regs(uint16_t InitStruct);
void Accel_AccFilterConfig(uint8_t FilterStruct);
void Accel_ReadAcc(void);

static void Error (void)
{
//invent error identifier???
	while(1)
	{
	}
}

static uint8_t I2Cx_ReadData(uint16_t Addr, uint8_t Reg)
{
  //HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;

	if (HAL_I2C_Mem_Read(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 0x10000) != HAL_OK)
  {
    // Execute user timeout callback 
    Error();
  }
  return value;
}

static void I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
  //HAL_StatusTypeDef status = HAL_OK;

  /* Check the communication status */
  if (HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 0x10000) != HAL_OK)
  {
    /* Execute user timeout callback */
    Error();
  }
}

uint8_t Accel_IO_Read(uint16_t DeviceAddr, uint8_t RegisterAddr)
{
	return I2Cx_ReadData(DeviceAddr, RegisterAddr);
}

void Accel_IO_Write(uint16_t DeviceAddr, uint8_t RegisterAddr, uint8_t Value)
{
  I2Cx_WriteData(DeviceAddr, RegisterAddr, Value);
}

uint8_t Accel_ReadID(void)
{  
  uint8_t ctrl = 0x00;
	ctrl = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_REG_ID_ADDR);
	return ctrl;
}

void Accel_Global_Init(void)
{
	uint16_t ctrl = 0x0000;
	HAL_Delay(1000);
	//while (connection_cnt)
	//	connection_cnt--;
  if(Accel_ReadID()!=__ACCEL_ID) 
		Error();
	
	ctrl |= (__ACCEL_NORMAL_MODE | __ACCEL_ODR_50_HZ | __ACCEL_AXES_ENABLE);
  ctrl |= ((__ACCEL_BlockUpdate_Continous | __ACCEL_BLE_LSB | __ACCEL_HR_ENABLE) <<8 );
  Accel_Config_Regs(ctrl);
	
	ctrl = (uint8_t) (__ACCEL_HPM_NORMAL_MODE | __ACCEL_HPFCF_16 | __ACCEL_HPF_AOI1_DISABLE | __ACCEL_HPF_AOI2_DISABLE);
	Accel_AccFilterConfig(ctrl);
}

void Accel_Config_Regs(uint16_t InitStruct)
{
	uint8_t ctrl = 0x00;
	ctrl = (uint8_t) InitStruct;
	Accel_IO_Write(__ACCEL_ADDR, __ACCEL_CTRL_REG1, ctrl);
	ctrl = (uint8_t) (InitStruct << 8);
	Accel_IO_Write(__ACCEL_ADDR, __ACCEL_CTRL_REG4, ctrl);
}

void Accel_AccFilterConfig(uint8_t FilterStruct)
{
  uint8_t tmpreg;
  /* Read CTRL_REG2 register */
  tmpreg = Accel_IO_Read(__ACCEL_ADDR	, __ACCEL_CTRL_REG2);
  tmpreg &= 0x0C; //Filtered data selection & HPF enabled
  tmpreg |= FilterStruct;
  /* Write value to ACC MEMS CTRL_REG2 register */
  Accel_IO_Write(__ACCEL_ADDR, __ACCEL_CTRL_REG2, tmpreg);
}

void Accel_GetXYZ(int16_t* pData)

{
  int16_t pnRawData[3]; //global buffer for obtaining
  uint8_t ctrlx[2]={0,0};
  int8_t buffer[6];
  uint8_t i = 0;
  uint8_t sensitivity = __ACCEL_ACC_SENSITIVITY_2G;
	
  /* Read the acceleration control register content */
  ctrlx[0] = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_CTRL_REG4);
  ctrlx[1] = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_CTRL_REG5);

  /* Read output register X, Y & Z acceleration */
  buffer[0] = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_OUT_X_L_A);
  buffer[1] = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_OUT_X_H_A);
  buffer[2] = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_OUT_Y_L_A);
  buffer[3] = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_OUT_Y_H_A);
  buffer[4] = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_OUT_Z_L_A);
  buffer[5] = Accel_IO_Read(__ACCEL_ADDR, __ACCEL_OUT_Z_H_A);

  /* Check in the control register4 the data alignment*/
  if(!(ctrlx[0] & __ACCEL_BLE_MSB))
  {
    for(i=0; i<3; i++)
    {
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i]);
    }
  }
  else /* Big Endian Mode */
  {
    for(i=0; i<3; i++)
    {
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1]);
    }
  }
  /* Normal mode */
  /* Switch the sensitivity value set in the CRTL4 */
  switch(ctrlx[0] & __ACCEL_FULLSCALE_16G)
  {
  case __ACCEL_FULLSCALE_2G:
    sensitivity = __ACCEL_ACC_SENSITIVITY_2G;
    break;
  case __ACCEL_FULLSCALE_4G:
    sensitivity = __ACCEL_ACC_SENSITIVITY_4G;
    break;
  case __ACCEL_FULLSCALE_8G:
    sensitivity = __ACCEL_ACC_SENSITIVITY_8G;
    break;
  case __ACCEL_FULLSCALE_16G:
    sensitivity = __ACCEL_ACC_SENSITIVITY_16G;
    break;
  }
	
  /* Obtain the mg value for the three axis */
  for(i=0; i<3; i++)
  {
    pData[i]=(pnRawData[i] * sensitivity);
  }
}

void Accel_ReadAcc(void)
{
	int16_t buffer[3] = {0};
	Accel_GetXYZ(buffer);
  xval = buffer[0];
  yval = buffer[1];
  zval = buffer[2];
}



