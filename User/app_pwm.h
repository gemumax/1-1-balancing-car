#ifndef __APP_PWM_H
#define __APP_PWM_H

void App_PWM_Init(void);
void App_PWM_Cmd(uint8_t on);
void App_PWM_Set_L(float Duty);
void App_PWM_Set_R(float Duty);

#endif
