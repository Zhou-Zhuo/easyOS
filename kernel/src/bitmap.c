#define BITMAP_C

#include <stddef.h>
#include <bitmap.h>

inline void bmp_reset(bitmap_t *bmp)
{
	uint8_t i;

	bmp->group = 0x00;
	for(i = 0; i < 8; i ++)
		bmp->table[i] = 0x00;
}

/* set bit i of bmp->group only when bmp->table[i] is Full */
inline void bmp_setbit_f(bitmap_t *bmp, uint8_t nr)
{
	bmp->table[nr >> 3] |= 1 << (nr & 0x07);
	if(bmp->table[nr >> 3] == 0xff)
		bmp->group |= 1 << (nr >> 3);
}

inline void bmp_clearbit_f(bitmap_t *bmp, uint8_t nr)
{
	bmp->table[nr >> 3] &= ~(1 << (nr & 0x07));
	bmp->group &= ~(1 << (nr >> 3));
}

/* set bit i of bmp->group once bmp->table[i] is Not Empty */
inline void bmp_setbit_ne(bitmap_t *bmp, uint8_t nr)
{
	bmp->table[nr >> 3] |= 1 << (nr & 0x07);
	bmp->group |= 1 << (nr >> 3);
}

inline void bmp_clearbit_ne(bitmap_t *bmp, uint8_t nr)
{
	bmp->table[nr >> 3] &= ~(1 << (nr & 0x07));
	if(bmp->table[nr >> 3] == 0x00)
		bmp->group &= ~(1 << (nr >> 3));
}


/* get the minimum Resetted bit */ 
inline uint8_t bmp_getmin_r(bitmap_t *bmp)
{
	uint8_t min;

	min = unmap_tbl[(uint8_t)~(bmp->group)];
	min = unmap_tbl[(uint8_t)~(bmp->table[min])] + (min << 3);

	return min;
}

/* get the minimum Setted bit */ 
inline uint8_t bmp_getmin_s(bitmap_t *bmp)
{
	uint8_t min;

	min = unmap_tbl[bmp->group];
	min = unmap_tbl[bmp->table[min]] + (min << 3);

	return min;
}


