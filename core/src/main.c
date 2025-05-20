#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

void led_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;                     // Enable GPIOC
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);      // Clear MODE13, CNF13
    GPIOC->CRH |=  GPIO_CRH_MODE13_1;                       // Output mode 2 MHz
}

void task_sensor_data(void *param) {
    (void)param;
    vTaskDelay(pdMS_TO_TICKS(1));
    while (1) {
        GPIOC->ODR &= ~GPIO_ODR_ODR13;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_processor(void *param) {
    while(1);
}

void task_alert(void *param) {
    while(1);
}

void task_controller(void *param) {
    while(1);
}

int main(void) {
    led_init();

    // Task to read the sensor and send
    // it to the task_processor
    xTaskCreate(task_sensor_data, "sensor_data", 128, NULL, 1, NULL);

    // Receive data from task_sensor_data and 
    // process it. And inform task_alert if the average
    // of temp or light go beyond thresold value.
    xTaskCreate(task_processor, "processor", 128, NULL, 1, NULL);

    // alert immediately when by an LED and
    // buzzer, log the information via UART
    xTaskCreate(task_alert, "alert", 128, NULL, 1, NULL);

    // Listen for an interrupt using a key
    // and switch between power saving mode
    // and normal mode. where the alert thread
    // should not log via UART, read light sensor
    // data only. the average should be taken from
    // last 10 value and this is to be done from alert
    // task. the task_processor should be suspended completely
    xTaskCreate(task_controller, "controller", 128, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);  // Should never reach here
}

