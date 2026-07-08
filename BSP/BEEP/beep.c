#include "beep.h"
#include "stm32f1xx_hal_tim.h"
#include "common.h"

static uint buzzLen;
static millis8_t startTime;
static buzzFinish_f onFinish;
static tonePrio_t prio;

static void stop(void);

static TIM_HandleTypeDef htim2;

void TIM_PWM_Init_Init(u32 arr, u32 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	TIM_OC_InitTypeDef TIM_OCInitStructure = {0};

	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_AFIO_REMAP_TIM2_PARTIAL_1();

	GPIO_InitStructure.Pin = GPIO_PIN_15;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = psc;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = arr;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim2);

	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM2;
	TIM_OCInitStructure.Pulse = 25;
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim2, &TIM_OCInitStructure, TIM_CHANNEL_1);
}

void buzzer_init()
{
	TIM_PWM_Init_Init(50, 1439);
}

void buzzer_buzz(uint16_t len, uint16_t tone, vol_t volType, tonePrio_t _prio, buzzFinish_f _onFinish)
{
	if(_prio < prio)
		return;

	if(len == 0 && tone == TONE_STOP)
	{
		stop();
		return;
	}
	else if(tone == TONE_STOP)
	{
		stop();
		return;
	}

	prio = _prio;
	onFinish = _onFinish;
	buzzLen = len;
	startTime = millis();

	if(tone == TONE_PAUSE)
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 5000);
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		return;
	}

	uint ocr;
	byte vol;

	switch(volType)
	{
		case VOL_UI:
			vol = appConfig.volUI;
			break;
		case VOL_ALARM:
			vol = appConfig.volAlarm;
			break;
		case VOL_HOUR:
			vol = appConfig.volHour;
			break;
		default:
			vol = 2;
			break;
	}

	ocr = 50000 / tone;

	switch(vol)
	{
		case 0:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 5000);
			break;
		case 1:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ocr / 8);
			break;
		case 2:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ocr / 4);
			break;
		case 3:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ocr / 2);
			break;
	}

	__HAL_TIM_SET_AUTORELOAD(&htim2, ocr);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

static void stop(void)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	prio = 0;
	if(onFinish != NULL)
	{
		buzzFinish_f f = onFinish;
		onFinish = NULL;
		f();
	}
}

void buzzer_update(void)
{
	if(buzzLen && (millis8_t)(millis() - startTime) >= buzzLen)
	{
		buzzLen = 0;
		stop();
	}
}

BOOL buzzer_isActive(void)
{
	return buzzLen;
}