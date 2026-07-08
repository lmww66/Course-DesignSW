#include "buttons.h"
#include "config.h"
#include "pwrmgr.h"
#include "millis.h"
#include "appconfig.h"
#include "tune.h"
#include "tunes.h"
#include "beep.h"
#include <stdio.h>

#define BTN_IS_PRESSED  4
#define BTN_NOT_PRESSED 4

typedef struct {
  millis_t pressedTime;
  bool processed;
  uint8_t counter;
  bool funcDone;
  button_f onPress;
  const tune_t* tune;
} s_button;

static s_button buttons[BTN_COUNT];

static void processButtons(void);
static void processButton(s_button*, uint8_t);
static uint8_t bitCount(uint8_t);

void buttons_init()
{
  buttons_startup();

  buttons[BTN_1].tune = tuneBtn1;
  buttons[BTN_2].tune = tuneBtn2;
  buttons[BTN_3].tune = tuneBtn3;
}

void buttons_update()
{
  processButtons();
}

void buttons_startup()
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_AFIO_REMAP_SWJ_NOJTAG();

	HAL_NVIC_DisableIRQ(EXTI4_IRQn);

	EXTI->IMR &= ~GPIO_PIN_4;
	EXTI->EMR &= ~GPIO_PIN_4;
	EXTI->RTSR &= ~GPIO_PIN_4;
	EXTI->FTSR &= ~GPIO_PIN_4;
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);

	GPIO_InitStructure.Pin = GPIO_PIN_0;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_2;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_4;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void buttons_shutdown()
{
}

static void processButtons()
{
  uint8_t isPressed[BTN_COUNT];
  isPressed[BTN_1] = KEY0;
  isPressed[BTN_2] = KEY1;
  isPressed[BTN_3] = KEY2;

#if COMPILE_UART
  {
    static millis_t lastDebug = 0;
    millis_t now = millis();
    if((millis_t)(now - lastDebug) >= 2000)
    {
      lastDebug = now;
      //printf("BTN: KEY0=%d KEY1=%d KEY2=%d active=%d\r\n",
             //HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0),
             //HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2),
             //HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4),
             //pwrmgr_userActive());
    }
  }
#endif

  for(uint8_t i = 0; i < BTN_COUNT; i++)
    processButton(&buttons[i], !isPressed[i]);
}

static void processButton(s_button* button, uint8_t isPressed)
{
  button->counter <<= 1;
  if (isPressed)
  {
    button->counter |= 1;

    if (bitCount(button->counter) >= BTN_IS_PRESSED)
    {
      if (!button->processed)
      {
        button->pressedTime = millis();
        button->processed = true;
      }

      if (!button->funcDone && button->onPress != NULL && button->onPress())
      {
        button->funcDone = true;
        tune_play(button->tune, VOL_UI, PRIO_UI);
#if COMPILE_UART
        printf("BTN pressed! func executed\r\n");
#endif
      }
    }
  }
  else
  {
    if (bitCount(button->counter) <= BTN_NOT_PRESSED)
    {
      button->processed = false;
      button->funcDone = false;
    }
  }
}

static uint8_t bitCount(uint8_t val)
{
  uint8_t count = 0;
  for (; val; val >>= 1)
    count += val & 1;
  return count;
}

button_f buttons_setFunc(btn_t btn, button_f func)
{
  button_f old = buttons[btn].onPress;
  buttons[btn].onPress = func;
  return old;
}

void buttons_setFuncs(button_f btn1, button_f btn2, button_f btn3)
{
  buttons[BTN_1].onPress = btn1;
  buttons[BTN_2].onPress = btn2;
  buttons[BTN_3].onPress = btn3;
}

bool buttons_isActive()
{
  if (!appConfig.sleepTimeout)
    return true;

  uint timeout = (appConfig.sleepTimeout * 5) * 1000;

  for(uint8_t i = 0; i < BTN_COUNT; i++)
  {
    if (millis() - buttons[i].pressedTime < timeout)
      return true;
  }

  return false;
}

void buttons_wake()
{
  for(uint8_t i = 0; i < BTN_COUNT; i++)
  {
    buttons[i].funcDone   = true;
    buttons[i].processed  = true;
    buttons[i].counter    = BTN_IS_PRESSED;
    buttons[i].pressedTime = millis();
  }
}