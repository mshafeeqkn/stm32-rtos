PROJECT = stm32_flash

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS = -mcpu=cortex-m3 -mthumb -Os -Wall -nostdlib -nostartfiles
CFLAGS += -I./Core

LDFLAGS = -T stm32f103c6tx.ld -nostartfiles -Wl,--gc-sections

SRC = core/main.c core/startup_stm32f103x6.s
OBJ = $(SRC:.c=.o)

BUILD_DIR = build
ELF = $(BUILD_DIR)/$(PROJECT).elf
BIN = $(BUILD_DIR)/$(PROJECT).bin

all: $(BUILD_DIR) $(BIN)

$(BUILD_DIR):
	mkdir -p $@

$(ELF): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR)

flash: $(BIN)
	st-flash write $(BIN) 0x8000000

.PHONY: all clean flash
