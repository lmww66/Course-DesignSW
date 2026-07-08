#ifndef _GUI_LOG_CONSOLE_H_
#define _GUI_LOG_CONSOLE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "config.h"

#if COMPILE_UART

#include "FreeRTOS.h"
#include "semphr.h"

extern SemaphoreHandle_t xConsoleMutex;

static inline void console_loop_show(void) {}

static inline void console_log(unsigned short time_delay, char* fmt, ...)
{
	(void)time_delay;
	if(xConsoleMutex != NULL)
		xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\r\n");
	if(xConsoleMutex != NULL)
		xSemaphoreGive(xConsoleMutex);
}

static inline bool up_line(void) { return true; }
static inline bool down_line(void) { return true; }

#else

static inline void console_loop_show(void) {}
static inline void console_log(unsigned short time_delay, char* fmt, ...) { (void)time_delay; (void)fmt; }
static inline bool up_line(void) { return true; }
static inline bool down_line(void) { return true; }

#endif

#endif