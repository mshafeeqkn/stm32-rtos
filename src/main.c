#include "FreeRTOS.h"
#include "task.h"
#include "stm32f103_defs.h"

void led_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;                     // Enable GPIOC
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);      // Clear MODE13, CNF13
    GPIOC->CRH |=  GPIO_CRH_MODE13_1;                       // Output mode 2 MHz
}

void led_toggle(void) {
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}

#if 0
void led_task(void *param) {
    (void)param;
    while (1) {
        led_toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
#endif

void delay(volatile uint32_t count) {
    while (count--);
}

int main(void) {
    led_init();

    while(1) {
        led_toggle();
        delay(150000);
    }

    // xTaskCreate(led_task, "LED", 128, NULL, 1, NULL);
    // vTaskStartScheduler();
    while (1);  // Should never reach here
}

