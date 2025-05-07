PROJECT = stm32_flash

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS = -mcpu=cortex-m3 -mthumb -Os -Wall -nostdlib -nostartfiles
CFLAGS += -I./FreeRTOS/include -I./include -I./FreeRTOS/portable/GCC/ARM_CM3

FREERTOS_SRC = \
  FreeRTOS/Source/tasks.c \
  FreeRTOS/Source/queue.c \
  FreeRTOS/Source/list.c \
  FreeRTOS/portable/GCC/ARM_CM3/port.c \
  FreeRTOS/portable/mem_mgmt/heap_4.c
# FREERTOS_SRC =

SRC = src/main.c src/freertos.c src/startup_stm32f103x6.s $(FREERTOS_SRC)
BUILD_DIR = build

SRC_C = $(filter %.c,$(SRC))
SRC_S = $(filter %.s,$(SRC))

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_C)) \
      $(patsubst %.s,$(BUILD_DIR)/%.o,$(SRC_S))

LDFLAGS = -T stm32f103c6tx.ld -nostartfiles -Wl,--gc-sections

ELF = $(BUILD_DIR)/$(PROJECT).elf
BIN = $(BUILD_DIR)/$(PROJECT).bin

all: $(BUILD_DIR) $(BIN)

$(BUILD_DIR):
	mkdir -p $@

$(ELF): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo $(SRC)
	rm -rf $(BUILD_DIR)

flash: $(BIN)
	st-flash write $(BIN) 0x8000000

.PHONY: all clean flash
