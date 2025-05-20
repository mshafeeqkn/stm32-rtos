#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "queue.h"

#define NUM_SAMPLES     100

typedef struct {
    uint16_t temp;
    uint16_t light;
} sensor_data_t;

static QueueHandle_t sensor_queue;

void handle_error() {
    GPIOC->ODR &= ~GPIO_ODR_ODR13;
}

void clear_error() {
    GPIOC->ODR |= GPIO_ODR_ODR13;
}

void led_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;                     // Enable GPIOC
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);      // Clear MODE13, CNF13
    GPIOC->CRH |=  GPIO_CRH_MODE13_1;                       // Output mode 2 MHz
    GPIOC->ODR |=  GPIO_ODR_ODR13;
}

void delay(volatile uint32_t count) {
    while (count--);
}

void adc_init() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // GPIOA1 analog input mode
    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);

    // Document RM0008 Section 11.3.1
    // The ADC can be powered-on by setting the ADON bit in the
    // ADC_CR2 register. When the ADON bit is set for the first
    // time, it wakes up the ADC from Power Down mode. Conversion
    // starts when ADON bit is set for a second time by software
    // after ADC power-up time (giving ~100ms)
    ADC1->CR2 |= ADC_CR2_ADON;
    delay(100);
    ADC1->CR2 |= ADC_CR2_ADON;

    // Caliberate ADC after each power up
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL);
}

uint16_t get_adc_val(uint8_t chan) {
    ADC1->SQR3 = chan;                  // Select channel
    ADC1->CR2 |= ADC_CR2_ADON;          // Enable ADC
    while(!(ADC1->SR & ADC_SR_EOC));    // Wait until the End Of Conversion flag is set
    return ADC1->DR;                    // Get the data from ADC Data Register
}

BaseType_t send_sensor_data(sensor_data_t *data) {
    return xQueueSend(sensor_queue, data, pdMS_TO_TICKS(100));
}

BaseType_t receive_sensor_data(sensor_data_t *data) {
    return xQueueReceive(sensor_queue, data, pdMS_TO_TICKS(100));
}

void task_sensor_data(void *param) {
    (void)param;
    sensor_data_t data;
    while (1) {
        data.temp = 1500; // get_adc_val(1);
        data.light = 2000; // get_adc_val(2);
        if (send_sensor_data(&data) != pdTRUE) {
            handle_error();
            vTaskDelay(pdMS_TO_TICKS(3000));
            clear_error();
        } else {
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

static sensor_data_t sensor_data[NUM_SAMPLES];

void task_processor(void *param) {
    sensor_data_t data;
    uint8_t index = 0;
    uint32_t temp_sum = 0;
    uint32_t light_sum = 0;
    uint32_t temp_thres = 100 * NUM_SAMPLES;
    uint32_t light_thres = 100 * NUM_SAMPLES;

    while(1) {
        if(receive_sensor_data(&data) == pdTRUE) {
            if(sensor_data[index].temp != 0) {
                temp_sum -= sensor_data[index].temp;
            }

            if(sensor_data[index].light != 0) {
                light_sum -= sensor_data[index].light;
            }

            temp_sum += data.temp;
            light_sum += data.light;

            if(temp_sum > temp_thres || light_sum > light_thres) {
                // send alert to alert thread
            }

            sensor_data[index].temp = data.temp;
            sensor_data[index].light = data.light;
            index++;
            if(index >= NUM_SAMPLES) {
                index %= NUM_SAMPLES;
            }
        }
    }
}

void task_alert(void *param) {
    while(1);
}

void task_controller(void *param) {
    while(1);
}

int main(void) {
    led_init();

    // create queue to send data from task_sensor_data
    // to task_processor.
    sensor_queue = xQueueCreate(10, sizeof(sensor_data_t));
    if(sensor_queue == NULL) {
        handle_error();
        while(1);
    }

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

