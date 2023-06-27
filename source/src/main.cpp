/**
 * @file main.cpp
 *
 * @brief ZED-F9P implementation
 *
 (c) 2023 Forstner Michael and its subsidiaries.

	 Subject to your compliance with these terms,you may use this software and
     any derivatives exclusively with Forstner Michael products.It is your responsibility
     to comply with third party license terms applicable to your use of third party
     software (including open source software) that may accompany Forstner Michael software.

     THIS SOFTWARE IS SUPPLIED BY Forstner Michael "AS IS". NO WARRANTIES, WHETHER
     EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
     WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
     PARTICULAR PURPOSE.

     IN NO EVENT WILL Forstner Michael BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
     INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
     WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF Forstner Michael HAS
     BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
     FULLEST EXTENT ALLOWED BY LAW, Forstner Michael'S TOTAL LIABILITY ON ALL CLAIMS IN
     ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
     THAT YOU HAVE PAID DIRECTLY TO Forstner Michael FOR THIS SOFTWARE.
 *
 */

/**
 * @addtogroup main
 *
 * @section Revision History
 * - V1.0 Initial release
 */

#include <Arduino.h>
#include <M5Core2.h>
#include <esp_task_wdt.h>
#include <SparkFun_u-blox_GNSS_v3.h>
#include "main.h"
#include "sd_card.h"
#include "real_time_clock.h"
#include "battery.h"
#include "display.h"
#include "tough.h"
#include "gnss.h"
#include "bluetooth_serial.h"
#include "wlan_client.h"
#include "assist_now_client.h"
#include "ntrip_client.h"
#include "page.h"
#include "led_bar.h"

TaskHandle_t subtask1_handle = NULL;
TaskHandle_t subtask2_handle = NULL;
portMUX_TYPE subtask2_taskmux = portMUX_INITIALIZER_UNLOCKED;
struct sd_card_config1 *sd_card_config1_data;
struct sd_card_config2 *sd_card_config2_data;
struct real_time_clock *real_time_clock_data;
struct battery *battery_data;
struct gnss *gnss_data;
struct bluetooth_serial *bluetooth_serial_data;
struct wlan_client *wlan_client_data;
struct assist_now_client *assist_now_client_data;
struct ntrip_client *ntrip_client_data;
bool wlan_client_active = false;
bool assist_now_client_active = false;
bool ntrip_client_active = false;
extern int button;

/**
 * @brief setup program
 */
void setup(void)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 

    M5.begin(true, true, true, true, kMBusModeOutput);
    Serial.print(F("ZED-F9P V1.0 startup... ok\n"));
    Serial.print(F("Initialize watchdog... "));
    if (esp_task_wdt_init(10, true) == ESP_OK)
    { 
        if (esp_task_wdt_add(NULL) == ESP_OK) Serial.print(F("ok\n"));
        else Serial.print(F("failed\n"));
    }
    else Serial.print(F("failed\n"));
    Serial.print(F("Check SD card for startup picture... "));
    if (sd_card_check_startup() == true)
    {
        Serial.print(F("failed\n"));
        page_error(0);
    }
    else
    {
        Serial.print(F("ok\n"));
        tft.createSprite(320, 240);
        tft.drawJpgFile(SD, "/icons/startup.jpg");
        tft.pushSprite(0, 0);
        tft.deleteSprite();
    }
    Serial.print(F("Check SD card... "));
    if (sd_card_check() == true)
    {
        Serial.print(F("failed\n"));
        page_error(1);
    }
    else Serial.print(F("ok\n"));
    Serial.print(F("Read config file... "));
    sd_card_config1_data = (struct sd_card_config1 *)malloc(sizeof(struct sd_card_config1));
    sd_card_config2_data = (struct sd_card_config2 *)malloc(sizeof(struct sd_card_config2));
    if (sd_card_config_read(sd_card_config1_data, sd_card_config2_data) == true)
    {
        Serial.print(F("failed\n"));
        page_error(1);
    }
    else Serial.print(F("ok\n"));
    Serial.print(F("Initialize real time clock... ok\n"));
    real_time_clock_data = (struct real_time_clock *)malloc(sizeof(struct real_time_clock));
    real_time_clock_init(real_time_clock_data, sd_card_config1_data);
    Serial.print(F("Initialize battery... ok\n"));
    battery_data = (struct battery *)malloc(sizeof(struct battery));
    battery_init(battery_data);
    Serial.print(F("Initialize display... ok\n"));
    display_init(sd_card_config1_data);
    Serial.print(F("Initialize touch... ok\n"));
    touch_init();
    Serial.print(F("Initialize GNSS... "));
    gnss_data = (struct gnss *)malloc(sizeof(struct gnss));
    if (gnss_init(gnss_data) == true)
    {
        Serial.print(F("failed\n"));
        page_error(2);
    }
    else Serial.print(F("ok\n"));
    Serial.print(F("Initialize Bluetooth serial... "));
    bluetooth_serial_data = (struct bluetooth_serial *)malloc(sizeof(struct bluetooth_serial));
    if (bluetooth_serial_init(bluetooth_serial_data) == true)
    {
        Serial.print(F("failed\n"));
        page_error(3);
    }
    else Serial.print(F("ok\n"));
    Serial.print(F("Initialize led bar... ok\n"));
    led_bar_init();
    Serial.print(F("Starting subtask 1... "));
    if (xTaskCreatePinnedToCore(subtask1, "SUBTASK1", 5000, NULL, 1, &subtask1_handle, 1) == 0)
    {
        Serial.print(F("failed\n"));
        page_error(4);
    }
    else Serial.print(F("ok\n"));
    Serial.print(F("Starting subtask 2... "));
    wlan_client_data = (struct wlan_client *)malloc(sizeof(struct wlan_client));
    wlan_client_data->active = false;
    assist_now_client_data = (struct assist_now_client *)malloc(sizeof(struct assist_now_client));
    assist_now_client_data->active = false;
    ntrip_client_data = (struct ntrip_client *)malloc(sizeof(struct ntrip_client));
    ntrip_client_data->active = false;
    if (xTaskCreatePinnedToCore(subtask2, "SUBTASK2", 10000, NULL, 1, &subtask2_handle, 1) == 0)
    {
        Serial.print(F("failed\n"));
        page_error(4);
    }
    else Serial.print(F("ok\n"));
    M5.Spk.DingDong();
}

