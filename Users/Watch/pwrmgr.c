/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

// Deals with sleeping and waking up/��Դ����

#include "common.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

#define BATTERY_CUTOFF	2800

typedef enum
{
	SYS_AWAKE,
	SYS_CRTANIM,
	SYS_SLEEP
} sys_t;

typedef enum
{
	USER_ACTIVE,
	USER_INACTIVE
} user_t;


static sys_t systemState;
static user_t userState;

static void batteryCutoff(void);

void pwrmgr_init()
{
	systemState = SYS_AWAKE;
	userState = USER_ACTIVE;
	//set_sleep_mode(SLEEP_MODE_IDLE);
}

static void batteryCutoff()
{
//	// If the battery voltage goes below a threshold then disable
//	// all wakeup sources apart from USB plug-in and go to power down sleep.
//	// This helps protect the battery from undervoltage and since the battery's own PCM hasn't kicked in yet the RTC will continue working.

//	uint voltage = battery_voltage();
//	if(voltage < BATTERY_CUTOFF && !USB_CONNECTED() && voltage)
//	{
//		// Turn everything off
//		buttons_shutdown();
//		tune_stop(PRIO_MAX);
//		led_stop();
//		oled_power(OLED_PWR_OFF);
//		time_shutdown();
//		
//		// Stay in this loop until USB is plugged in or the battery voltage is above the threshold
//		do
//		{
//			set_sleep_mode(SLEEP_MODE_PWR_DOWN);

//			cli();
//			sleep_enable();
//			sleep_bod_disable();
//			sei();
//			sleep_cpu();
//			sleep_disable();
//			
//			// Get battery voltage
//			battery_updateNow();

//		} while(!USB_CONNECTED() && battery_voltage() < BATTERY_CUTOFF);

//		// Wake up
//		time_wake();
//		buttons_startup();
//		buttons_wake();
//		oled_power(OLED_PWR_ON);
//	}
}

bool keep_on=0;
short pitch_a,roll_a,yaw_a;
/*************************����ת******************************************/
void movecheck(void)
{
		if(MPU_Get_Gyroscope(&pitch_a,&roll_a,&yaw_a)==0)
			if(pitch_a>2300||pitch_a<-2300)
			{
				vTaskDelay(pdMS_TO_TICKS(300));
				u8 retry = 100;
				while(MPU_Get_Gyroscope(&pitch_a,&roll_a,&yaw_a) && --retry)
					vTaskDelay(pdMS_TO_TICKS(5));
				if(retry && (pitch_a>2300||pitch_a<-2300))
					userWake();
			}
}

extern bool MoveCheckFlag;
void pwrmgr_update()
{
	batteryCutoff();

	if(!appConfig.sleepTimeout)
		return;

	bool buttonsActive = buttons_isActive()||keep_on;

	if(buttonsActive)
	{
		
#if COMPILE_ANIMATIONS
		if(systemState == SYS_CRTANIM && buttonsActive)
		{
			display_startCRTAnim(CRTANIM_OPEN);
			systemState = SYS_AWAKE;
		}
		else
#endif
		{
		}
	}
	else
	{
#if COMPILE_ANIMATIONS
		if(systemState == SYS_AWAKE)
		{
			systemState = SYS_CRTANIM;
			display_startCRTAnim(CRTANIM_CLOSE);
		}
		else if(systemState == SYS_CRTANIM)
#endif
		{
			systemState = SYS_CRTANIM;

			if(time_wake() != RTCWAKE_SYSTEM)
				userWake();
			if(MoveCheckFlag)
				movecheck();
		}
	}
}



bool pwrmgr_userActive()
{
	return userState == USER_ACTIVE;
}

void userWake(void)
{
	userState = USER_ACTIVE;
	buttons_wake();
	display_startCRTAnim(CRTANIM_OPEN);
	//oled_power(OLED_PWR_ON);
	//battery_setUpdate(3);
}

//static void userSleep()
//{
//	userState = USER_INACTIVE;
//	//oled_power(OLED_PWR_OFF);
//}