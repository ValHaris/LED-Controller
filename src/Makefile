###############################################################################
# Makefile for the project phasermk3++
###############################################################################
# 
# Warning, no automatic dependency calculation is done!
# 

## General Flags
PROJECT = leuchter
MCU = attiny84
TARGET = leuchter.elf
CC = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-stack-protector
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=leuchter.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = leuchter.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) leuchter.hex leuchter.eep leuchter.lss 

## Compile
leuchter.o: leuchter.cpp Makefile
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<



##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}

	@echo
	@avr-size  --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean asm
clean:
	-rm -rf $(OBJECTS) leuchter.elf dep/* leuchter.hex leuchter.eep leuchter.lss leuchter.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

programchip: leuchter.hex 
	avrdude -p t84 -c stk500v2 -e -P /dev/ttyS0 -U flash:w:leuchter.hex

readfuses: 
	avrdude -p t84 -c stk500v2 -P /dev/ttyS0 -n -v
	
writefuses:
	avrdude -p t84 -c stk500v2 -P /dev/ttyS0 -U lfuse:w:0xe2:m -U hfuse:w:0xde:m -U efuse:w:0xff:m
	 
asm: leuchter.asm

leuchter.asm: leuchter.o
	avr-objdump -d -M intel -S -l $< > $@

.PHONY: runtest

runtest: test
	./test	
	
test: testInputFilter.cpp inputfilter.h adc.h
	g++ -g -O0 $< -o $@
	