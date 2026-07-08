#ifndef _MPU_TASK_H_
#define _MPU_TASK_H_

#include "sys.h"

extern float pitch;
extern float roll;
extern float yaw;

extern unsigned long step_count;
extern unsigned char dmp_ready;

void mpu_open(void);
void mpu_updata(void);
void step_update(void);

#endif