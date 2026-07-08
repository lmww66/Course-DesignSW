#include "oled_driver.h"
#include "delay.h"

#if (TRANSFER_METHOD ==HW_IIC)
//I2C_Configuration����ʼ��Ӳ��IIC����
void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB1PeriphClockCmd(IIC_RCC_APB1Periph_I2CX,ENABLE);
	RCC_APB2PeriphClockCmd(IIC_RCC_APB2Periph_GPIOX,ENABLE);

	GPIO_InitStructure.GPIO_Pin =  IIC_SCL_Pin_X | IIC_SDA_Pin_X;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//I2C���뿪©���
	GPIO_Init(IIC_GPIOX, &GPIO_InitStructure);

	I2C_DeInit(I2CX);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;//������I2C��ַ,���д
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 1320000;

	I2C_Cmd(I2CX, ENABLE);
	I2C_Init(I2CX, &I2C_InitStructure);
	WaitTimeMs(200);
}

 /**
  * @brief  I2C_WriteByte����OLED�Ĵ�����ַдһ��byte������
  * @param  addr���Ĵ�����ַ
	*					data��Ҫд�������
  * @retval ��
  */
void I2C_WriteByte(uint8_t addr,uint8_t data)
{
  while(I2C_GetFlagStatus(I2CX, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2CX, ENABLE);//����I2C1
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,��ģʽ*/

	I2C_Send7bitAddress(I2CX, OLED_ADDRESS, I2C_Direction_Transmitter);//������ַ -- Ĭ��0x78
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2CX, addr);//�Ĵ�����ַ
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2CX, data);//��������
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2CX, ENABLE);//�ر�I2C1����
}

void WriteCmd(unsigned char cmd)//д����
{
		I2C_WriteByte(0x00, cmd);
}

void WriteDat(unsigned char dat)//д����
{
		I2C_WriteByte(0x40, dat);
}

void OLED_FILL(unsigned char BMP[])
{
		unsigned int n;
		unsigned char *p;
		p=BMP;
		WriteCmd(0xb0);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		while(I2C_GetFlagStatus(I2CX, I2C_FLAG_BUSY));	
		I2C_GenerateSTART(I2C1, ENABLE);//����I2C1
		while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,��ģʽ*/
		I2C_Send7bitAddress(I2CX, OLED_ADDRESS, I2C_Direction_Transmitter);//������ַ -- Ĭ��0x78
		while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		I2C_SendData(I2CX, 0x40);//�Ĵ�����ַ
		while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	
		for(n=0;n<128*8;n++)
		{
			I2C_SendData(I2CX, *p++);//��������
			while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		}	
		I2C_GenerateSTOP(I2CX, ENABLE);//�ر�I2C1����
}



#elif  (TRANSFER_METHOD ==HW_SPI)

	#define OLED_RESET_LOW()					HAL_GPIO_WritePin(SPI_RES_GPIOX, SPI_RES_PIN, GPIO_PIN_RESET)
	#define OLED_RESET_HIGH()					HAL_GPIO_WritePin(SPI_RES_GPIOX, SPI_RES_PIN, GPIO_PIN_SET)
	 
	#define OLED_CMD_MODE()						HAL_GPIO_WritePin(SPI_DC_GPIOX, SPI_DC_PIN, GPIO_PIN_RESET)
	#define OLED_DATA_MODE()					HAL_GPIO_WritePin(SPI_DC_GPIOX, SPI_DC_PIN, GPIO_PIN_SET)

	#define OLED_CS_HIGH()   					HAL_GPIO_WritePin(SPI_CS_GPIOX, SPI_CS_Pin_X, GPIO_PIN_SET)
	#define OLED_CS_LOW()  		  			HAL_GPIO_WritePin(SPI_CS_GPIOX, SPI_CS_Pin_X, GPIO_PIN_RESET)
	
	
void SPI_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	#if 	(USE_HW_SPI==SPI_2)
		RCC_APB1PeriphClockCmd(SPI_RCC_APB1Periph_SPIX, ENABLE);
	#elif (USE_HW_SPI==SPI_1)
		RCC_APB2PeriphClockCmd(SPI_RCC_APB2Periph_SPIX, ENABLE);
	#endif
	RCC_APB2PeriphClockCmd(SPI_RCC_APB2Periph_GPIOX|RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin_X;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CS_GPIOX, &GPIO_InitStructure);  
	OLED_CS_HIGH();
	
	GPIO_InitStructure.GPIO_Pin = SPI_HW_ALL_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_HW_ALL_GPIOX, &GPIO_InitStructure);   
	
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI_Direction_1Line_Tx; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 			
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 	
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		  
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  			
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPIX, &SPI_InitStructure);  
	SPI_Cmd(SPIX, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SPI_RES_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_RES_GPIOX, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = SPI_DC_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_DC_GPIOX, &GPIO_InitStructure);  
	OLED_DATA_MODE();
	
	OLED_RESET_HIGH();
	WaitTimeMs(100);
	OLED_RESET_LOW();
	WaitTimeMs(100);
	OLED_RESET_HIGH();
}

void SPI_WriterByte(unsigned char dat)
{

	while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) == RESET ){}; //���ָ����SPI��־λ�������:���ͻ���ձ�־λ	  
		SPI_I2S_SendData(SPIX, dat); //ͨ������SPIx����һ������
	while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE) == RESET){};//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ  						    
	  SPI_I2S_ReceiveData(SPIX); //����ͨ��SPIx������յ�����	

}

