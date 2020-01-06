#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

#ifdef BITMAP_C 
#define __BITMAP_EXTERN 
#else 
#define __BITMAP_EXTERN extern
#endif

typedef struct {
	uint8_t group;
	uint8_t table[8];
} bitmap_t;

#ifdef BITMAP_C
const uint8_t unmap_tbl[] = {
	0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
};
#else 
extern const uint8_t unmap_tbl[];
#endif
/* reset every bit of bitmap */
void bmp_reset(bitmap_t *bmp);

void bmp_setbit_f(bitmap_t *bmp, uint8_t nr);

void bmp_clearbit_f(bitmap_t *bmp, uint8_t nr);

void bmp_setbit_ne(bitmap_t *bmp, uint8_t nr);

void bmp_clearbit_ne(bitmap_t *bmp, uint8_t nr);

uint8_t bmp_getmin_r(bitmap_t *bmp);

uint8_t bmp_getmin_s(bitmap_t *bmp);

#endif
