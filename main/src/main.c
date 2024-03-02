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
#include "inc/bt_a2dp.h"
#include "inc/bt_gap.h"

#define TAG "FIRMAo.oSPEAKER"
#define bt_device_name "FIRMAo.oSPEAKER"

#define NEXT_BTN 13
#define LAST_BTN 14
#define STOP_BTN 25
#define DIODA  32
#define DIODA_DIS 33
#define GPIO_SD 10

static bool next_state=false;
static bool last_state=false;


void i2s_configuration(void);
void gpio_configuration(void);
void gpio_task(void * pvParameteres);

static void next_action(void *param);
static void last_action(void *param);

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

    err = esp_a2d_register_callback(callback_bt_a2d);
    err = esp_a2d_sink_register_data_callback(bt_a2d_data_i2s);
    err = esp_a2d_sink_init();

    err = esp_bt_gap_register_callback(bt_gap);
    err = esp_bt_dev_set_device_name(bt_device_name);
    esp_bt_io_cap_t io_cap=ESP_BT_IO_CAP_IO;
    err = esp_bt_gap_set_security_param(ESP_BT_SP_IOCAP_MODE, &io_cap, sizeof(io_cap));
    err = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

    

    while(1)
    {
    
         vTaskDelay(10);

    }


}




void gpio_configuration(void){
    //diody do komunikacji statusu połączenia
    gpio_set_direction(DIODA, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIODA_DIS, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_SD, GPIO_MODE_OUTPUT);
    
    gpio_set_level(GPIO_SD, 1);
}

void i2s_configuration(void){
    
    esp_err_t err;
   
    i2s_config_t i2sConfiguration = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,                          // Only TX
        .sample_rate = 44100,
        .bits_per_sample = 16,
        .channel_format = I2S_CHANNEL_STEREO,                   //2-channels
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,              // MAX98357A I2S Philips
        .dma_buf_count = 8,
        .dma_buf_len = 64,
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
    i2s_set_sample_rates(0, 22050); //set sample rates
}
