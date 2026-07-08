/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 
 
#define eeprom_busy_wait() do {} while (!eeprom_is_ready())//�ȴ�EEPROM�������

extern uint8_t eeprom_read_byte (const uint8_t *addr);//��ȡָ����ַ��һ���ֽ�8bit��EEPROM����

extern uint16_t eeprom_read_word (const uint16_t *addr);//��ȡָ����ַ��һ����16bit��EEPROM����

extern void eeprom_read_block (void *buf, const void *addr, size_t n);//��ȡ��ָ����ַ��ʼ��ָ�����ȵ�EEPROM����

extern void eeprom_write_byte (uint8_t *addr, uint8_t val);//��ָ����ַд��һ���ֽ�8bit��EEPROM����

extern void eeprom_write_word (uint16_t *addr, uint16_t val);//��ָ����ַд��һ����16bit��EEPROM����
 
 
 */

#include "common.h"
//#include "stmflash.h"



#define LENTH(Buffer) sizeof(Buffer)	 		  	//���鳤��	
#define SIZE(buff) LENTH(buff)/4+((LENTH(buff)%4)?1:0)

#define EEPROM_CHECK_NUM 0xFF // Any 8 bit number that isn't 0 or 255

#define eepCheck_SAVE_ADDR   0X080E0000 	//����FLASH �����ַ(����Ϊż��������������,Ҫ���ڱ�������ռ�õ�������.
										//����,д������ʱ��,���ܻᵼ�²�����������,�Ӷ����𲿷ֳ���ʧ.��������.
#define appConfig_SAVE_ADDR  eepCheck_SAVE_ADDR +16 	//����FLASH �����ַ(����Ϊż��������������,Ҫ���ڱ�������ռ�õ�������.
										//����,д������ʱ��,���ܻᵼ�²�����������,�Ӷ����𲿷ֳ���ʧ.��������.


appconfig_s appConfig;     //appconfig_s�ĳ���Ϊ8

//static byte eepCheck EEMEM ;//= EEPROM_CHECK_NUM;

//static appconfig_s eepConfig EEMEM = {
//	0, // sleep timeout (this value * 5 = sleep timeout in secs)
//	false, // invert
//#if COMPILE_ANIMATIONS
//	true, // animations
//#endif
//	true, // 180 rotate
//	false, // FPS
//	TIMEMODE_24HR,
//	{
//		255 // volume + brightness (all max)
//	}
//};

void appconfig_init()
{

//	 STMFLASH_Read(eepCheck_SAVE_ADDR,(u32*)(&eepCheck),LENTH(byte));
////   appConfig = (appconfig_s *) malloc(sizeof(appconfig_s));
//	     memset(&appConfig, 0x00, LENTH(appconfig_s));

//	
//	
//	 if(eepCheck == EEPROM_CHECK_NUM)
//	
//		 STMFLASH_Read(appConfig_SAVE_ADDR,(u32*)(&appConfig),LENTH(appconfig_s));
//	else
//	{
//    eepCheck = EEPROM_CHECK_NUM;
//		STMFLASH_Write(eepCheck_SAVE_ADDR,(u32*)(&eepCheck),LENTH(byte));

		appconfig_reset();
//	}

//	if(appConfig.sleepTimeout > 12)
//		appConfig.sleepTimeout = 0;
}

void appconfig_save()
{

  	//STMFLASH_Write(appConfig_SAVE_ADDR,(u32*)(&appConfig),LENTH(appconfig_s));
}

void appconfig_reset()
{
	appConfig.sleepTimeout = 0;
	appConfig.invert = false;
#if COMPILE_ANIMATIONS
	appConfig.animations = true;
#endif
	appConfig.display180 = false;
  	appConfig.CTRL_LEDs = false;
	appConfig.showFPS = false;
	appConfig.timeMode = TIMEMODE_24HR;
	//appConfig.volumes = 255;
	
	appConfig.volUI=2;
	appConfig.volAlarm=2;
	appConfig.volHour=1;
	
	appconfig_save();

//	alarm_reset();
}