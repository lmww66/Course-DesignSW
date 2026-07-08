#ifndef __USART_H
#define __USART_H

#include "stm32f1xx_hal.h"

#define USART_REC_LEN  200
#define EN_USART1_RX   1

extern UART_HandleTypeDef huart1;
extern uint8_t  USART_RX_BUF[USART_REC_LEN];
extern uint16_t USART_RX_STA;

void uart_init(uint32_t bound);

#endif