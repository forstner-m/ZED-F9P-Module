/**
 * @file assist_now_client.cpp
 *
 * @brief AssistNow client related functionality implementation.
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
#include "assist_now_client.h"
#include "sd_card.h"
#include "gnss.h"

WiFiClient wifi_client;
extern portMUX_TYPE subtask2_taskmux; 
extern SFE_UBLOX_GNSS_SERIAL gnss_serial;
extern bool assist_now_client_active;

/**
 * @brief Transfer data from the AssistNow client
 * @param [in] wlan_client_data
 */
void assist_now_client_transfer(struct assist_now_client *assist_now_client_data)
{
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
 
    esp_task_wdt_reset();
    curr_millis = millis();
    if ((unsigned long)(curr_millis - last_millis) > 1000)
    {
        portENTER_CRITICAL(&subtask2_taskmux);
        if (assist_now_client_data->active != assist_now_client_active)
        {
            assist_now_client_data->active = assist_now_client_active;
            assist_now_client_data->update = true;
        }
        portEXIT_CRITICAL(&subtask2_taskmux);
        last_millis = curr_millis;
    }
}

/**
 * @brief Communication from the AssistNow client
 * @param [in] sd_card_config2_data
 * @return error
 */
bool assist_now_client(struct sd_card_config2 *sd_card_config2_data)
{
    bool error = false;
    uint16_t counter = 0;
    unsigned long timeout = millis();
    char data[512];
    char temp[128];
    String payload;

    esp_task_wdt_reset();
    sprintf(data, "%s", sd_card_config2_data->assist_now_server);
    if (wifi_client.connect(data, 80) == true)
    {
        snprintf(data, sizeof(data), "GET http://%s/GetOnlineData.ashx?token=%s;gnss=gps,qzss,glo,bds,gal;datatype=eph,alm,aux,pos; HTTP/1.0\r\n", sd_card_config2_data->assist_now_server, sd_card_config2_data->assist_now_token);
        snprintf(temp, sizeof(temp), "Host: %s", sd_card_config2_data->assist_now_server);
        strncat(data, temp, sizeof(temp));
        snprintf(temp, sizeof(temp), "User-Agent: M5Stack Core2\r\n");
        strncat(data, temp, sizeof(temp));
        snprintf(temp, sizeof(temp), "Accept: */*\r\n");
        strncat(data, temp, sizeof(temp));
        snprintf(temp, sizeof(temp), "Connection: close\r\n");
        strncat(data, temp, sizeof(temp));
        wifi_client.write(data, strlen(data));
        wifi_client.print("\r\n");
        if (wifi_client.connected() == true)
        {
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
                    if (strstr(data, "200") != nullptr)
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
                if (error == false)
                {
                    counter = 0;
                    while (wifi_client.available() > 0)
                    {
                        data[counter] = (char)wifi_client.read();
                        counter = counter + 1;
                        if (strstr(data, "\r\n\r\n") != nullptr)
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
                    if (error == false)
                    {
                        payload = wifi_client.readString();
                        if (payload.length() > 0)
                        {
                            if ((unsigned int)gnss_serial.pushAssistNowData(payload, (size_t)payload.length()) == payload.length()) error = false;
                            else error = true;
                        }
                        else error = true;
                    }
                }
            }
        }
        else error = true;
    }
    else error = true;
    wifi_client.stop();
    
    return error;
}
