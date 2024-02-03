/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "string.h"
#include "nvs_flash.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "inc/bt_avrc.h"

#define TAG "FIRMAo.oSPEAKER"
#define bt_device_name "FIRMAo.oSPEAKER"

#define NEXT_BTN 36
#define LAST_BTN 39
#define STOP_BTN 34
#define DIODA  32

void i2s_configuration(void);
void gpio_configuration(void);

void app_main(void)
{
    esp_err_t err; //handler błędów 
    err = nvs_flash_init();
    if(err != ESP_OK){
        ESP_LOGI(TAG, "NVS INIT PROBLEM");
    }
    
    //inicjalizacja gpios
    gpio_configuration();
    
    //konfiguracja i2c 
    //Todo DAC!!!
    i2s_configuration();


    esp_bt_controller_config_t bt_controller  = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&bt_controller);
    err = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);

    err= esp_bluedroid_init();
    err = esp_bluedroid_enable();

    err = esp_avrc_ct_register_callback(callback_bt_avrc);
    err = esp_avrc_ct_init();

    err = esp_bt_dev_set_device_name(bt_device_name);
}

void gpio_configuration(void){

    gpio_set_direction(NEXT_BTN, GPIO_MODE_INPUT);
    gpio_set_direction(LAST_BTN, GPIO_MODE_INPUT);
    gpio_set_direction(STOP_BTN, GPIO_MODE_INPUT);
    gpio_set_direction(DIODA, GPIO_MODE_OUTPUT);
}

void i2s_configuration(void){
    
    esp_err_t err;
   
    i2s_config_t i2sConfiguration = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,                          // Only TX
        .sample_rate = 44100,
        .bits_per_sample = 16,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                   //2-channels
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,              // MAX98357A I2S Philips
        .dma_buf_count = 6,
        .dma_buf_len = 60,
        .intr_alloc_flags = 0,                                          //Default interrupt priority
        .tx_desc_auto_clear = true                                      //Auto clear tx descriptor on underflow
        };
    
    err = i2s_driver_install(0,&i2sConfiguration, 0, NULL);

    if(err != ESP_OK){
        ESP_LOGI(TAG, "I2S failed start!");
    }

    i2s_pin_config_t i2s_pins_config = {
        .bck_io_num = 4,
        .ws_io_num = 5,
        .data_out_num = 18,
        .data_in_num = I2S_PIN_NO_CHANGE             
    };
    
    err = i2s_set_pin(0,&i2s_pins_config);

    if(err != ESP_OK){
        ESP_LOGI(TAG, "I2S pin config fail!");
    }

}