void WriteCmd(unsigned char cmd)
{
	OLED_CMD_MODE();
	OLED_CS_LOW();
	SPI_WriterByte(cmd);
	OLED_CS_HIGH();
	OLED_DATA_MODE();
}

void WriteDat(unsigned char dat)
{
	OLED_DATA_MODE();
	OLED_CS_LOW();
	SPI_WriterByte(dat);
	OLED_CS_HIGH();
	OLED_DATA_MODE();
}

void OLED_FILL(unsigned char BMP[])
{
	uint8_t i,j;
	unsigned char *p;
	p=BMP;

  for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);	

		for(j=0;j<128;j++)
		{
			WriteDat(*p++);
		}
	}
}

#elif  (TRANSFER_METHOD ==SW_SPI)

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET)
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET)
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)
#define OLED_RST_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)
 		     
#define OLED_CS_Clr()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET)
#define OLED_CS_Set()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET)

void SW_SPI_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);

	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10, GPIO_PIN_SET);
	OLED_RST_Set();

	delay_ms(100);
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set();
}

void WriteCmd(unsigned char dat)
{	
	uint8_t i;			  
	OLED_DC_Clr();
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
}

void WriteDat(unsigned char dat)
{	
	uint8_t i;			  
	OLED_DC_Set();
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
}

void OLED_FILL(unsigned char BMP[])
{
	uint8_t i,j;
	unsigned char *p = BMP;

	for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		// page0-page1
		WriteCmd(0x00);		    // low column start address
		WriteCmd(0x10);	        // high column start address

		// ====== 优化核心：开启连续数据传输 ======
		IIC_Start();
		Write_IIC_Byte(0x78);   // Slave address (设备地址)
		IIC_Wait_Ack();	
		Write_IIC_Byte(0x40);	// 写数据命令 (只发一次!)
		IIC_Wait_Ack();	

		for(j=0;j<128;j++)
		{
            // 连续128次只发纯数据，不发启停信号
			Write_IIC_Byte(*p++);
			IIC_Wait_Ack();	
		}
		IIC_Stop();  	  
		// ========================================
	}
}

#elif  (TRANSFER_METHOD ==SW_IIC)
void SW_IIC_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
 	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
	delay_ms(200);
}

void IIC_Start()
{
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	//delay_us(2);
	OLED_SDIN_Clr();
	//delay_us(2);
	OLED_SCLK_Clr();
	//delay_us(2);
}

void IIC_Stop()
{
	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	//delay_us(2);
	OLED_SCLK_Set();
	//delay_us(2);
	OLED_SDIN_Set();
	//delay_us(2);
}

void IIC_Wait_Ack()
{
	OLED_SDIN_Set();
	//delay_us(1);
	OLED_SCLK_Set();
	//delay_us(2);
	OLED_SCLK_Clr();
	//delay_us(2);
}

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		if(IIC_Byte & 0x80)
			OLED_SDIN_Set();
		else
			OLED_SDIN_Clr();
		IIC_Byte <<= 1;
		//delay_us(1);
		OLED_SCLK_Set();
		//delay_us(2);
		OLED_SCLK_Clr();
		//delay_us(1);
	}
}
/**********************************************
// IIC Write Command
**********************************************/
void WriteCmd(unsigned char dat)
{
   IIC_Start();
   Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
   Write_IIC_Byte(dat); 
	IIC_Wait_Ack();	
   IIC_Stop();
}
void WriteDat(unsigned char dat)
{	
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
   Write_IIC_Byte(dat);
	IIC_Wait_Ack();	
   IIC_Stop();  	  
}

void OLED_FILL(unsigned char BMP[])
{
	uint8_t i,j;
	unsigned char *p;
	p=BMP;

	for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);	

		for(j=0;j<128;j++)
		{
			WriteDat(*p++);
		}
	}
}

#endif 



void OLED_Init(void)
{
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //���ȵ��� 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
	
//	WriteCmd(0xAE);//--display off
//	WriteCmd(0x02);//---set low column address
//	WriteCmd(0x10);//---set high column address
//	WriteCmd(0x40);//--set start line address  
//	WriteCmd(0xB0);//--set page address
//	WriteCmd(0x81); // contract control
//	WriteCmd(0xFF);//--128   
//	WriteCmd(0xA1);//set segment remap 
//	WriteCmd(0xA6);//--normal / reverse
//	WriteCmd(0xA8);//--set multiplex ratio(1 to 64)
//	WriteCmd(0x3F);//--1/32 duty
//	WriteCmd(0xC8);//Com scan direction
//	WriteCmd(0xD3);//-set display offset
//	WriteCmd(0x00);//
//	
//	WriteCmd(0xD5);//set osc division
//	WriteCmd(0x80);//
//	
//	WriteCmd(0xD8);//set area color mode off
//	WriteCmd(0x05);//
//	
//	WriteCmd(0xD9);//Set Pre-Charge Period
//	WriteCmd(0xF1);//
//	
//	WriteCmd(0xDA);//set com pin configuartion
//	WriteCmd(0x12);//
//	
//	WriteCmd(0xDB);//set Vcomh
//	WriteCmd(0x30);//
//	
//	WriteCmd(0x8D);//set charge pump enable
//	WriteCmd(0x14);//
//	
//	WriteCmd(0xAF);//--turn on oled panel
	OLED_CLS();
}


void OLED_CLS(void)//����
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
		{
				WriteDat(0x00);
		}
	}
}

void OLED_ON(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X14);  //������ɱ�
	WriteCmd(0XAF);  //OLED����
}



void OLED_OFF(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X10);  //�رյ�ɱ�
	WriteCmd(0XAE);  //OLED����
}