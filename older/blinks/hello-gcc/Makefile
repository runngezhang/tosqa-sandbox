XAS = arm-none-eabi-gcc -c -mthumb -mcpu=cortex-m3 -mthumb-interwork -I. \
	-x assembler-with-cpp -g -D__ASSEMBLY__ -DVECT_TAB_FLASH -DFLASH_RUN \
	-DLPC1758 -DUSER -Wa,-g

XCC = arm-none-eabi-gcc -c -mthumb -g -Os -mcpu=cortex-m3 -mthumb-interwork \
	-I. -DVECT_TAB_FLASH -DFLASH_RUN -DLPC1758 -DUSER -I. -ffunction-sections \
	-fdata-sections -Wall -Wextra -Wimplicit -Wcast-align -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wcast-qual -Wcast-align -MMD -MP \
	-Wnested-externs -std=gnu99

XLD = arm-none-eabi-gcc -mthumb -g -Os -mcpu=cortex-m3 -mthumb-interwork \
	-DVECT_TAB_FLASH -DFLASH_RUN -DLPC1758 -DUSER -I. -ffunction-sections \
	-fdata-sections -Wall -Wextra -Wimplicit -Wcast-align -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wcast-qual -Wcast-align -MMD -MP startup.o \
	-nostartfiles -Wl,-Map=firmware.map,--cref,--gc-sections -lgcc -lc -lm -L. \
	-T./lpc1758_flash.ld

OBJ = main.o core_cm3.o system_LPC17xx.o

all: firmware.bin

firmware.elf: $(OBJ) startup.o
	# XLD $(OBJ) -o $@
	@$(XLD) $(OBJ) -o $@
	@arm-none-eabi-size firmware.elf

.SUFFIXES:
.SUFFIXES: .o .c .S .elf .hex .bin .lss .sym

.c.o:
	# XCC $? -o $@
	@$(XCC) $? -o $@

.S.o:
	# XAS $? -o $@
	@$(XAS) $? -o $@
.elf.hex:
	# OBJCOPY -O ihex $? $@
	@arm-none-eabi-objcopy -O ihex $? $@
.elf.bin:
	# OBJCOPY -O binary $? $@
	@arm-none-eabi-objcopy -O binary $? $@
.elf.lss:
	# OBJDUMP -h -S -C -r $? >$@
	@arm-none-eabi-objdump -h -S -C -r $? >$@
.elf.sym:
	# NM -n $? >$@
	@arm-none-eabi-nm -n $? >$@

clean:
	rm -f *.o *.d *.elf *.hex *.bin *.lss *.sym *.map
