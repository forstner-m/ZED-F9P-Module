/**
 * @file display.cpp
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
#include "display.h"
#include "sd_card.h"

/**
 * @brief Timer for the display
 * @param [in,out] sd_card_config1_data, button
 * @return display_on
 */
bool display_timer(struct sd_card_config1 *sd_card_config1_data, int *button)
{
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
	time_t timestamp = (time_t)0;
	struct tm timestamp_data;
    static bool display_on = true;
    static uint16_t timeout = sd_card_config1_data->display_timeout;   

    esp_task_wdt_reset();
    if (sd_card_config1_data->display_timer == 1)
    {
        curr_millis = millis();
        if ((unsigned long)(curr_millis - last_millis) > 1000)
        {
            if (timeout == 0)
            {
                time(&timestamp);
                localtime_r(&timestamp, &timestamp_data);
                if (sd_card_config1_data->display_time_on < sd_card_config1_data->display_time_off)
                {
                    if (((sd_card_config1_data->display_time_on < (timestamp_data.tm_hour * 60 + timestamp_data.tm_min)) && ((timestamp_data.tm_hour * 60 + timestamp_data.tm_min) < sd_card_config1_data->display_time_off)))
                    {
                        if (display_on == false)
                        {
                            M5.lcd.wakeup();
                            M5.Axp.SetLcdVoltage((3300 - 2500) / 100 * sd_card_config1_data->display_backlight + 2500);
                            M5.Axp.SetLed(1);
                            display_on = true;
                        }
                    }
                    else
                    {
                        if (display_on == true)
                        {
                            M5.lcd.sleep();
                            M5.Axp.SetLcdVoltage(2500);
                            M5.Axp.SetLed(0);
                            display_on = false;
                        }
                    }
                }
                else
                {
                    if (((sd_card_config1_data->display_time_off < (timestamp_data.tm_hour * 60 + timestamp_data.tm_min)) && ((timestamp_data.tm_hour * 60 + timestamp_data.tm_min) < sd_card_config1_data->display_time_on)))
                    {
                        if (display_on == true)
                        {
                            M5.lcd.sleep();
                            M5.Axp.SetLcdVoltage(2500);
                            M5.Axp.SetLed(0);
                            display_on = false;
                        }
                    }
                    else
                    {
                        if (display_on == false)
                        {
                            M5.lcd.wakeup();
                            M5.Axp.SetLcdVoltage((3300 - 2500) / 100 * sd_card_config1_data->display_backlight + 2500);
                            M5.Axp.SetLed(1);
                            display_on = true;
                        }
                    }
                }
            }
            else timeout = timeout - 1;
            last_millis = curr_millis;
        }
        if(*button != 0)
        {
            if (display_on == false)
            {
                M5.lcd.wakeup();
                M5.Axp.SetLcdVoltage((3300 - 2500) / 100 * sd_card_config1_data->display_backlight + 2500);
                M5.Axp.SetLed(1);
                *button = 0;
                display_on = true;
            }
            timeout = sd_card_config1_data->display_timeout;
        }
    }
    else display_on = true;

    return display_on;
}

/**
 * @brief Initialize the display
 * @param [in] sd_card_config1_data
 */
void display_init(struct sd_card_config1 *sd_card_config1_data)
{
    esp_task_wdt_reset();
    if (sd_card_config1_data->display_rotation == true) M5.Lcd.setRotation(3);
    else M5.Lcd.setRotation(1);
    M5.Axp.SetLcdVoltage((3300 - 2500) / 100 * sd_card_config1_data->display_backlight + 2500);
    M5.Axp.SetLed(1);
}
