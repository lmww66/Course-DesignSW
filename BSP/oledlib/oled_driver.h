#ifndef __OLED_DRIVER_H
#define	__OLED_DRIVER_H
#include "stm32f1xx_hal.h"
#include "oled_config.h"
#include "oled_basic.h"

#if (TRANSFER_METHOD ==HW_IIC)

	#if (USE_HW_IIC ==IIC_1)
		#define OLED_ADDRESS	0x78
		#define IIC_GPIOX                GPIOB
		#define IIC_SCL_Pin_X            GPIO_PIN_6
		#define IIC_SDA_Pin_X            GPIO_PIN_7
		#define I2CX                     I2C1
	#elif (USE_HW_IIC ==IIC_2)
		#define OLED_ADDRESS	0x78
		#define IIC_GPIOX                GPIOB
		#define IIC_SCL_Pin_X            GPIO_PIN_10
		#define IIC_SDA_Pin_X            GPIO_PIN_11
		#define I2CX                     I2C2
	#endif
	
#elif (TRANSFER_METHOD ==SW_IIC)

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET)
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET)
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET)


#elif (TRANSFER_METHOD ==HW_SPI)
	
	#if (USE_HW_SPI==SPI_2)
		#define SPIX                      SPI2
		#define SPI_CS_Pin_X              GPIO_PIN_12
		#define SPI_CS_GPIOX              GPIOB
		#define SPI_HW_ALL_PINS           (GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15)
		#define SPI_HW_ALL_GPIOX          GPIOB
		#define SPI_RES_PIN               GPIO_PIN_10
		#define SPI_RES_GPIOX             GPIOB
		#define SPI_DC_PIN                GPIO_PIN_11
		#define SPI_DC_GPIOX              GPIOB
	#endif

#elif (TRANSFER_METHOD ==SW_SPI)


#endif

void I2C_Configuration(void);
void I2C_WriteByte(uint8_t addr,uint8_t data);
void SPI_Configuration(void);

void SW_SPI_Configuration(void);

void SPI_WriterByte(unsigned char dat);
void WriteCmd(unsigned char cmd);
void WriteDat(unsigned char Dat);

void SW_IIC_Configuration(void);

void OLED_Init(void);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_FILL(unsigned char BMP[]);

#endif