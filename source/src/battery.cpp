/**
 * @file battery.cpp
 *
 * @brief Battery related functionality implementation.
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
#include "battery.h"

/**
 * @brief Transfer data from the battery
 * @param [in] battery_data
 */
void battery_transfer(struct battery *battery_data)
{
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
 
    esp_task_wdt_reset();
    curr_millis = millis();
    if ((unsigned long)(curr_millis - last_millis) > 1000)
    {
        battery_data->level = M5.Axp.GetBatteryLevel();
        battery_data->charging = M5.Axp.isCharging();
        battery_data->update = true;
        last_millis = curr_millis;
    }
}

/**
 * @brief Initialize the battery
 * @param [in] battery_data
 */
void battery_init(struct battery *battery_data)
{
    esp_task_wdt_reset();
    M5.Axp.SetCHGCurrent(10);                           //960mA
    battery_data->level = M5.Axp.GetBatteryLevel();
    battery_data->charging = M5.Axp.isCharging();
    battery_data->update = true;
}
