## 代码

#include <stdio.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"

#define JSC5396_DEVADDR 0x1e
#define MDC_PORT	GPIOB
#define MDC_PIN 	GPIO_PIN_10
#define MDIO_PORT 	GPIOB
#define MDIO_PIN 	GPIO_PIN_11

#define MDC_Set() HAL_GPIO_WritePin(MDC_PORT, MDC_PIN, GPIO_PIN_SET)
#define MDC_Clr() HAL_GPIO_WritePin(MDC_PORT, MDC_PIN, GPIO_PIN_RESET)
#define MDIO_Set() HAL_GPIO_WritePin(MDIO_PORT, MDIO_PIN, GPIO_PIN_SET)
#define MDIO_Clr() HAL_GPIO_WritePin(MDIO_PORT, MDIO_PIN, GPIO_PIN_RESET)
#define MDIO_Get()	HAL_GPIO_ReadPin(MDIO_PORT, MDIO_PIN)

void udelay(int us)
{
	int i,j;
	for(i=us;i>0;i--)
		for(j=10;j>0;j--)
			__NOP();
}

void bit_set(uint8_t b)	
{	
	MDC_Clr();	
	if((b))					
		MDIO_Set();	
	else					
		MDIO_Clr();	
	udelay(1);	
	MDC_Set();	
	udelay(1);	
}

void bit_get(uint8_t *pb)	
{	
	MDC_Clr();	
	MDIO_Set();	
	udelay(1);	
	*(pb) = (uint8_t)MDIO_Get();
	MDC_Set();	
	udelay(1);	
}

void phy_gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/*MDC  */
  GPIO_InitStruct.Pin = MDC_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MDC_PORT, &GPIO_InitStruct);
	
	/* MDIO */
  GPIO_InitStruct.Pin = MDIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MDIO_PORT, &GPIO_InitStruct);
}

void mdio_write(uint8_t phyaddr, uint8_t regaddr, uint16_t data)
{
	int i;
	//Preamble
	for(i=0;i<32;i++)
	{
		bit_set(1);
	}
	//Start
	bit_set(0);
	bit_set(1);

	//OP code
	bit_set(0);
	bit_set(1);

	//phy address
	for(i=0;i<5;i++)
	{
		if(phyaddr&(0x10>>i))
			bit_set(1);
		else
			bit_set(0);
	}
	
	//reg address
	for(i=0;i<5;i++)
	{
		if(regaddr&(0x10>>i))
			bit_set(1);
		else
			bit_set(0);
	}
	
	//ack
	bit_set(1);
	bit_set(0);
	
	//data
	for(i=0;i<16;i++)
	{
		if(data&(0x8000>>i))
			bit_set(1);
		else
			bit_set(0);
	}
}

uint16_t mdio_read(uint8_t phyaddr, uint8_t regaddr)
{
	uint16_t data = 0;
	int i;
	uint8_t b;
	//Preamble
	for(i=0;i<32;i++)
	{
		bit_set(1);
	}
	//Start
	bit_set(0);
	bit_set(1);

	//OP code
	bit_set(1);
	bit_set(0);

	//phy address
	for(i=0;i<5;i++)
	{
		if(phyaddr&(0x10>>i))
			bit_set(1);
		else
			bit_set(0);
	}
	
	//reg address
	for(i=0;i<5;i++)
	{
		if(regaddr&(0x10>>i))
			bit_set(1);
		else
			bit_set(0);
	}
	
	//ack
	bit_get(&b);
	bit_get(&b);
	
	//data
	for(i=0;i<16;i++)
	{
		data <<= 1;
		bit_get(&b);
		if(b)
			data |= 0x01;
	}
	return data;
}

## 调用

void YT8521_Test()
{
	uint16_t data;
	//write
	mdio_write(0x0, 0x1e, 0xa003);	//set reg address
	mdio_write(0x0, 0x1f, 0x80f1);	//set reg value
	//read
	data = mdio_read(0x0, 0x11);
	printf("sds 0x11 = %04x\n", data);
	
	mdio_write(0x0, 0x1e, 0xa004);
	HAL_Delay(1);
	data = mdio_read(0x0, 0x1f);
	printf("0xa004 = %04x\n", data);
}
