#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!   
#
##############################################################################################
#
#	this is originally from https://github.com/halherta/iotogglem0_wspl
#
#
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# make flash_stlink = build project files and flash the microcontroller via st-flash (https://github.com/texane/stlink.git)
#
# To rebuild project do "make clean" and "make all".
#
# This makefile offers the possibility to build STM32 projects without any IDE.
# Since the latest release of the GNU ARM Plugin for the eclipse IDE is a crap it gives back the
# possibilty to define own project directory structures.
#
# Included originally in the yagarto projects. Original Author : Michael Fischer
# Modified by Franz Flasch
# Use at your own risk!!!!!
#
##############################################################################################
# Start of default section
#
CCPREFIX   = arm-none-eabi-
CC         = $(CCPREFIX)gcc
LD         = $(CCPREFIX)ld
CP         = $(CCPREFIX)objcopy
DUMP	   = $(CCPREFIX)objdump
AS         = $(CCPREFIX)gcc -x assembler-with-cpp
GDB		   = $(CCPREFIX)gdb
GDBTUI     = $(CCPREFIX)gdb --tui
HEX        = $(CP) -O ihex
BIN        = $(CP) -O binary -S
DIS		   = $(DUMP) -D -S 
MCU        = cortex-m3
 
# List all C defines here
DDEFS =

# Define optimisation level here
OPT = -O3

# Define project name and Ram/Flash mode here
PROJECT	= easyOS

# Source directories
LINKERSCRIPT	= ./linkerscript/stm32_flash.ld
APP_INC_DIR	= ./app/inc
APP_INC_DIR += ./user_lib/inc
APP_INC_DIR += ./kernel/inc

# APPLICATION SPECIFIC
SRC  = ./app/src/main.c
SRC  += ./app/src/stm32_configuration.c
SRC  += ./user_lib/src/commands.c
SRC  += ./user_lib/src/incomplete_shell.c
SRC  += ./user_lib/src/incom_sh_test.c
SRC  += ./user_lib/src/mpu9250_lib.c
SRC  += ./user_lib/src/sensfusion.c
SRC  += ./user_lib/src/stm32f10x_it.c
SRC  += ./user_lib/src/stm32_i2c_lib.c
SRC  += ./user_lib/src/stm32_init.c
SRC  += ./user_lib/src/stm32_usart_lib.c
SRC  += ./user_lib/src/user_lib.c
SRC  += ./user_lib/src/serial.c
SRC  += ./user_lib/src/init.c
SRC  += ./user_lib/src/timer.c
SRC  += ./kernel/src/task.c
SRC  += ./kernel/src/init.c
SRC  += ./kernel/src/scheduler.c
SRC  += ./kernel/src/bitmap.c
SRC  += ./kernel/src/event.c
SRC  += ./kernel/src/mailbox.c
SRC  += ./syscmd/src/cmd_listtask.c
ASM_SRC = ./kernel/src/cpu.s

INCDIRS = $(APP_INC_DIR)

# include submakefiles here
include makefile_std_lib.mk # STM32 Standard Peripheral Library


INCDIR  = $(patsubst %,-I%, $(INCDIRS))

## run from Flash
DEFS	= $(DDEFS) -DRUN_FROM_FLASH=1

OBJS  	= $(ASM_SRC:.s=.o) $(SRC:.c=.o)

DEPS	= $(ASM_SRC:.s=.d) $(SRC:.c=.d)

MCFLAGS = -mcpu=$(MCU)

ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst) 
CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -fno-strict-aliasing -Wall -Werror -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)

# "-Xlinker --gc-sections" - removes unused code from the output binary - saves memory
#LDFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb -nostartfiles -Xlinker --gc-sections -T$(LINKERSCRIPT) -Wl,-Map=$(PROJECT).map $(LIBDIR) $(LIB)
LDFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb -nostartfiles -Xlinker --gc-sections -T$(LINKERSCRIPT) -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch $(LIBDIR) $(LIB)

SOURSES += $(SRC) 
SOURSES += $(ASM_SRC)	


#
# makefile rules
# 
all: $(OBJS) $(PROJECT).elf  $(PROJECT).hex $(PROJECT).bin $(PROJECT).dis
	$(CCPREFIX)size $(PROJECT).elf
 
%o: %c 
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

%o: %s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(OBJS)
	$(CC) -v $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@
	
%bin: %elf
	$(BIN)  $< $@

%dis: %elf
	$(DIS) $< > $@

ctags: 
	ctags -R .
	
flash_openocd: $(PROJECT).bin
	openocd -s ~/opt/openocd/share/openocd/scripts/ -f interface/jlink.cfg -f target/stm32f0x.cfg -c "init" -c "reset halt" -c "sleep 100" -c "wait_halt 2" -c "flash write_image erase $(PROJECT).bin 0x08000000" -c "sleep 100" -c "verify_image $(PROJECT).bin 0x08000000" -c "sleep 100" -c "reset run" -c shutdown
	> flashed

#flash_stlink: $(PROJECT).bin
#	st-flash write $(PROJECT).bin 0x8000000

#erase_openocd:
#	openocd -s ~/EmbeddedArm/openocd-bin/share/openocd/scripts/ -f interface/stlink-v2.cfg -f target/stm32f0x_stlink.cfg -c "init" -c "reset halt" -c "sleep 100" -c "stm32f1x mass_erase 0" -c "sleep 100" -c shutdown 

#erase_stlink:
#	st-flash erase

debug_openocd: $(PROJECT).elf flashed
	xterm -e openocd -s ~/opt/openocd/share/openocd/scripts/ -f interface/jlink.cfg -f target/stm32f0x.cfg -c "init" -c "halt" -c "reset halt" &
	$(GDB) --eval-command="target remote localhost:3333" $(PROJECT).elf 

debug_clewn: $(PROJECT).elf flashed
	xterm -e openocd -s ~/opt/openocd/share/openocd/scripts/ -f interface/jlink.cfg -f target/stm32f0x.cfg -c "init" -c "halt" -c "reset halt" & clewn -gc $(GDB) -ga --eval-command="target remote localhost:3333" -ga $(PROJECT).elf

#debug_stlink: $(PROJECT).elf
#	xterm -e st-util &
#	$(GDBTUI) --eval-command="target remote localhost:4242"  $(PROJECT).elf -ex 'load'

flashed: $(PROJECT).bin
	openocd -s ~/opt/openocd/share/openocd/scripts/ -f interface/jlink.cfg -f target/stm32f0x.cfg -c "init" -c "reset halt" -c "sleep 500" -c "wait_halt 2" -c "flash write_image erase $(PROJECT).bin 0x08000000" -c "sleep 500" -c "verify_image $(PROJECT).bin 0x08000000" -c "sleep 500" -c "reset run" -c shutdown
	> $@
		
clean:
	-rm -rf $(OBJS)
	-rm -rf $(DEPS)
	-rm -rf $(PROJECT).elf
	-rm -rf $(PROJECT).map
	-rm -rf $(PROJECT).hex
	-rm -rf $(PROJECT).bin
	-rm -rf $(PROJECT).dis
	-rm -rf $(SRC:.c=.lst)
	-rm -rf $(ASRC:.s=.lst)

include $(DEPS)

# auto generate dependency
%d: %c
	$(CC) -M $(CPFLAGS) $(INCDIR) $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,$(subst .d,.o,$@) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%d: %s
	$(CC) -M $(ASFLAGS) $(INCDIR) $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,$(subst .d,.o,$@) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# *** EOF ***
