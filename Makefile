PROJECT = stm32_flash

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# CFLAGS = -mcpu=cortex-m3 -mthumb -Os -Wall -nostdlib -nostartfiles -DSTM32F103x6
CFLAGS = -IFreeRTOS/Source/include -ICore/Inc -IFreeRTOS/Source/portable/GCC/ARM_CM3

C_SRC = \
    Core/Src/main.c \
    Core/Src/freertos.c \
    Core/Src/stm32f1xx_it.c \
    Core/Src/system_stm32f1xx.c \
    Core/Src/sysmem.c \
    Core/Src/syscalls.c \
    FreeRTOS/Source/tasks.c \
    FreeRTOS/Source/queue.c \
    FreeRTOS/Source/list.c \
    FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
    FreeRTOS/Source/portable/MemMang/heap_4.c

A_SRC = startup_stm32f103x6.s

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SRC:.c=.o)))
vpath %.c $(sort $(dir $(C_SRC)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(A_SRC:.s=.o)))
vpath %.s $(sort $(dir $(A_SRC)))

BUILD_DIR = build

LDFLAGS = -T stm32f103c6tx.ld -nostartfiles -Wl,--gc-sections

ELF = $(BUILD_DIR)/$(PROJECT).elf
BIN = $(BUILD_DIR)/$(PROJECT).bin

all: $(ELF) $(BIN)

$(BUILD_DIR)/%.o: %.c
	$(CC) -c -mcpu=cortex-m3 -mthumb -DSTM32F103x6 $(CFLAGS)  -Og -Wall -fdata-sections -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s
	$(CC) -x assembler-with-cpp -c -mcpu=cortex-m3 -mthumb   -DSTM32F103x6 $(CFLAGS)  -Og -Wall -fdata-sections -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" $< -o $@

$(ELF): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(OBJECTS)  -mcpu=cortex-m3 -mthumb -specs=nano.specs -Tstm32f103c6tx.ld  -lc -lm -lnosys  -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -o $(ELF)

$(BUILD_DIR):
	@mkdir -p $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR)

flash: $(BIN)
	st-flash write $(BIN) 0x8000000

.PHONY: all clean flash
