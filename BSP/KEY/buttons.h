#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#define KEY0  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define KEY1  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)
#define KEY2  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)

typedef bool (*button_f)(void);

typedef enum
{
	BTN_1 = 0,
	BTN_2 = 1,
	BTN_3 = 2,
	BTN_COUNT = 3
} btn_t;

void buttons_init(void);
void buttons_update(void);
void buttons_startup(void);
void buttons_shutdown(void);
button_f buttons_setFunc(btn_t, button_f);
void buttons_setFuncs(button_f, button_f, button_f);
bool buttons_isActive(void);
void buttons_wake(void);

#endif