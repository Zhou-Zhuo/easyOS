#ifndef CPU_H
#define CPU_H

#include <common.h>

#define CPU_FREQ 72000000

#define __ENTER_CRITICAL() \
	cpu_sr = cpu_sr_save()

#define __EXIT_CRITICAL() \
	cpu_sr_restore(cpu_sr)

typedef uint32_t cpu_sr_t;

void context_switch();
void first_switch();
uint32_t cpu_sr_save();
void cpu_sr_restore(uint32_t cpu_sr);

#endif

