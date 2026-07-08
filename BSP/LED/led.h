#ifndef __LED_H
#define __LED_H
#include "sys.h"
#include "typedefs.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */






typedef enum
{
	LED_RED,
	LED_GREEN
} led_t;

//#define LED_FLASH_INFINITE	0
//#define LED_ALL				255
#define LED_FLASH_FAST	50
#define LED_BRIGHTNESS_MAX	255

//LED�˿ڶ���
#define LED0 PAout(6)	// DS0
#define LED1 PBout(0)	// DS1	 



void led_init(void);//��ʼ��	
void led_flash(led_t, byte, byte);
BOOL led_flashing(void);
void led_update(void);
void led_stop(void);

 




#endif