/**
 * @brief main program
 */
void loop(void)
{
    bool display_on = true;
    
    while(1)
    {
        esp_task_wdt_reset(); 
        M5.update();
        display_on = display_timer(sd_card_config1_data, &button);
        real_time_clock_transfer(real_time_clock_data);
        battery_transfer(battery_data);
        bluetooth_serial_transfer(bluetooth_serial_data);
        wlan_client_transfer(wlan_client_data);
        assist_now_client_transfer(assist_now_client_data);
        ntrip_client_transfer(ntrip_client_data);
        gnss_transfer(gnss_data);
        if (display_on == true) page(sd_card_config1_data, &button, real_time_clock_data, battery_data, gnss_data, bluetooth_serial_data, wlan_client_data, assist_now_client_data, ntrip_client_data);
    }
}

/**
 * @brief Subtask 1 for background processes
 * @param [in] parameter
 */
void subtask1(void *parameter) 
{ 
    unsigned long curr_millis = 0;
    static unsigned long last_millis = (unsigned long)(millis() - 3540000);

    while(1)
    {     
        esp_task_wdt_reset();
        curr_millis = millis();
        gnss(gnss_data);
        if ((unsigned long)(curr_millis - last_millis) > 3600000)
        {
            if (real_time_clock_set() == false) last_millis = curr_millis;
        }
    }
}

/**
 * @brief Subtask 2 for background processes
 * @param [in] parameter
 */
void subtask2(void *parameter) 
{ 
    bool bluetooth_serial_error = true;
    bool wlan_client_error = true;
    bool assist_now_client_error = true;
    bool ntrip_client_error = true;
    unsigned long curr_millis = 0;
    static unsigned long last_millis = (unsigned long)(millis() - 50000);

    while(1)
    {
        esp_task_wdt_reset();   
        bluetooth_serial();
        curr_millis = millis();
        if ((unsigned long)(curr_millis - last_millis) > 60000)
        {
            if (wlan_client_error == true)
            {
                Serial.print(F("Initialize WLAN client... "));
                wlan_client_error = wlan_client(sd_card_config2_data);
                if (wlan_client_error == true) Serial.print(F("failed\n"));
                else Serial.print(F("ok\n"));
            }
            last_millis = curr_millis;
        }
        if (wlan_client_error == false)
        {
            wlan_client_error = wlan_client(sd_card_config2_data);
            if (wlan_client_error == true)
            {
                Serial.print(F("Disconnect WLAN client... ok\n"));
                assist_now_client_error = true;
                ntrip_client_error = true;
            }
        }

        portENTER_CRITICAL(&subtask2_taskmux);
        wlan_client_active = !wlan_client_error;
        portEXIT_CRITICAL(&subtask2_taskmux);

        if (wlan_client_error == false)
        {
            if (assist_now_client_error == true)
            {
                Serial.print(F("Initialize AssistNow client... "));
                assist_now_client_error = assist_now_client(sd_card_config2_data);
                if (assist_now_client_error == true) Serial.print(F("failed\n"));
                else Serial.print(F("ok\n"));

                portENTER_CRITICAL(&subtask2_taskmux);
                assist_now_client_active = !assist_now_client_error;
                portEXIT_CRITICAL(&subtask2_taskmux);  
            }

            if (ntrip_client_error == true)
            {
                Serial.print(F("Initialize NTRIP client... ")); 
                ntrip_client_error = ntrip_client_init(sd_card_config2_data);
                if (ntrip_client_error == true) Serial.print(F("failed\n"));
                else Serial.print(F("ok\n"));
            }
            else
            {
                ntrip_client_error = ntrip_client();
                if (ntrip_client_error == true) Serial.print(F("Disconnect NTRIP client... ok\n"));
            }
        }
        
        portENTER_CRITICAL(&subtask2_taskmux);
        assist_now_client_active = !assist_now_client_error;
        ntrip_client_active = !ntrip_client_error;
        portEXIT_CRITICAL(&subtask2_taskmux);
    }
}
