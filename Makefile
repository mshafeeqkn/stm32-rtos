PROJECT = stm32_flash

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size


C_SRC = \
    Core/Src/main.c \
    Core/Src/freertos.c \
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

COM_FLAGS = -mcpu=cortex-m3 -mthumb
OPTS = -Og -Wall
C_INC = -IFreeRTOS/Source/include -ICore/Inc -IFreeRTOS/Source/portable/GCC/ARM_CM3
CFLAGS = $(COM_FLAGS) -DSTM32F103x6 $(C_INC) $(OPTS) -MMD -MP -MF"$(@:%.o=%.d)"  -fdata-sections -ffunction-sections
LDFLAGS = $(COM_FLAGS) -Tstm32f103c6tx.ld -specs=nano.specs -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

ELF = $(BUILD_DIR)/$(PROJECT).elf
BIN = $(BUILD_DIR)/$(PROJECT).bin

all: $(ELF) $(BIN)

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s
	$(CC) -x assembler-with-cpp -c $(CFLAGS) $< -o $@

$(ELF): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(OBJECTS)  $(LDFLAGS)  -lc -lm -lnosys -o $(ELF)

$(BUILD_DIR):
	@mkdir -p $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR)

flash: $(BIN)
	st-flash --reset write $(BIN) 0x8000000

.PHONY: all clean flash
