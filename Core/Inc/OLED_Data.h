#ifndef __OLED_DATA_H
#define __OLED_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    char Index[16];
    uint8_t Data[32];
} ChineseCell_t;

extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F6x8[][6];
extern const uint8_t OLED_F12x24[][36];
extern const ChineseCell_t OLED_CF16x16[];
extern const uint8_t Return[];
extern const uint8_t Frame[];
extern const uint8_t Menu_Graph[][128];
extern const uint8_t Stopimage[][32];
extern const uint8_t Ground[];
extern const uint8_t barrier[][48];
extern const uint8_t Cloud[];
extern const uint8_t Dino[][48];
extern const uint8_t eyebrow[][32];
extern const uint8_t mouth[];
extern const uint8_t Battery[];
#ifdef __cplusplus
}
#endif

#endif /* __OLED_DATA_H */
