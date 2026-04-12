#ifndef __MYRTC_H
#define __MYRTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern uint16_t MyRTC_Time[6];

void MyRTC_Init(void);
void MyRTC_ReadTime(void);
void MyRTC_SetTime(void);

#ifdef __cplusplus
}
#endif

#endif /* __MYRTC_H */
