#include "usart.h"
#include "stdio.h"

UART_HandleTypeDef huart1;

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

#if EN_USART1_RX
uint8_t USART_RX_BUF[USART_REC_LEN];
uint16_t USART_RX_STA = 0;

void uart_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStructure.Pin = GPIO_PIN_9;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_10;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
// 配置 PA6 为推挽输出
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;             // 改为引脚 6
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   // 推挽输出
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 默认输出低电平，确保开机时灯是灭的
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
		
	huart1.Instance = USART1;
	huart1.Init.BaudRate = bound;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);

	HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	HAL_UART_Receive_IT(&huart1, &USART_RX_BUF[0], 1);

	HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
}

void USART1_IRQHandler(void)
{
	uint8_t Res;

	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
	{
		Res = (uint8_t)(huart1.Instance->DR & 0xFF);

		if((USART_RX_STA & 0x8000) == 0)
		{
			if(Res == '#') 
			{
				USART_RX_STA |= 0x8000; 
			}
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
					USART_RX_STA++;
					if(USART_RX_STA > (USART_REC_LEN - 1)) USART_RX_STA = 0;
				}
			}
		}
	}

#endif