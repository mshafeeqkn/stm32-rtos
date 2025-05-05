#include <stdint.h>

#define RCC_APB2ENR   (*(volatile uint32_t*)0x40021018)
#define GPIOC_CRH     (*(volatile uint32_t*)0x40011004)
#define GPIOC_ODR     (*(volatile uint32_t*)0x4001100C)

void delay(volatile uint32_t count) {
    while (count--);
}

int main(void) {
    // Enable GPIOC clock
    RCC_APB2ENR |= (1 << 4);

    // Configure PC13 as output (MODE = 0b10, CNF = 0b00)
    GPIOC_CRH &= ~(0xF << 20); // Clear MODE13 and CNF13
    GPIOC_CRH |=  (0x2 << 20); // MODE13 = 0b10 (2 MHz output)

    while (1) {
        GPIOC_ODR ^= (1 << 13);  // Toggle LED
        delay(1500000);
    }
}

