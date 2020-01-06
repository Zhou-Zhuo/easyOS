#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#define CONSOLE_BUFF_SIZE 128

//typedef uint16_t size_t;

//open flags:
#define R_NONBLOCK 0x0001
#define W_NONBLOCK 0x0002

typedef struct {
	uint16_t oflag;

	int8_t (*open) (uint16_t oflag);
	size_t (*read) (void *buff, size_t len);
	size_t (*write) (void *buff, size_t len);
} serial_t;

#define printf(x, ...)     uprintf(x,##__VA_ARGS__)
#define getc()   ugetc()
#define putc(x)  uputc(x)

int uprintf(const char *fmt, ...);
char ugetc();
char uputc(char ch);
int dprintf(serial_t *dev, const char *fmt, ...);
char dgetc(serial_t *dev);
char dputc(serial_t *dev, char ch);
__inline size_t read(serial_t *dev, void *buff, size_t len);
__inline size_t write(serial_t *dev, void *buff, size_t len);

#endif

