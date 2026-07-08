#include "delay.h"
#include "stm32f1xx_hal.h"

#define DWT_CYCCNT    ((volatile uint32_t *)0xE0001004)
#define DWT_CONTROL   ((volatile uint32_t *)0xE0001000)
#define SCB_DEMCR     ((volatile uint32_t *)0xE000EDFC)

static uint8_t fac_us = 0;

void delay_init(void)
{
    fac_us = SystemCoreClock / 1000000;

    *SCB_DEMCR |= (1 << 24);
    *DWT_CYCCNT = 0;
    *DWT_CONTROL |= (1 << 0);
}

void delay_us(uint32_t nus)
{
    uint32_t start = *DWT_CYCCNT;
    uint32_t ticks = nus * fac_us;
    while ((*DWT_CYCCNT - start) < ticks);
}

void delay_ms(uint16_t nms)
{
    HAL_Delay(nms);
}