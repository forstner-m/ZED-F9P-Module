/**
 * @file wlan_client.cpp
 *
 * @brief WLAN client related functionality implementation.
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

#include <Arduino.h>
#include <M5Core2.h>
#include <esp_task_wdt.h>
#include <SparkFun_u-blox_GNSS_v3.h>
#include <WiFi.h>
#include "wlan_client.h"
#include "sd_card.h"

extern portMUX_TYPE subtask2_taskmux; 
extern bool wlan_client_active;

/**
 * @brief Transfer data from the WLAN client
 * @param [in] wlan_client_data
 */
void wlan_client_transfer(struct wlan_client *wlan_client_data)
{
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
 
    esp_task_wdt_reset();
    curr_millis = millis();
    if ((unsigned long)(curr_millis - last_millis) > 1000)
    {
        portENTER_CRITICAL(&subtask2_taskmux);
        if (wlan_client_data->active != wlan_client_active)
        {
            wlan_client_data->active = wlan_client_active;
            wlan_client_data->update = true;
        }
        portEXIT_CRITICAL(&subtask2_taskmux);
        last_millis = curr_millis;
    }
}

/**
 * @brief Communication from the WLAN client
 * @param [in] sd_card_config2_data
 * @return error
 */
bool wlan_client(struct sd_card_config2 *sd_card_config2_data)
{
    bool error = false;
    uint8_t counter = 0;

    esp_task_wdt_reset();
    if (WiFi.status() != WL_CONNECTED) 
    {
        WiFi.begin(sd_card_config2_data->wlan_ssid, sd_card_config2_data->wlan_password);
        do
        {
            counter = counter + 1;
            delay(500);
        }
        while ((WiFi.status() != WL_CONNECTED) && (counter < 5));
        if (WiFi.status() == WL_CONNECTED) error = false;
        else error = true;
    }
    else error = false;

    return error;
}
