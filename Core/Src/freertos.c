/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  */
/* USER CODE END Header */

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "string.h"
#include <stdio.h>

/* USER CODE BEGIN Includes */
#include "freertos_tasks.h"
#include "common.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "mpu_task.h"
#include "wait_data_clock.h"
#include "gui_log_console.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* USER CODE BEGIN Variables */

/* External globals */
extern char imu_run;
extern float pitch, roll, yaw;
extern unsigned char dmp_ready;
extern uint8_t USART_RX_BUF[];
extern uint16_t USART_RX_STA;

#define USART_REC_LEN 200

/* System init flag */
static bool system_initialized = false;

/* Console print mutex */
SemaphoreHandle_t xConsoleMutex = NULL;

/* USER CODE END Variables */

/* Task handles */
osThreadId_t displayTaskHandle;
osThreadId_t buttonsTaskHandle;
osThreadId_t sensorsTaskHandle;
osThreadId_t buzzerTaskHandle;
osThreadId_t systemTaskHandle;

/* Mutex handles */
osMutexId_t oledMutexHandle;

/* Message queue handles */
osMessageQueueId_t systemQueueHandle;
osMessageQueueId_t displayQueueHandle;

/* Task attributes */
const osThreadAttr_t displayTask_attributes = {
  .name = "display",
  .stack_size = 384 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t buttonsTask_attributes = {
  .name = "buttons",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t sensorsTask_attributes = {
  .name = "sensors",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t buzzerTask_attributes = {
  .name = "buzzer",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

const osThreadAttr_t systemTask_attributes = {
  .name = "system",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* USER CODE BEGIN FunctionPrototypes */
void StartDisplayTask(void *argument);
void StartButtonsTask(void *argument);
void StartSensorsTask(void *argument);
void StartBuzzerTask(void *argument);
void StartSystemTask(void *argument);
/* USER CODE END FunctionPrototypes */

void MX_FREERTOS_Init(void);
extern void MX_FREERTOS_Init(void);

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  oledMutexHandle = osMutexNew(NULL);
  xConsoleMutex = xSemaphoreCreateMutex();
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  systemQueueHandle = osMessageQueueNew(10, sizeof(task_msg_t), NULL);
  displayQueueHandle = osMessageQueueNew(5, sizeof(task_msg_t), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  displayTaskHandle = osThreadNew(StartDisplayTask, NULL, &displayTask_attributes);
  buttonsTaskHandle = osThreadNew(StartButtonsTask, NULL, &buttonsTask_attributes);
  sensorsTaskHandle = osThreadNew(StartSensorsTask, NULL, &sensorsTask_attributes);
  buzzerTaskHandle  = osThreadNew(StartBuzzerTask,  NULL, &buzzerTask_attributes);
  systemTaskHandle  = osThreadNew(StartSystemTask,  NULL, &systemTask_attributes);

  printf("[FRTOS] task create: display=%s buttons=%s sensors=%s buzzer=%s system=%s\r\n",
    displayTaskHandle ? "OK" : "FAIL",
    buttonsTaskHandle ? "OK" : "FAIL",
    sensorsTaskHandle  ? "OK" : "FAIL",
    buzzerTaskHandle  ? "OK" : "FAIL",
    systemTaskHandle  ? "OK" : "FAIL");
  printf("[FRTOS] free heap: %u bytes\r\n", (unsigned)xPortGetFreeHeapSize());

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* USER CODE END RTOS_EVENTS */

  system_initialized = true;
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
  * @brief  Display task - OLED rendering at 16ms period
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN StartDisplayTask */
  console_log(1, "[TASK] display started");

  for (;;)
  {
    if (osMutexAcquire(oledMutexHandle, 5) == osOK)
    {
      display_update();
      memset(&oledBuffer, 0x00, FRAME_BUFFER_SIZE);
      osMutexRelease(oledMutexHandle);
    }

    osDelay(pdMS_TO_TICKS(PERIOD_DISPLAY));
  }
  /* USER CODE END StartDisplayTask */
}

/* USER CODE BEGIN Header_StartButtonsTask */
/**
  * @brief  Buttons task - button scanning at 10ms period
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartButtonsTask */
void StartButtonsTask(void *argument)
{
  /* USER CODE BEGIN StartButtonsTask */
  console_log(1, "[TASK] buttons started");

  for (;;)
  {
    if (pwrmgr_userActive())
    {
      buttons_update();
    }

    osDelay(pdMS_TO_TICKS(PERIOD_BUTTONS));
  }
  /* USER CODE END StartButtonsTask */
}

/* USER CODE BEGIN Header_StartSensorsTask */
/**
  * @brief  Sensors task - MPU6050 data at 5ms period
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartSensorsTask */
void StartSensorsTask(void *argument)
{
  /* USER CODE BEGIN StartSensorsTask */
  console_log(1, "[TASK] sensors started");

  uint16_t stepDiv = 0;

  for (;;)
  {
    if (dmp_ready)
    {
      mpu_dmp_get_data(&pitch, &roll, &yaw);
    }

    stepDiv++;
    if (stepDiv >= (500 / PERIOD_SENSORS_FAST))
    {
      stepDiv = 0;
      step_update();
    }

    osDelay(pdMS_TO_TICKS(PERIOD_SENSORS_FAST));
  }
  /* USER CODE END StartSensorsTask */
}

/* USER CODE BEGIN Header_StartBuzzerTask */
/**
  * @brief  Buzzer task - tone updates at 1ms period
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartBuzzerTask */
void StartBuzzerTask(void *argument)
{
  /* USER CODE BEGIN StartBuzzerTask */
  console_log(1, "[TASK] buzzer started");

  for (;;)
  {
    buzzer_update();

    osDelay(pdMS_TO_TICKS(PERIOD_BUZZER));
  }
  /* USER CODE END StartBuzzerTask */
}

/* USER CODE BEGIN Header_StartSystemTask */
/**
  * @brief  System task - power, alarm, stopwatch, battery at 100ms period
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartSystemTask */

uint8_t Calculate_WeekDay(uint16_t year, uint8_t month, uint8_t day) 
{
    if (month == 1 || month == 2) 
    {
        month += 12;
        year--;
    }

    int week = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1) % 7;
    
    if (week == 0) week = 7; 
    
    return (uint8_t)week;
}

void StartSystemTask(void *argument)
{
  /* USER CODE BEGIN StartSystemTask */
  console_log(1, "[TASK] system started");
  uint32_t lastSecTick = osKernelGetTickCount();

  for (;;)
  {
    uint32_t now = osKernelGetTickCount();
    if((now - lastSecTick) >= pdMS_TO_TICKS(1000))
    {
      uint32_t halMs = HAL_GetTick();
      //console_log(1, "[TIME] secs=%02d frTck=%lu halTck=%lu", timeDate.time.secs, now, halMs);
      time_update();
      lastSecTick += pdMS_TO_TICKS(1000);
    }

    /* Power management */
    pwrmgr_update();

    /* Alarm update */
    if (pwrmgr_userActive())
    {
      alarm_update();
    }

#if COMPILE_STOPWATCH
    stopwatch_update();
#endif

    if (USART_RX_STA & 0x8000) // 0x8000 标志位被置1，代表收到了一句带回车换行的完整指令
    {
				uint16_t len = USART_RX_STA & 0x3FFF;
        USART_RX_BUF[len] = '\0';
        // 1. 读取传感器数据指令：手机发送 "DATA"
        if (strstr((const char*)USART_RX_BUF, "DATA") != NULL)
        {
            // 通过蓝牙给手机回传当前的姿态角数据
            printf("[MPU6050] Pitch: %.1f, Roll: %.1f, Yaw: %.1f\r\n", pitch, roll, yaw);
        }
        
        // 2. 预留的外设控制指令：手机发送 "LED_ON" 或 "LED_OFF"
        else if (strstr((const char*)USART_RX_BUF, "LED_ON") != NULL)
        {
            printf("Command Received: LED ON!\r\n");
            // PA1 输出高电平，点亮独立灯珠
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); 
        }
        else if (strstr((const char*)USART_RX_BUF, "LED_OFF") != NULL)
        {
            printf("Command Received: LED OFF!\r\n");
            // PA1 输出低电平，熄灭独立灯珠
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); 
        }
				
				else if (strncmp((const char*)USART_RX_BUF, "TIME:", 5) == 0)
        {
            int year, month, day, hour, minute, second;
            
            // 使用 sscanf 从字符串中提取 6 个数字
            if (sscanf((const char*)USART_RX_BUF, "TIME:%d,%d,%d,%d,%d,%d", 
                       &year, &month, &day, &hour, &minute, &second) == 6)
            {
                
                timeDate.date.year  = year; 
                timeDate.date.month = month - 1;
                timeDate.date.date   = day;
							
								timeDate.date.day = Calculate_WeekDay(2000 + year, month, day)-1;;
                
                timeDate.time.hour   = hour;
                timeDate.time.mins = minute;
                timeDate.time.secs    = second;

                printf("Time Sync OK: %04d-%02d-%02d %02d:%02d:%02d\r\n", 
                       timeDate.date.year, month, day, hour, minute, second);
            }
            else
            {
                printf("Time Format Error! Use TIME:YY,MM,DD,hh,mm,ss#\r\n");
            }
        }
        
        // 3. 查无此指令
       else
        {
            printf("Unknown Command: %s\r\n", USART_RX_BUF); 
        }

        // 【关键】执行完命令后，必须清空接收缓冲区和状态标志位
        memset(USART_RX_BUF, 0, USART_REC_LEN); // 清空数组，防止上次的指令残留
        USART_RX_STA = 0;                       // 计数值清零，准备接收下一条
    }
    /* ======================================================= */
		
    osDelay(pdMS_TO_TICKS(PERIOD_SYSTEM));
  }
  /* USER CODE END StartSystemTask */
}

/* USER CODE BEGIN Application */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("[FATAL] Stack overflow in: %s\r\n", pcTaskName);
    for(;;);
}

void vApplicationMallocFailedHook(void)
{
    printf("[FATAL] Malloc failed!\r\n");
    for(;;);
}
/* USER CODE END Application */