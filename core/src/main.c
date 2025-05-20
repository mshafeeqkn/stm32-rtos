#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

void led_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;                     // Enable GPIOC
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);      // Clear MODE13, CNF13
    GPIOC->CRH |=  GPIO_CRH_MODE13_1;                       // Output mode 2 MHz
}

void led_toggle(void) {
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void delay(volatile uint32_t count) {
    while (count--);
}

void led_on_task(void *param) {
    (void)param;
    vTaskDelay(pdMS_TO_TICKS(1));
    while (1) {
        GPIOC->ODR &= ~GPIO_ODR_ODR13;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void led_off_task(void *param) {
    (void)param;
    while (1) {
        GPIOC->ODR |= GPIO_ODR_ODR13;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

int main(void) {
    led_init();
    xTaskCreate(led_on_task, "LED_ON", 128, NULL, 1, NULL);
    xTaskCreate(led_off_task, "LED_OFF", 128, NULL, 1, NULL);
    vTaskStartScheduler();
    while (1);  // Should never reach here
}

