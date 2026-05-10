#ifndef __SETTIME_H
#define __SETTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "menu.h"

UiPage_t Page_SelField(void);
UiPage_t Page_AdjYear(void);
UiPage_t Page_AdjMonth(void);
UiPage_t Page_AdjDay(void);
UiPage_t Page_AdjHour(void);
UiPage_t Page_AdjMin(void);
UiPage_t Page_AdjSec(void);

#ifdef __cplusplus
}
#endif

#endif /* __SETTIME_H */
