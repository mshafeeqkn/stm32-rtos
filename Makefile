PROJECT = stm32_flash

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS = -mcpu=cortex-m3 -mthumb -Os -Wall -nostdlib -nostartfiles
FREERTOS_SRC = \
  FreeRTOS/Source/tasks.c \
  FreeRTOS/Source/queue.c \
  FreeRTOS/Source/list.c \
  FreeRTOS/portable/GCC/ARM_CM3/port.c

CFLAGS += -I./FreeRTOS/include -I./include -I./FreeRTOS/portable/GCC/ARM_CM3

SRC = Core/main.c Core/startup_stm32f103x6.s $(FREERTOS_SRC)

LDFLAGS = -T stm32f103c6tx.ld -nostartfiles -Wl,--gc-sections

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
