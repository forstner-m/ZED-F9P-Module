/**
 * @file ntrip_client.cpp
 *
 * @brief NTRIP client related functionality implementation.
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
#include <WiFiClient.h>
#include <Base64.h>
#include "ntrip_client.h"
#include "sd_card.h"
#include "gnss.h"
#include "assist_now_client.h"

unsigned long timeout = 0;
extern WiFiClient wifi_client;
extern portMUX_TYPE subtask2_taskmux; 
extern SFE_UBLOX_GNSS_SERIAL gnss_serial;
extern bool ntrip_client_active;

/**
 * @brief Transfer data from the NTRIP client
 * @param [in] wlan_client_data
 */
void ntrip_client_transfer(struct ntrip_client *ntrip_client_data)
{
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
 
    esp_task_wdt_reset();
    curr_millis = millis();
    if ((unsigned long)(curr_millis - last_millis) > 1000)
    {
        portENTER_CRITICAL(&subtask2_taskmux);
        if (ntrip_client_data->active != ntrip_client_active)
        {
            ntrip_client_data->active = ntrip_client_active;
            ntrip_client_data->update = true;
        }
        portEXIT_CRITICAL(&subtask2_taskmux);
        last_millis = curr_millis;
    }
}

/**
 * @brief Communication from the NTRIP client
 * @param [in] sd_card_config2_data
 * @return error
 */
bool ntrip_client(void)
{
    bool error = false;
    uint16_t counter = 0;
    char data[2048];

    esp_task_wdt_reset();
    if (wifi_client.connected() == true)
    {
        counter = 0;
        while (wifi_client.available() > 0)
        {
            data[counter] = (char)wifi_client.read();
            counter = counter + 1;
            if (counter == sizeof(data))
            {
                error = true;
                break;
            }
        }
        if ((counter > 0) && (error == false)) error = !gnss_serial.pushRawData((uint8_t *)data, counter, true);
        if ((unsigned long)(millis() - timeout) > 10000) error = true;
    }
    else error = true;
    if (error == true)
    {
        if (wifi_client.connected() == true) wifi_client.stop();
        delay(5000);
    }
    else timeout = millis();

    return error;
}

/**
 * @brief Initialize the NTRIP client
 * @param [in] sd_card_config2_data
 * @return error
 */
bool ntrip_client_init(struct sd_card_config2 *sd_card_config2_data)
{
    bool error = false;
    uint16_t counter = 0;
    char data[512];
    char temp[128];
    char user_credentials[sizeof(sd_card_config2_data->ntrip_user) + sizeof(sd_card_config2_data->ntrip_password) + 1];
    base64 base;
    String encoded_credentials_str;

    esp_task_wdt_reset();
    if (wifi_client.connect(sd_card_config2_data->ntrip_server, sd_card_config2_data->ntrip_port) == true) 
    {
        snprintf(data, sizeof(data), "GET /%s HTTP/1.0\r\n", sd_card_config2_data->ntrip_mount_point);
        snprintf(temp, sizeof(temp), "User-Agent: M5Stack Core2\r\n");
        strncat(data, temp, sizeof(temp));
        if (strlen(sd_card_config2_data->ntrip_user) == 0)
        {
            snprintf(temp, sizeof(temp), "Accept: */*\r\n");
            strncat(data, temp, sizeof(temp));
            snprintf(temp, sizeof(temp), "Connection: close\r\n");
            strncat(data, temp, sizeof(temp));
        }
        else
        {
            snprintf(user_credentials, sizeof(user_credentials), "%s:%s", sd_card_config2_data->ntrip_user, sd_card_config2_data->ntrip_password);
            encoded_credentials_str = base.encode(user_credentials);
            char encoded_credentials[encoded_credentials_str.length() + 1];
            encoded_credentials_str.toCharArray(encoded_credentials, sizeof(encoded_credentials));
            snprintf(temp, sizeof(temp), "Authorization: Basic %s\r\n", encoded_credentials);
            strncat(data, temp, sizeof(data) - 1);
        }
        wifi_client.write(data, strlen(data));
        wifi_client.print("\r\n");
        if (wifi_client.connected() == true)
        {
            timeout = millis();
            do
            {
                if ((unsigned long)(millis() - timeout) > 5000) error = true;
                else error = false;
            }
            while ((wifi_client.available() == 0) && (error == false));
            if (error == false)
            {
                counter = 0;
                while (wifi_client.available() > 0)
                {
                    data[counter] = (char)wifi_client.read();
                    counter = counter + 1;
                    if (strstr(data, "200 OK\r\n") != nullptr)
                    {
                        error = false;
                        break;
                    }
                    else error = true;
                    if (counter == sizeof(data))
                    {
                        error = true;
                        break;
                    }
                }
            }
        }
        else error = true;
    }
    else error = true;
    if (error == true)
    {
        if (wifi_client.connected() == true) wifi_client.stop();
        delay(5000);
    }
    else timeout = millis();

    return error;
}
