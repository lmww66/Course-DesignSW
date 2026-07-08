#include "common.h"
#include "delay.h"
#include "oled_driver.h"
#include "mpu_task.h"

extern bool DeepSleepFlag;

#define STR_WIFICMDMENU  "< My Menu >"
#define CMD1_NAME "Shut Down"
#define CMD2_NAME "Deep Sleep"
#define CMD3_NAME "MPU_Display"
#define CMD4_NAME "History_Data"
#define CMD5_NAME "Back"

#define OPTION_COUNT 5


static void mSelect()
{
	doAction(false); 
	//menuData.isOpen = false;
}

void ShutDown(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}

void cmd2(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	DeepSleepFlag = 1;
	OLED_OFF();
	menuData.isOpen = false;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_AFIO_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	EXTI->IMR |= GPIO_PIN_4;
	EXTI->FTSR |= GPIO_PIN_4;
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 4);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

void cmd3(void)
{
	animation_start(display_load, ANIM_MOVE_OFF);
	menuData.isOpen = false;
}
static u8 log_time = 5;

static void LogTimeUpdate()
{
//	battery_updateNow();
	log_time+=2;
	if(log_time>15)
		log_time=1;
}

static void itemLoader(byte num)
{
	char buff[20];
	num = 0;

	setMenuOption_P(num++, PSTR(CMD1_NAME), NULL, ShutDown);

	setMenuOption_P(num++, PSTR(CMD2_NAME), NULL, cmd2);
	
	setMenuOption_P(num++, PSTR(CMD3_NAME), NULL, mpu_open);
	
	setMenuOption_P(num++, PSTR(CMD4_NAME), NULL, cmd3);
	
	sprintf_P((char *)buff, PSTR("Log Time  %d min"), log_time);
	setMenuOption_P(num++, buff, NULL, LogTimeUpdate);

	setMenuOption_P(num++, PSTR(CMD5_NAME), NULL, cmd3);
}

bool my_menu_open(void)
{
	menuData.isOpen = true;  //�򿪲˵�
	
	display_setDrawFunc(menu_draw);  //�󶨻��ƺ���Ϊmenu_draw

	buttons_setFuncs(menu_up, menu_down ,menu_select);  //�󶨰������ܺ���

	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_WIFICMDMENU));   //��ȡ��ǰ�˵���Ϣ��ѡ��������˵����������ֻ���ͼ�꣩
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);  //�󶨲˵��ĺ���,��ǰ������ѡ��ȷ��
	beginAnimation2(NULL);   
	return true;
}