GNU_PATH:=
GNU_PREF:=arm-none-eabi-

DNLD_PORT:=com1

CC:=$(GNU_PATH)$(GNU_PREF)gcc
AS:=$(GNU_PATH)$(GNU_PREF)as
LD:=$(GNU_PATH)$(GNU_PREF)ld
OBJCOPY:=$(GNU_PATH)$(GNU_PREF)objcopy
SIZE:=$(GNU_PATH)$(GNU_PREF)size
RM:=rm
ECHO:=/bin/echo -e
MKIMG:=tools/mkimg
UPLOAD:=tools/upload

TARGET:=upccar


SOURCES:=$(wildcard ./src/*.c) $(TARGET).c
OBJECTS:=$(SOURCES:%.c=%.o) startup_lpc18xx.o
ASM:=$(SOURCES:%.c=%.s)

CC_OPT:=-c -std=c99 -mcpu=cortex-m3 -nostartfiles -nodefaultlibs -mthumb -Wno-all \
-Os -mapcs-frame -fno-builtin $(DEBUG) -D__thumb2__=1  -msoft-float \
-mno-sched-prolog -fno-hosted -march=armv7-m -mfix-cortex-m3-ldrd \
-ffunction-sections -fdata-sections -I./inc \

CC_OPT_UNOP:=-c -std=c99 -mcpu=cortex-m3 -nostartfiles -nodefaultlibs -mthumb -Wno-all \
-O0 -mapcs-frame -fno-builtin -D__thumb2__=1  -msoft-float \
-mno-sched-prolog -fno-hosted -march=armv7-m -mfix-cortex-m3-ldrd \
-ffunction-sections -fdata-sections -I./inc \

CC_OPT_SPDOP:=-c -std=c99 -mcpu=cortex-m3 -nostartfiles -nodefaultlibs -mthumb -Wno-all \
-O3 -mapcs-frame -fno-builtin -D__thumb2__=1  -msoft-float \
-mno-sched-prolog -fno-hosted -march=armv7-m -mfix-cortex-m3-ldrd \
-ffunction-sections -fdata-sections -I./inc \

CC_S_OPT:=-S -dp -mcpu=cortex-m3 -nostartfiles -nodefaultlibs -mthumb -Wno-all \
-Os -mapcs-frame -fno-builtin -D__thumb2__=1 -msoft-float \
-mno-sched-prolog -fno-hosted -march=armv7-m -mfix-cortex-m3-ldrd \
-ffunction-sections -fdata-sections -I./inc \

AS_OPT:=-mcpu=cortex-m3

LD_OPT:=-static \
-L'C:/Programs/GNUARM/lib/gcc/arm-none-eabi/4.3.3/thumb2' \
-L'C:/Programs/GNUARM/arm-none-eabi/lib/thumb2' -nostdlib --gc-sections \
-Map memory.map -T ldscript_ram_gnu.ld  

.PHONY: $(TARGET) $(TARGET)_show_info clean distclean successful upload

$(TARGET): $(TARGET).rom $(TARGET)_show_info successful

%.o: %.c
	@$(ECHO) "CC $^"
	@$(CC) $(CC_OPT) $^ -o $@

%.s: %.c
	@$(ECHO) "CC -S $^"
	@$(CC) $(CC_S_OPT) $^ -o $@

startup_lpc18xx.o: ./src/startup_lpc18xx.s
	@$(ECHO) "AS $^"
	@$(AS) $(AS_OPT) -o $@ $^

$(TARGET).elf: $(OBJECTS)
	@$(ECHO) "LD $^"
	@$(LD) $^ $(LD_OPT) -o $@

$(TARGET).bin: $(TARGET).elf
	@$(ECHO) "OBJCOPY $< -> $@"
	@$(OBJCOPY) -O binary $< $@

$(TARGET).rom: $(TARGET).bin
	@rm -f $@
	@$(ECHO) "MKIMG $^ -> $@"
	@$(MKIMG) $^ $@ >/dev/null

$(TARGET)_show_info: $(TARGET).elf
	@$(SIZE) $^

successful:
	@$(ECHO) "\n\nBuild successful"

clean:
	@$(ECHO) "RM $(OBJECTS) $(ASM) $(TARGET).elf memory.map"
	@$(RM) -f $(OBJECTS) $(ASM) $(TARGET).elf memory.map

distclean: clean
	@$(ECHO) "RM $(TARGET).bin $(TARGET).rom"
	@$(RM) -f $(TARGET).bin $(TARGET).rom

upload: $(TARGET).rom
	@$(UPLOAD) $^

