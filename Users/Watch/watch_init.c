#include "common.h"
#include "watch_init.h"
#include "usart.h"

void watch_hardware_init(void)
{
    char i = 0, count = 0;

    console_log(1, "===== N|Watch Hardware Init =====");

    {
        GPIO_InitTypeDef GPIO_InitStructure = {0};
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStructure.Pin = GPIO_PIN_12;
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    }
    console_log(1, "[OK] PWR GPIO init (PB12)");

    delay_init();
    console_log(1, "[OK] DWT delay init (SysCoreClock=%lu)", SystemCoreClock);

    LCD_Init();
    console_log(1, "[OK] OLED init (PB8/PB9 I2C)");

    millis_init();
    console_log(1, "[OK] millis init");

    delay_ms(50);
    i = MPU_Init();
    console_log(1, "[..] MPU6050 init result=%d", i);

    if (i == 0)
    {
        delay_ms(50);
        i = mpu_dmp_init();
        while ((++count) < 5 && i)
        {
            console_log(1, "[!!] MPU DMP error=%d, retry %d/5", i, count);
            delay_ms(100);
            i = mpu_dmp_init();
        }
        if (i == 0)
        {
            dmp_ready = 1;
            console_log(1, "[OK] MPU6050 DMP init (PB10/PB11 I2C)");
        }
        else
            console_log(1, "[FAIL] MPU6050 DMP init err=%d", i);
    }

    milliseconds = 0;
    memset(&oledBuffer, 0x00, FRAME_BUFFER_SIZE);

    appconfig_init();
    console_log(1, "[OK] appconfig init (sleepTimeout=%d, timeMode=%d)",
                appConfig.sleepTimeout, appConfig.timeMode);

    buzzer_init();
    console_log(1, "[OK] buzzer init (TIM2 CH1 PA15)");

    buttons_init();
    console_log(1, "[OK] buttons init (PA0/PA2/PA4)");

    alarm_init();
    console_log(1, "[OK] alarm init");

    pwrmgr_init();
    console_log(1, "[OK] pwrmgr init");

    console_log(1, "===== All Hardware Init Done =====");

    display_set(watchface_normal);
    display_load();
    console_log(1, "[OK] display set to watchface, entering FreeRTOS...");
}