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

void clock_init(void) {
    // 1. Enable HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)); // Wait till HSE is ready

    // 2. Configure flash latency
    FLASH->ACR |= FLASH_ACR_PRFTBE;       // Enable prefetch buffer
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;    // 2 wait states

    // 3. Set PLL multiplier to 9 (8 MHz * 9 = 72 MHz)
    RCC->CFGR &= ~(RCC_CFGR_PLLMULL);     // Clear PLL multiplier bits
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    // 4. Set PLL source to HSE
    RCC->CFGR |= RCC_CFGR_PLLSRC;         // HSE oscillator clock selected as PLL input

    // 5. Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));   // Wait for PLL ready

    // 6. Set AHB, APB1, and APB2 prescalers
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;      // AHB = SYSCLK
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;     // APB1 = SYSCLK/2 (max 36 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;     // APB2 = SYSCLK

    // 7. Select PLL as system clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Wait for PLL used as system clock
}

void led_task(void *param) {
    (void)param;
    while (1) {
        led_toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void delay(volatile uint32_t count) {
    while (count--);
}

int main(void) {
    clock_init();
    led_init();
	while(0) {
		led_toggle();
		delay(1000000);
	}
    xTaskCreate(led_task, "LED", 128, NULL, 1, NULL);
    vTaskStartScheduler();
    while (1);  // Should never reach here
}

