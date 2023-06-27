/**
 * @file real_time_clock.cpp
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
#include <SparkFun_u-blox_GNSS_v3.h>
#include "real_time_clock.h"
#include "sd_card.h"

extern bool gnss_fix_ok;
extern portMUX_TYPE gnss_taskmux; 
extern SFE_UBLOX_GNSS gnss_i2c;

/**
 * @brief Transfer data from the real time clock
 * @param [in] real_time_clock_data
 */
void real_time_clock_transfer(struct real_time_clock *real_time_clock_data)
{
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
 
    esp_task_wdt_reset();
    curr_millis = millis();
    if ((unsigned long)(curr_millis - last_millis) > 1000)
    {
        portENTER_CRITICAL(&gnss_taskmux);
        real_time_clock_data->gnss_fix_ok = gnss_fix_ok;
        portEXIT_CRITICAL(&gnss_taskmux);
        
        last_millis = curr_millis;
    }
}

/**
 * @brief Set the real time clock
 * @return error
 */
bool real_time_clock_set(void)
{
    bool error = true;
	time_t timestamp = (time_t)0;
    struct tm timestamp_data;
    struct timeval timestamp_val;
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    int pin_timepulse;

    esp_task_wdt_reset();
    if (gnss_fix_ok == true)
    {
        Serial.print(F("Update real time clock... "));
        pin_timepulse = digitalRead(REALTIMECLOCK_TIMEPULSE);
        while(digitalRead(REALTIMECLOCK_TIMEPULSE) == pin_timepulse)
        {
        }
        delay(500);
        timestamp = (time_t)(gnss_i2c.getUnixEpoch() + 1);
        gmtime_r(&timestamp, &timestamp_data);
        date.Year = (uint16_t)(timestamp_data.tm_year + 1900);
        date.Month = (uint8_t)(timestamp_data.tm_mon + 1);
        date.Date = (uint8_t)timestamp_data.tm_mday;
        time.Hours = (uint8_t)timestamp_data.tm_hour;
        time.Minutes = (uint8_t)timestamp_data.tm_min;
        time.Seconds = (uint8_t)timestamp_data.tm_sec;
        while(digitalRead(REALTIMECLOCK_TIMEPULSE) != pin_timepulse)
        {
        }
        timestamp_val.tv_sec = timestamp;
        timestamp_val.tv_usec = 0;
        settimeofday(&timestamp_val, NULL);
        M5.Rtc.SetDate(&date);
        M5.Rtc.SetTime(&time);
        error = false;
        if (error == false) Serial.print(F("ok\n"));
        else Serial.print(F("failed\n"));
    }
    else error = true;

    return error;
}

/**
 * @brief Initialize the real time clock
 * @param [in] real_time_clock_data, sd_card_config1_data
 */
void real_time_clock_init(struct real_time_clock *real_time_clock_data, struct sd_card_config1 *sd_card_config1_data)
{
	time_t timestamp = (time_t)0;
	struct tm timestamp_data;
    struct timeval timestamp_val;
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    esp_task_wdt_reset();
    pinMode(REALTIMECLOCK_TIMEPULSE, INPUT_PULLUP);
    real_time_clock_data->gnss_fix_ok = false;
    real_time_clock_data->timestamp = (time_t)0;
    setenv("TZ", "GMT", 1);
    tzset();
    M5.Rtc.GetDate(&date);
    M5.Rtc.GetTime(&time);
    timestamp_data.tm_year = (int)date.Year - 1900;
    timestamp_data.tm_mon =  (int)date.Month - 1;
    timestamp_data.tm_mday = (int)date.Date;
    timestamp_data.tm_hour = (int)time.Hours;
    timestamp_data.tm_min =  (int)time.Minutes;
    timestamp_data.tm_sec =  (int)time.Seconds;
    timestamp = mktime(&timestamp_data);
    timestamp_val.tv_sec = timestamp;
    timestamp_val.tv_usec = 0;
    settimeofday(&timestamp_val, NULL);
    setenv("TZ", sd_card_config1_data->timezone, 1);
    tzset();
}
