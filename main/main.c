#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/freeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

SemaphoreHandle_t mutex_i2c; //xSemaphoreHandle
//xSemaphoreHandle mutex_i2c;
float temperature;

float access_i2c(int cmd) {
    if(cmd==1) {
        ESP_LOGI("i2c","reading temperature sensor");
        return 20.0 * ((float) rand() / (float)(RAND_MAX/10));
    } else {
        ESP_LOGI("i2c", "writing to LCD");
        printf("LCD display - Temperature = %f", temperature);
    }
    return 0;
}

void read_sensor(void *params) {
    while(true) {
        if(xSemaphoreTake(mutex_i2c, 1000 / portTICK_PERIOD_MS)) {
            temperature = access_i2c(1); 
            ESP_LOGI("reading","LCD display - Temperature = %f", temperature);
            xSemaphoreGive(mutex_i2c);
        } else {
            ESP_LOGE("reading","Cannot take sensor reading");
        }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void lcd_display(void *params) {
    while(true) {
        if(xSemaphoreTake(mutex_i2c, 1000 / portTICK_PERIOD_MS)) {
            temperature = access_i2c(2);
            ESP_LOGI("i2c", "writing to LCD");
            xSemaphoreGive(mutex_i2c);
        } else {
            ESP_LOGE("i2c","Cannot write to sensor");
        }
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}

/*void task1(void *params)
{
    while(true) {
        //printf("Reading sensors...");
        ESP_LOGI("task1","Sensor readings"); //ESP_LOGV, ESP_LOGW, ESP_LOGE, ESP_LOGD
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task2(void *params)
{
    while(true) {
        //printf("Reading sensors...");
        ESP_LOGI("task2","Write to display"); 
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}*/

void app_main() 
{
    mutex_i2c = xSemaphoreCreateMutex();
    xTaskCreate(&read_sensor, "reading", 2048, "Task 1", 1, NULL);
    xTaskCreate(&lcd_display, "writing", 2048, "Task 2", 1, NULL);
}