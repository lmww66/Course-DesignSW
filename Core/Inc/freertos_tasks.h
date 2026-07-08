#ifndef __FREERTOS_TASKS_H
#define __FREERTOS_TASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "cmsis_os.h"

/* Task priorities (higher number = higher priority) */
#define TASK_PRIO_DISPLAY       4
#define TASK_PRIO_BUTTONS       3
#define TASK_PRIO_SENSORS       3
#define TASK_PRIO_BUZZER        3
#define TASK_PRIO_SYSTEM        2
#define TASK_PRIO_IDLE          0

/* Task stack sizes (in words) */
#define STACK_DISPLAY           512
#define STACK_BUTTONS           256
#define STACK_SENSORS           512
#define STACK_BUZZER            256
#define STACK_SYSTEM            256

/* Task periods (in ms) */
#define PERIOD_DISPLAY          16
#define PERIOD_BUTTONS          10
#define PERIOD_SENSORS_SLOW     1000
#define PERIOD_SENSORS_FAST     5
#define PERIOD_BUZZER           1
#define PERIOD_SYSTEM           100

/* Inter-task message types */
typedef enum
{
    MSG_NONE = 0,
    MSG_BUTTON_PRESSED,
    MSG_ALARM_TRIGGERED,
    MSG_SENSOR_DATA_READY,
    MSG_USER_WAKE,
    MSG_DISPLAY_REFRESH
} task_msg_type_t;

typedef struct
{
    task_msg_type_t type;
    uint32_t        param;
} task_msg_t;

/* Task handles */
extern osThreadId_t displayTaskHandle;
extern osThreadId_t buttonsTaskHandle;
extern osThreadId_t sensorsTaskHandle;
extern osThreadId_t buzzerTaskHandle;
extern osThreadId_t systemTaskHandle;

/* Mutex handles */
extern osMutexId_t oledMutexHandle;

/* Message queue handles */
extern osMessageQueueId_t systemQueueHandle;
extern osMessageQueueId_t displayQueueHandle;

/* Task function declarations */
void StartDisplayTask(void *argument);
void StartButtonsTask(void *argument);
void StartSensorsTask(void *argument);
void StartBuzzerTask(void *argument);
void StartSystemTask(void *argument);

#endif /* __FREERTOS_TASKS_H */