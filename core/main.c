#include "FreeRTOS.h"
#include "task.h"

#define RCC_APB2ENR   (*(volatile uint32_t*)0x40021018)
#define GPIOC_CRH     (*(volatile uint32_t*)0x40011004)
#define GPIOC_ODR     (*(volatile uint32_t*)0x4001100C)

void led_init(void) {
    RCC_APB2ENR |= (1 << 4);              // Enable GPIOC
    GPIOC_CRH &= ~(0xF << 20);            // Clear MODE13, CNF13
    GPIOC_CRH |=  (0x2 << 20);            // Output mode 2 MHz
}

void led_toggle(void) {
    GPIOC_ODR ^= (1 << 13);
}

void led_task(void *param) {
    (void)param;
    while (1) {
        led_toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void) {
    led_init();
    xTaskCreate(led_task, "LED", 32, NULL, 1, NULL);
    vTaskStartScheduler();
    while (1);  // Should never reach here
}

