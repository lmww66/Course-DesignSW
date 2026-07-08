/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

#define OPTION_COUNT		5

static prev_menu_s prevMenuData;

static void mSelect(void);
static void itemLoader(byte);

void mDiagOpen()
{
	//rtc_tempUpdate();//��ȡ�ڲ��¶ȴ�����
//	battery_update();

	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_DIAGNOSTICSMENU));
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
  
	setPrevMenuOpen(&prevMenuData, mDiagOpen);
	
	beginAnimation2(NULL);
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)
		appconfig_save();
	setPrevMenuExit(&prevMenuData);
	doAction(exitSelected());
	
}

static void itemLoader(byte num)
{
	setMenuOption_P(3, PSTR("FW    " FW_VERSION), NULL, NULL);
	setMenuOption_P(4, PSTR("User   " USER_NAME), NULL, NULL);
	addBackOption();
}


//static void updateFPS()
//{
//	char buff[20];
//	char c =  appConfig.showFPS ? CHAR_YES : CHAR_NO;
//	sprintf_P(buff, PSTR(STR_SHOWFPS), c);
//	setMenuOption(2, buff, NULL, setShowFPS);
//}

//static void setShowFPS()
//{
//	appConfig.showFPS = !appConfig.showFPS;
//}