/**
 * @file sd_card.cpp
 *
 * @brief sd_card related functionality implementation.
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
#include <float.h>
#include <time.h>
#include "sd_card.h"

/**
 * @brief Read the config file from the SD
 * @param [in] sd_card_config1_data, sd_card_config2_data
 * @return error
 */
bool sd_card_config_read(struct sd_card_config1 *sd_card_config1_data, struct sd_card_config2 *sd_card_config2_data)
{
    bool error = true;
    File datafile;
    char string[256];
    size_t length = 0;
    uint8_t counter = 0;

    esp_task_wdt_reset();
    datafile = SD.open("/config/config.ini", FILE_READ);
    if (datafile != 0)
    {
        sd_card_config1_data->timezone[0] = '\0'; 
        sd_card_config1_data->display_backlight = UINT8_MAX;
        sd_card_config1_data->display_rotation = UINT8_MAX;
        sd_card_config1_data->display_play = UINT8_MAX;
        sd_card_config1_data->display_timer = UINT8_MAX;
        sd_card_config1_data->display_time_on = UINT16_MAX;
        sd_card_config1_data->display_time_off = UINT16_MAX;
        sd_card_config1_data->display_timeout = UINT16_MAX;
        sd_card_config2_data->wlan_ssid[0] = '\0';
        sd_card_config2_data->wlan_password[0] = '\0';
        sd_card_config2_data->assist_now_server[0] = '\0';
        sd_card_config2_data->assist_now_token[0] = '\0';
        sd_card_config2_data->ntrip_server[0] = '\0';
        sd_card_config2_data->ntrip_port = UINT16_MAX;
        sd_card_config2_data->ntrip_mount_point[0] = '\0';
        sd_card_config2_data->ntrip_user[0] = '\0';
        sd_card_config2_data->ntrip_password[0] = '\0';
        do
        {
            length = datafile.readBytesUntil('\n', string, sizeof(string));
            string[length] = '\0';
            if (strncmp(string, "[system]", 8) == 0)
            {
                counter = 0;
                do
                {
                    length = datafile.readBytesUntil('=', string, sizeof(string));
                    string[length] = '\0';
                    if ((strncmp(string, "timezone", 8) == 0) && (sd_card_config1_data->timezone[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config1_data->timezone, string);
                        counter = counter + 1;
                    }
                }
                while ((datafile.available() > 0) && (counter < 1));
            }
            if (strncmp(string, "[display]", 9) == 0)
            {
                counter = 0;
                do
                {
                    length = datafile.readBytesUntil('=', string, sizeof(string));
                    string[length] = '\0';
                    if ((strncmp(string, "backlight", 9) == 0) && (sd_card_config1_data->display_backlight == UINT8_MAX))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        sd_card_config1_data->display_backlight = atoi(string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "rotation", 8) == 0) && (sd_card_config1_data->display_rotation == UINT8_MAX))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        if (strncmp(string, "on", 2) == 0) sd_card_config1_data->display_rotation = 1;
                        if (strncmp(string, "off", 3) == 0) sd_card_config1_data->display_rotation = 0;
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "play", 4) == 0) && (sd_card_config1_data->display_play == UINT8_MAX))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        if (strncmp(string, "on", 2) == 0) sd_card_config1_data->display_play = 1;
                        if (strncmp(string, "off", 3) == 0) sd_card_config1_data->display_play = 0;
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "timer", 5) == 0) && (sd_card_config1_data->display_timer == UINT8_MAX))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        if (strncmp(string, "on", 2) == 0) sd_card_config1_data->display_timer = 1;
                        if (strncmp(string, "off", 3) == 0) sd_card_config1_data->display_timer = 0;
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "timer_on", 8) == 0) && (sd_card_config1_data->display_time_on == UINT16_MAX))
                    {
                        length = datafile.readBytesUntil(':', string, sizeof(string));
                        string[length] = '\0';
                        sd_card_config1_data->display_time_on = atoi(string) * 60;
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        sd_card_config1_data->display_time_on = sd_card_config1_data->display_time_on + atoi(string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "timer_off", 9) == 0) && (sd_card_config1_data->display_time_off == UINT16_MAX))
                    {
                        length = datafile.readBytesUntil(':', string, sizeof(string));
                        string[length] = '\0';
                        sd_card_config1_data->display_time_off = atoi(string) * 60;
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        sd_card_config1_data->display_time_off = sd_card_config1_data->display_time_off + atoi(string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "timeout", 7) == 0) && (sd_card_config1_data->display_timeout == UINT16_MAX))
                    {
                        length = datafile.readBytesUntil(':', string, sizeof(string));
                        string[length] = '\0';
                        sd_card_config1_data->display_timeout = atoi(string) * 60;
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        sd_card_config1_data->display_timeout = sd_card_config1_data->display_timeout + atoi(string);
                        counter = counter + 1;
                    }
                }
                while ((datafile.available() > 0) && (counter < 7));
            }
            if (strncmp(string, "[wlan]", 6) == 0)
            {
                counter = 0;
                do
                {
                    length = datafile.readBytesUntil('=', string, sizeof(string));
                    string[length] = '\0';
                    if ((strncmp(string, "ssid", 4) == 0) && (sd_card_config2_data->wlan_ssid[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config2_data->wlan_ssid, string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "password", 8) == 0) && (sd_card_config2_data->wlan_password[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config2_data->wlan_password, string);
                        counter = counter + 1;
                    }
                }
                while ((datafile.available() > 0) && (counter < 2));
            }
            if (strncmp(string, "[assist_now]", 12) == 0)
            {
                counter = 0;
                do
                {
                    length = datafile.readBytesUntil('=', string, sizeof(string));
                    string[length] = '\0';
                    if ((strncmp(string, "server", 6) == 0) && (sd_card_config2_data->assist_now_server[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config2_data->assist_now_server, string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "token", 5) == 0) && (sd_card_config2_data->assist_now_token[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config2_data->assist_now_token, string);
                        counter = counter + 1;
                    }
                }
                while ((datafile.available() > 0) && (counter < 2));
            }
            if (strncmp(string, "[ntrip]", 7) == 0)
            {
                counter = 0;
                do
                {
                    length = datafile.readBytesUntil('=', string, sizeof(string));
                    string[length] = '\0';
                    if ((strncmp(string, "server", 6) == 0) && (sd_card_config2_data->ntrip_server[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config2_data->ntrip_server, string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "port", 4) == 0) && (sd_card_config2_data->ntrip_port == UINT16_MAX))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        sd_card_config2_data->ntrip_port = atol(string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "mount_point", 11) == 0) && (sd_card_config2_data->ntrip_mount_point[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config2_data->ntrip_mount_point, string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "user", 4) == 0) && (sd_card_config2_data->ntrip_user[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config2_data->ntrip_user, string);
                        counter = counter + 1;
                    }
                    if ((strncmp(string, "password", 8) == 0) && (sd_card_config2_data->ntrip_password[0] == '\0'))
                    {
                        length = datafile.readBytesUntil('\n', string, sizeof(string));
                        string[length - 1] = '\0';
                        strcpy(sd_card_config2_data->ntrip_password, string);
                        counter = counter + 1;
                    }
                }
                while ((datafile.available() > 0) && (counter < 5));
            }
        }
        while (datafile.available() > 0);
        datafile.close();
        if ((sd_card_config1_data->timezone[0] != '\0') &&
            (sd_card_config1_data->display_backlight != UINT8_MAX) &&
            (sd_card_config1_data->display_play != UINT8_MAX) &&
            (sd_card_config1_data->display_timer != UINT8_MAX) &&
            (sd_card_config1_data->display_time_on != UINT16_MAX) &&
            (sd_card_config1_data->display_time_off != UINT16_MAX) &&
            (sd_card_config1_data->display_timeout != UINT16_MAX) &&
            (sd_card_config2_data->wlan_ssid[0] != '\0') &&
            (sd_card_config2_data->wlan_password[0] != '\0') &&
            (sd_card_config2_data->assist_now_server[0] != '\0') &&
            (sd_card_config2_data->assist_now_token[0] != '\0') &&
            (sd_card_config2_data->ntrip_server[0] != '\0') &&
            (sd_card_config2_data->ntrip_port != UINT16_MAX) &&
            (sd_card_config2_data->ntrip_mount_point[0] != '\0') &&
            (sd_card_config2_data->ntrip_user[0] != '\0') &&
            (sd_card_config2_data->ntrip_password[0] != '\0')) error = false;
    }
    else error = true;

    return error;
}

/**
 * @brief Check the startup file on the SD
 * @return error
 */
bool sd_card_check_startup(void)
{
    esp_task_wdt_reset(); 
    if (SD.exists("/icons/startup.jpg") == false) return true;
 
    return false;
}

/**
 * @brief Check all necessary files on the SD
 * @return error
 */
bool sd_card_check(void)
{
    esp_task_wdt_reset(); 
    if (SD.exists("/config/config.ini") == false) return true;
    if (SD.exists("/data") == false) return true;

    return false;
}
