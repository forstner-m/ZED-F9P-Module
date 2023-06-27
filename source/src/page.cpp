/**
 * @file page.cpp
 *
 * @brief page related functionality implementation.
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
#include "page.h"
#include "sd_card.h"
#include "real_time_clock.h"
#include "battery.h"
#include "gnss.h"
#include "bluetooth_serial.h"
#include "wlan_client.h"
#include "assist_now_client.h"
#include "ntrip_client.h"

const char static PROGMEM weekday_text_0[] =  "Sunday";
const char static PROGMEM weekday_text_1[] =  "Monday";
const char static PROGMEM weekday_text_2[] =  "Tuesday";
const char static PROGMEM weekday_text_3[] =  "Wednesday";
const char static PROGMEM weekday_text_4[] =  "Thursday";
const char static PROGMEM weekday_text_5[] =  "Friday";
const char static PROGMEM weekday_text_6[] =  "Saturday";
const char* const PROGMEM weekday_text[]  = {weekday_text_0, weekday_text_1, weekday_text_2, weekday_text_3, weekday_text_4, weekday_text_5, weekday_text_6};

const char static PROGMEM month_text_0[] =  "Jan.";
const char static PROGMEM month_text_1[] =  "Feb.";
const char static PROGMEM month_text_2[] =  "March";
const char static PROGMEM month_text_3[] =  "April";
const char static PROGMEM month_text_4[] =  "May";
const char static PROGMEM month_text_5[] =  "June";
const char static PROGMEM month_text_6[] =  "July";
const char static PROGMEM month_text_7[] =  "Aug.";
const char static PROGMEM month_text_8[] =  "Sep.";
const char static PROGMEM month_text_9[] =  "Oct.";
const char static PROGMEM month_text_10[] =  "Nov.";
const char static PROGMEM month_text_11[] =  "Dec.";
const char* const PROGMEM month_text[]  = {month_text_0, month_text_1, month_text_2, month_text_3, month_text_4, month_text_5, month_text_6, month_text_7, month_text_8, month_text_9, month_text_10, month_text_11};

/**
 * @brief Show the Meteotime pages
 * @param [in] sd_card_config1_data, button, real_time_clock_data, battery_data, gnss_data, bluetooth_serial_data, wlan_client_data, assist_now_client_data, ntrip_client_data
 */
void page(struct sd_card_config1 *sd_card_config1_data, int *button, struct real_time_clock *real_time_clock_data, struct battery *battery_data, struct gnss *gnss_data, struct bluetooth_serial *bluetooth_serial_data, struct wlan_client *wlan_client_data, struct assist_now_client *assist_now_client_data, struct ntrip_client *ntrip_client_data)
{
    uint8_t counter = 0;
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
    static bool play = (bool)sd_card_config1_data->display_play;
    static bool play_last = false;
    static uint8_t page_counter = 0;
    static uint8_t page_counter_last = PAGE_TOTAL - 1;
    struct page_clock page_clock_data;
    struct page_status1 page_status1_data;
    struct page_status2 page_status2_data;
    struct page_navigation1 page_navigation1_data;
    struct page_navigation2 page_navigation2_data;
    struct page_relative_navigation page_relative_navigation_data;
    struct page_satellite_info page_satellite_info_data;
    time_t timestamp = (time_t)0;
    struct tm timestamp_data;
    static double rel_pos_length_offset = 0.0;

    esp_task_wdt_reset(); 
    switch (*button)
    {
        case -1:
        if (page_counter > 0) page_counter = page_counter - 1;
        last_millis = millis();
        *button = 0;
        break;

        case 1:
        if (page_counter < (PAGE_TOTAL - 1)) page_counter = page_counter + 1;
        last_millis = millis();
        *button = 0;
        break;

        case 2:
        last_millis = millis();
        play = !play;
        *button = 0;
        break;

        default:
        *button = 0;
        break;
    }

    if (play == true)
    {
        curr_millis = millis();
        if ((unsigned long)(curr_millis - last_millis) > 10000)
        {
            if (page_counter < (PAGE_TOTAL - 1)) page_counter = page_counter + 1;
            else page_counter = 0;
            last_millis = curr_millis;
        }
    }

    switch (page_counter)
    {
        case 0:
        time(&timestamp);
        if ((page_counter != page_counter_last) || (play != play_last) || (timestamp != real_time_clock_data->timestamp))
        {
            page_clock_data.actual_page = page_counter;
            page_clock_data.play = play;
            page_clock_data.timestamp = timestamp;
            page_clock_data.gnss_fix_ok = real_time_clock_data->gnss_fix_ok;
            page_clock(&page_clock_data);
            page_counter_last = page_counter;
            play_last = play;
            real_time_clock_data->timestamp = timestamp;
        }
        break;

        case 1:
        if ((page_counter != page_counter_last) || (play != play_last) || (wlan_client_data->update == true) || (assist_now_client_data->update == true) || (ntrip_client_data->update == true) || (gnss_data->update == true))
        {
            page_status1_data.actual_page = page_counter;
            page_status1_data.play = play;
            page_status1_data.wlan_client_active = wlan_client_data->active;
            page_status1_data.assist_now_client_active = assist_now_client_data->active;
            page_status1_data.ntrip_client_active = ntrip_client_data->active; 
            page_status1_data.gnss_fix_type = gnss_data->fix_type;
            page_status1_data.diff_soln = gnss_data->diff_soln;
            page_status1_data.gnss_carr_soln = gnss_data->carr_soln;
            page_status1_data.gnss_num_sv = gnss_data->num_sv;
            page_status1(&page_status1_data);
            page_counter_last = page_counter;
            play_last = play;
            wlan_client_data->update = false;
            assist_now_client_data->update = false;
            ntrip_client_data->update  = false;
            gnss_data->update = false;
        }
        break;

        case 2:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true) || (bluetooth_serial_data->update == true) || (battery_data->update == true))
        {
            page_status2_data.actual_page = page_counter;
            page_status2_data.play = play;
            page_status2_data.gnss_a_status = gnss_data->a_status;
            page_status2_data.bluetooth_serial_active = bluetooth_serial_data->active;
            page_status2_data.battery_level = roundf(battery_data->level);
            page_status2_data.battery_charging = battery_data->charging;
            page_status2(&page_status2_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
            bluetooth_serial_data->update = false;
            battery_data->update = false;
        }
        break;

        case 3:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true))
        {
            page_navigation1_data.actual_page = page_counter;
            page_navigation1_data.play = play;
            page_navigation1_data.gnss_fix_ok = gnss_data->gnss_fix_ok;
            page_navigation1_data.gnss_lon = gnss_data->lon;
            page_navigation1_data.gnss_lat = gnss_data->lat;
            page_navigation1_data.gnss_height = gnss_data->height;
            page_navigation1(&page_navigation1_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
        }
        break;

        case 4:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true))
        {
            page_navigation2_data.actual_page = page_counter;
            page_navigation2_data.play = play;
            page_navigation2_data.gnss_fix_ok = gnss_data->gnss_fix_ok;
            page_navigation2_data.gnss_g_speed = gnss_data->g_speed;
            page_navigation2_data.gnss_head_mot = gnss_data->head_mot;
            page_navigation2_data.gnss_p_acc = gnss_data->p_acc;
            page_navigation2(&page_navigation2_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
        }
        break;

        case 5:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true))
        {
            page_relative_navigation_data.actual_page = page_counter;
            page_relative_navigation_data.play = play;
            page_relative_navigation_data.gnss_carr_soln = gnss_data->carr_soln;
            if ((page_counter != page_counter_last) && (gnss_data->carr_soln != 0)) rel_pos_length_offset = gnss_data->rel_pos_length;
            page_relative_navigation_data.gnss_rel_pos_length = gnss_data->rel_pos_length - rel_pos_length_offset;
            page_relative_navigation_data.gnss_acc_length = gnss_data->acc_length;
            page_relative_navigation(&page_relative_navigation_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
        }
        break;

        case 6:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true))
        {
            page_satellite_info_data.actual_page = page_counter;
            page_satellite_info_data.play = play;
            strcpy(page_satellite_info_data.title, "GPS Information");
            for (counter = 0; counter < 16; counter = counter + 1)
            {
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_id = gnss_data->gnss_satellite_info_gps_data[counter].sv_id;
                page_satellite_info_data.gnss_satellite_info_data[counter].cno = gnss_data->gnss_satellite_info_gps_data[counter].cno;
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_used = gnss_data->gnss_satellite_info_gps_data[counter].sv_used;
            }
            page_satellite_info(&page_satellite_info_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
        }
        break;

        case 7:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true))
        {
            page_satellite_info_data.actual_page = page_counter;
            page_satellite_info_data.play = play;
            strcpy(page_satellite_info_data.title, "Galileo Information");
            for (counter = 0; counter < 16; counter = counter + 1)
            {
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_id = gnss_data->gnss_satellite_info_galileo_data[counter].sv_id;
                page_satellite_info_data.gnss_satellite_info_data[counter].cno = gnss_data->gnss_satellite_info_galileo_data[counter].cno;
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_used = gnss_data->gnss_satellite_info_galileo_data[counter].sv_used;
            }
            page_satellite_info(&page_satellite_info_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
        }
        break;

        case 8:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true))
        {
            page_satellite_info_data.actual_page = page_counter;
            page_satellite_info_data.play = play;
            strcpy(page_satellite_info_data.title, "GLONASS Information");
            for (counter = 0; counter < 16; counter = counter + 1)
            {
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_id = gnss_data->gnss_satellite_info_glonass_data[counter].sv_id;
                page_satellite_info_data.gnss_satellite_info_data[counter].cno = gnss_data->gnss_satellite_info_glonass_data[counter].cno;
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_used = gnss_data->gnss_satellite_info_glonass_data[counter].sv_used;
            }
            page_satellite_info(&page_satellite_info_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
        }
        break;

        case 9:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true))
        {
            page_satellite_info_data.actual_page = page_counter;
            page_satellite_info_data.play = play;
            strcpy(page_satellite_info_data.title, "Beidou Information");
            for (counter = 0; counter < 16; counter = counter + 1)
            {
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_id = gnss_data->gnss_satellite_info_beidou_data[counter].sv_id;
                page_satellite_info_data.gnss_satellite_info_data[counter].cno = gnss_data->gnss_satellite_info_beidou_data[counter].cno;
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_used = gnss_data->gnss_satellite_info_beidou_data[counter].sv_used;
            }
            page_satellite_info(&page_satellite_info_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
        }
        break;

        case 10:
        if ((page_counter != page_counter_last) || (play != play_last) || (gnss_data->update == true))
        {
            page_satellite_info_data.actual_page = page_counter;
            page_satellite_info_data.play = play;
            strcpy(page_satellite_info_data.title, "SBAS Information");
            for (counter = 0; counter < 16; counter = counter + 1)
            {
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_id = gnss_data->gnss_satellite_info_sbas_data[counter].sv_id;
                page_satellite_info_data.gnss_satellite_info_data[counter].cno = gnss_data->gnss_satellite_info_sbas_data[counter].cno;
                page_satellite_info_data.gnss_satellite_info_data[counter].sv_used = gnss_data->gnss_satellite_info_sbas_data[counter].sv_used;
            }
            page_satellite_info(&page_satellite_info_data);
            page_counter_last = page_counter;
            play_last = play;
            gnss_data->update = false;
        }
        break;

        default:
        break;
    }
}

/**
 * @brief Show the clock
 * @param [in] page_clock_data
 */
void page_clock(struct page_clock *page_clock_data)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 
    char string[40];
    char string1[20];
    char string2[20];
	struct tm timestamp_data;
    uint16_t counter = 0;
    float sec_x = 0;
    float sec_y = 0;
    float hour_x = 0;
    float hour_y = 0;
    float sec_deg = 0;
    float min_deg = 0;
    float hour_deg = 0;
 
    esp_task_wdt_reset(); 
    localtime_r(&(page_clock_data->timestamp), &timestamp_data);
    tft.createSprite(320, 240);
    tft.fillSprite(NAVY);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    strcpy_P(string, (char *)pgm_read_dword(&(weekday_text[timestamp_data.tm_wday])));
    tft.drawString(string, 5, 5, 4);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TR_DATUM);
    sprintf(string, "%u/%u", page_clock_data->actual_page + 1, PAGE_TOTAL);
    tft.drawString(string, 315, 5, 4);

    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    sprintf(string1, "%u.", timestamp_data.tm_mday);
    strcpy_P(string2, (char *)pgm_read_dword(&(month_text[timestamp_data.tm_mon])));
    sprintf(string, "%s %s", string1, string2); 
    tft.drawString(string, 5, 35, 4);

    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    sprintf(string, "%u", timestamp_data.tm_year + 1900);
    tft.drawString(string, 5, 65, 4);

    tft.fillCircle(PAGE_CLOCK_X, PAGE_CLOCK_Y, (int32_t)(PAGE_CLOCK_SCALE * 98.0f), DARKGREEN);
    tft.fillCircle(PAGE_CLOCK_X, PAGE_CLOCK_Y, (int32_t)(PAGE_CLOCK_SCALE * 92.0f), DARKTEAL);
    tft.drawCircle(PAGE_CLOCK_X, PAGE_CLOCK_Y, (int32_t)(PAGE_CLOCK_SCALE * 98.0f), DARKGREY);

    for (counter = 0; counter < 360; counter = counter + 30)
    {
        sec_x = cosf(((float)counter - 90.0f) * 0.0174532925f);
        sec_y = sinf(((float)counter - 90.0f) * 0.0174532925f);
        tft.drawLine((int32_t)(sec_x * PAGE_CLOCK_SCALE * 94.0f + PAGE_CLOCK_X), (int32_t)(sec_y * PAGE_CLOCK_SCALE * 94.0f + PAGE_CLOCK_Y), 
                        (int32_t)(sec_x * PAGE_CLOCK_SCALE * 80.0f + PAGE_CLOCK_X), (int32_t)(sec_y * PAGE_CLOCK_SCALE * 80.0f + PAGE_CLOCK_Y), DARKGREEN);
    }

    for (counter = 0; counter < 360; counter = counter + 6)
    {
        sec_x = cosf((counter - 90.0f) * 0.0174532925f);
        sec_y = sinf((counter - 90.0f) * 0.0174532925f);
        tft.drawPixel((int32_t)(sec_x * PAGE_CLOCK_SCALE * 82.0f + PAGE_CLOCK_X), (int32_t)(sec_y * PAGE_CLOCK_SCALE * 82.0f + PAGE_CLOCK_Y), WHITE);
        if(counter == 0  || counter == 180) tft.fillCircle((int32_t)(sec_x * PAGE_CLOCK_SCALE * 82.0f + PAGE_CLOCK_X), (int32_t)(sec_y * PAGE_CLOCK_SCALE * 82.0f + PAGE_CLOCK_Y), 2, WHITE);
        if(counter == 90 || counter == 270) tft.fillCircle((int32_t)(sec_x * PAGE_CLOCK_SCALE * 82.0f + PAGE_CLOCK_X), (int32_t)(sec_y * PAGE_CLOCK_SCALE * 82.0f + PAGE_CLOCK_Y), 2, WHITE);
    }

    tft.fillCircle(PAGE_CLOCK_X, PAGE_CLOCK_Y, (int32_t)(PAGE_CLOCK_SCALE * 5.0f), WHITE); 

    sec_deg = (float)timestamp_data.tm_sec * 6.0f;
    min_deg = (float)timestamp_data.tm_min * 6.0f + sec_deg * 0.01666667f;
    hour_deg = (float)timestamp_data.tm_hour * 30.0f + min_deg * 0.0833333f;
    hour_x = cosf((hour_deg - 90.0f) * 0.0174532925f) * PAGE_CLOCK_SCALE * 52.0f + PAGE_CLOCK_X;    
    hour_y = sinf((hour_deg - 90.0f) * 0.0174532925f) * PAGE_CLOCK_SCALE * 52.0f + PAGE_CLOCK_Y;
    tft.drawLine((int32_t)hour_x, (int32_t)hour_y, PAGE_CLOCK_X, PAGE_CLOCK_Y, WHITE);
    tft.drawLine((int32_t)hour_x + 1, (int32_t)hour_y, PAGE_CLOCK_X + 1, PAGE_CLOCK_Y, WHITE);
    tft.drawLine((int32_t)hour_x - 1, (int32_t)hour_y, PAGE_CLOCK_X - 1, PAGE_CLOCK_Y, WHITE);
    tft.drawLine((int32_t)hour_x, (int32_t)hour_y - 1, PAGE_CLOCK_X, PAGE_CLOCK_Y - 1, WHITE);
    tft.drawLine((int32_t)hour_x, (int32_t)hour_y + 1, PAGE_CLOCK_X, PAGE_CLOCK_Y + 1, WHITE);
    tft.drawLine((int32_t)(cosf((min_deg - 90.0f) * 0.0174532925f) * PAGE_CLOCK_SCALE * 74.0f + PAGE_CLOCK_X), (int32_t)(sinf((min_deg - 90.0f) * 0.0174532925f) * PAGE_CLOCK_SCALE * 74.0f + PAGE_CLOCK_Y), PAGE_CLOCK_X, PAGE_CLOCK_Y, WHITE);
    tft.drawLine((int32_t)(cosf((sec_deg - 90.0f) * 0.0174532925f) * PAGE_CLOCK_SCALE * 78.0f + PAGE_CLOCK_X), (int32_t)(sinf((sec_deg - 90.0f) * 0.0174532925f) * PAGE_CLOCK_SCALE * 78.0f + PAGE_CLOCK_Y), PAGE_CLOCK_X, PAGE_CLOCK_Y, RED);

    tft.fillCircle(PAGE_CLOCK_X, PAGE_CLOCK_Y, (int32_t)(PAGE_CLOCK_SCALE * 3.0f), RED);       

    if (page_clock_data->gnss_fix_ok == true)
    {
        tft.fillRoundRect(5, 150, 80, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 150, 80, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 150, 80, 50, 10, DARKRED);
        tft.drawRoundRect(5, 150, 80, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(F("GNSS"), 45, 177, 4);

    if (page_clock_data->actual_page > 0)
    {
        tft.drawLine(52, 213, 42, 223, WHITE);
        tft.drawLine(42, 223, 52, 233, WHITE);
        tft.drawLine(62, 213, 52, 223, WHITE);
        tft.drawLine(52, 223, 62, 233, WHITE);
    }
    if (page_clock_data->play == true) tft.drawRect(150, 213, 20, 20, WHITE);
    else
    {
        tft.drawLine(150, 213, 150, 233, WHITE);
        tft.drawLine(150, 213, 170, 223, WHITE);
        tft.drawLine(170, 223, 150, 233, WHITE);
    }
    if (page_clock_data->actual_page < PAGE_TOTAL - 1)
    {
        tft.drawLine(262, 213, 272, 223, WHITE);
        tft.drawLine(272, 223, 262, 233, WHITE);
        tft.drawLine(252, 213, 262, 223, WHITE);
        tft.drawLine(262, 223, 252, 233, WHITE);
    }

    tft.pushSprite(0, 0);
    tft.deleteSprite();
}

/**
 * @brief Show the status 1 page
 * @param [in] page_status1_data
 */
void page_status1(struct page_status1 *page_status1_data)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 
    char string[40];

    esp_task_wdt_reset();
    tft.createSprite(320, 240);
    tft.fillSprite(NAVY);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(F("Status 1"), 5, 5, 4);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TR_DATUM);
    sprintf(string, "%u/%u", page_status1_data->actual_page + 1, PAGE_TOTAL);
    tft.drawString(string, 315, 5, 4);
 
    if (page_status1_data->wlan_client_active == true)
	{
        tft.fillRoundRect(5, 40, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 40, 152, 50, 10, DARKGREY);
    }
    else
 	{
        tft.fillRoundRect(5, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 152, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(F("WLAN"), 81, 65, 4);

    if (page_status1_data->assist_now_client_active == true)
	{
        tft.fillRoundRect(5, 95, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 95, 152, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 95, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 95, 152, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(F("AssistNow"), 81, 120, 4);

    if (page_status1_data->ntrip_client_active == true)
	{
        tft.fillRoundRect(5, 150, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 150, 152, 50, 10, DARKGREY);
    }
    else
 	{
        tft.fillRoundRect(5, 150, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 150, 152, 50, 10, DARKGREY);
    }   
    tft.setTextColor(WHITE);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(F("NTRIP"), 81, 175, 4);

    switch (page_status1_data->gnss_fix_type)
    {
        case 0:
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("No Fix"), 238, 65, 4);
        break;

        case 1:
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("Dead R."), 238, 65, 4);
        break;

        case 2:
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("2D Fix"), 238, 65, 4);
        break;

        case 3:
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("3D Fix"), 238, 65, 4);
        break;

        case 4:
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("GNSS D. R."), 238, 65, 4);
        break;

        case 5:
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("Time Only"), 238, 65, 4);
        break;

        default:
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("Unknown"), 238, 65, 4);
        break;
    }

    switch (page_status1_data->gnss_carr_soln)
    {
        case 0:
        if (page_status1_data->diff_soln == 0)
        {
            tft.fillRoundRect(162, 95, 152, 50, 10, DARKRED);
            tft.drawRoundRect(162, 95, 152, 50, 10, DARKGREY);
            tft.setTextColor(WHITE);
            tft.setTextDatum(CC_DATUM);
            tft.drawString(F("DGNSS Off"), 238, 120, 4);
        }
        else
        {
            tft.fillRoundRect(162, 95, 152, 50, 10, DARKGREEN);
            tft.drawRoundRect(162, 95, 152, 50, 10, DARKGREY);
            tft.setTextColor(WHITE);
            tft.setTextDatum(CC_DATUM);
            tft.drawString(F("DGNSS On"), 238, 120, 4);
        }
        break;

        case 1:
        tft.fillRoundRect(162, 95, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(162, 95, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("RTK Float"), 238, 120, 4);
        break;

        case 2:
        tft.fillRoundRect(162, 95, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(162, 95, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("RTK Fix"), 238, 120, 4);
        break;

        default:
        tft.fillRoundRect(162, 95, 152, 50, 10, DARKRED);
        tft.drawRoundRect(162, 95, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("Unknown"), 238, 120, 4);
        break;
    }
    
    if (page_status1_data->gnss_num_sv != 0)
	{
        tft.fillRoundRect(162, 150, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(162, 150, 152, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(162, 150, 152, 50, 10, DARKRED);
        tft.drawRoundRect(162, 150, 152, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CC_DATUM);
    sprintf(string, "Satellites %d", page_status1_data->gnss_num_sv);
    tft.drawString(string, 238, 175, 4);

    if (page_status1_data->actual_page > 0)
    {
        tft.drawLine(52, 213, 42, 223, WHITE);
        tft.drawLine(42, 223, 52, 233, WHITE);
        tft.drawLine(62, 213, 52, 223, WHITE);
        tft.drawLine(52, 223, 62, 233, WHITE);
    }
    if (page_status1_data->play == true) tft.drawRect(150, 213, 20, 20, WHITE);
    else
    {
        tft.drawLine(150, 213, 150, 233, WHITE);
        tft.drawLine(150, 213, 170, 223, WHITE);
        tft.drawLine(170, 223, 150, 233, WHITE);
    }
    if (page_status1_data->actual_page < PAGE_TOTAL - 1)
    {
        tft.drawLine(262, 213, 272, 223, WHITE);
        tft.drawLine(272, 223, 262, 233, WHITE);
        tft.drawLine(252, 213, 262, 223, WHITE);
        tft.drawLine(262, 223, 252, 233, WHITE);
    }

    tft.pushSprite(0, 0);
    tft.deleteSprite();
}

/**
 * @brief Show the status 2 page
 * @param [in] page_status2_data
 */
void page_status2(struct page_status2 *page_status2_data)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 
    char string[40];

    esp_task_wdt_reset();
    tft.createSprite(320, 240);
    tft.fillSprite(NAVY);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(F("Status 2"), 5, 5, 4);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TR_DATUM);
    sprintf(string, "%u/%u", page_status2_data->actual_page + 1, PAGE_TOTAL);
    tft.drawString(string, 315, 5, 4);
 
    switch (page_status2_data->gnss_a_status)
    {
        case 0:
        tft.fillRoundRect(5, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("ANT Init"), 81, 65, 4);
        break;
    
        case 1:
        tft.fillRoundRect(5, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("ANT Unknown"), 81, 65, 4);
        break;

        case 2:
        tft.fillRoundRect(5, 40, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("ANT OK"), 81, 65, 4);
        break;

        case 3:
        tft.fillRoundRect(5, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("ANT Short"), 81, 65, 4);
        break;
        
        case 4:
        tft.fillRoundRect(5, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("ANT Open"), 81, 65, 4);
        break;

        default:
        tft.fillRoundRect(5, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("Unknown"), 81, 65, 4);
        break;
    }

    if (page_status2_data->bluetooth_serial_active == true)
	{
        tft.fillRoundRect(5, 95, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 95, 152, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 95, 152, 50, 10, DARKRED);
        tft.drawRoundRect(5, 95, 152, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(F("Bluetooth"), 81, 120, 4);

    if (page_status2_data->battery_level > 20.0)
	{
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(162, 40, 152, 50, 10, DARKRED);
        tft.drawRoundRect(162, 40, 152, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CC_DATUM);
    sprintf(string, "Bat.: %.0f %%", page_status2_data->battery_level);
    tft.drawString(string, 238, 65, 4);

    if (page_status2_data->battery_level > 99)
	{
        tft.fillRoundRect(162, 95, 152, 50, 10, DARKGREEN);
        tft.drawRoundRect(162, 95, 152, 50, 10, DARKGREY);
        tft.setTextColor(WHITE);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(F("Bat. Full"), 238, 120, 4);
    }
    else
    {
        if (page_status2_data->battery_charging == true)
        {
            tft.fillRoundRect(162, 95, 152, 50, 10, DARKGREEN);
            tft.drawRoundRect(162, 95, 152, 50, 10, DARKGREY);
            tft.setTextColor(WHITE);
            tft.setTextDatum(CC_DATUM);
            tft.drawString(F("Bat. Char."), 238, 120, 4);
        }
        else
        {
            tft.fillRoundRect(162, 95, 152, 50, 10, ORANGE);
            tft.drawRoundRect(162, 95, 152, 50, 10, DARKGREY);
            tft.setTextColor(WHITE);
            tft.setTextDatum(CC_DATUM);
            tft.drawString(F("Bat. Disch."), 238, 120, 4);
        }
    }

    if (page_status2_data->actual_page > 0)
    {
        tft.drawLine(52, 213, 42, 223, WHITE);
        tft.drawLine(42, 223, 52, 233, WHITE);
        tft.drawLine(62, 213, 52, 223, WHITE);
        tft.drawLine(52, 223, 62, 233, WHITE);
    }
    if (page_status2_data->play == true) tft.drawRect(150, 213, 20, 20, WHITE);
    else
    {
        tft.drawLine(150, 213, 150, 233, WHITE);
        tft.drawLine(150, 213, 170, 223, WHITE);
        tft.drawLine(170, 223, 150, 233, WHITE);
    }
    if (page_status2_data->actual_page < PAGE_TOTAL - 1)
    {
        tft.drawLine(262, 213, 272, 223, WHITE);
        tft.drawLine(272, 223, 262, 233, WHITE);
        tft.drawLine(252, 213, 262, 223, WHITE);
        tft.drawLine(262, 223, 252, 233, WHITE);
    }

    tft.pushSprite(0, 0);
    tft.deleteSprite();
}

/**
 * @brief Show the navigation 1 page
 * @param [in] page_navigation1_data
 */
void page_navigation1(struct page_navigation1 *page_navigation1_data)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 
    char string[40];

    esp_task_wdt_reset();
    tft.createSprite(320, 240);
    tft.fillSprite(NAVY);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(F("Navigation 1"), 5, 5, 4);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TR_DATUM);
    sprintf(string, "%u/%u", page_navigation1_data->actual_page + 1, PAGE_TOTAL);
    tft.drawString(string, 315, 5, 4);
 
    if (page_navigation1_data->gnss_fix_ok == true)
    {
        tft.fillRoundRect(5, 40, 310, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 40, 310, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 40, 310, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 310, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(F("LAT:"), 10, 65, 4);
    sprintf(string, "%+014.9f", page_navigation1_data->gnss_lat);
    tft.drawString(string, 100, 65, 4);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(F("o"), 290, 49, 2);
 
    if (page_navigation1_data->gnss_fix_ok == true)
    {
        tft.fillRoundRect(5, 95, 310, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 95, 310, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 95, 310, 50, 10, DARKRED);
        tft.drawRoundRect(5, 95, 310, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(F("LON:"), 10, 120, 4);
    sprintf(string, "%+014.9f", page_navigation1_data->gnss_lon);
    tft.drawString(string, 100, 120, 4);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(F("o"), 290, 104, 2);

    if (page_navigation1_data->gnss_fix_ok == true)
    {
        tft.fillRoundRect(5, 150, 310, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 150, 310, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 150, 310, 50, 10, DARKRED);
        tft.drawRoundRect(5, 150, 310, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(F("Height:"), 10, 175, 4);
    sprintf(string, "%01.4f m", page_navigation1_data->gnss_height);
    tft.drawString(string, 100, 175, 4);

    if (page_navigation1_data->actual_page > 0)
    {
        tft.drawLine(52, 213, 42, 223, WHITE);
        tft.drawLine(42, 223, 52, 233, WHITE);
        tft.drawLine(62, 213, 52, 223, WHITE);
        tft.drawLine(52, 223, 62, 233, WHITE);
    }
    if (page_navigation1_data->play == true) tft.drawRect(150, 213, 20, 20, WHITE);
    else
    {
        tft.drawLine(150, 213, 150, 233, WHITE);
        tft.drawLine(150, 213, 170, 223, WHITE);
        tft.drawLine(170, 223, 150, 233, WHITE);
    }
    if (page_navigation1_data->actual_page < PAGE_TOTAL - 1)
    {
        tft.drawLine(262, 213, 272, 223, WHITE);
        tft.drawLine(272, 223, 262, 233, WHITE);
        tft.drawLine(252, 213, 262, 223, WHITE);
        tft.drawLine(262, 223, 252, 233, WHITE);
    }

    tft.pushSprite(0, 0);
    tft.deleteSprite();
}

/**
 * @brief Show the navigation 2 page
 * @param [in] page_navigation2_data
 */
void page_navigation2(struct page_navigation2 *page_navigation2_data)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 
    char string[40];

    esp_task_wdt_reset();
    tft.createSprite(320, 240);
    tft.fillSprite(NAVY);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(F("Navigation 2"), 5, 5, 4);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TR_DATUM);
    sprintf(string, "%u/%u", page_navigation2_data->actual_page + 1, PAGE_TOTAL);
    tft.drawString(string, 315, 5, 4);
 
    if (page_navigation2_data->gnss_fix_ok == true)
    {
        tft.fillRoundRect(5, 40, 310, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 40, 310, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 40, 310, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 310, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(F("Speed:"), 10, 65, 4);
    sprintf(string, "%.3f m/s", page_navigation2_data->gnss_g_speed);
    tft.drawString(string, 125, 65, 4);
    tft.setTextDatum(CC_DATUM);
 
    if (page_navigation2_data->gnss_fix_ok == true)
    {
        tft.fillRoundRect(5, 95, 310, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 95, 310, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 95, 310, 50, 10, DARKRED);
        tft.drawRoundRect(5, 95, 310, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(F("Heading:"), 10, 120, 4);
    sprintf(string, "%09.5f", page_navigation2_data->gnss_head_mot);
    tft.drawString(string, 125, 120, 4);
    tft.setTextDatum(CC_DATUM);
    tft.drawString(F("o"), 250, 104, 2);

    if (page_navigation2_data->gnss_fix_ok == true)
    {
        tft.fillRoundRect(5, 150, 310, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 150, 310, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 150, 310, 50, 10, DARKRED);
        tft.drawRoundRect(5, 150, 310, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(F("Accuracy:"), 10, 175, 4);
    sprintf(string, "%.4f m", page_navigation2_data->gnss_p_acc);
    tft.drawString(string, 125, 175, 4);

    if (page_navigation2_data->actual_page > 0)
    {
        tft.drawLine(52, 213, 42, 223, WHITE);
        tft.drawLine(42, 223, 52, 233, WHITE);
        tft.drawLine(62, 213, 52, 223, WHITE);
        tft.drawLine(52, 223, 62, 233, WHITE);
    }
    if (page_navigation2_data->play == true) tft.drawRect(150, 213, 20, 20, WHITE);
    else
    {
        tft.drawLine(150, 213, 150, 233, WHITE);
        tft.drawLine(150, 213, 170, 223, WHITE);
        tft.drawLine(170, 223, 150, 233, WHITE);
    }
    if (page_navigation2_data->actual_page < PAGE_TOTAL - 1)
    {
        tft.drawLine(262, 213, 272, 223, WHITE);
        tft.drawLine(272, 223, 262, 233, WHITE);
        tft.drawLine(252, 213, 262, 223, WHITE);
        tft.drawLine(262, 223, 252, 233, WHITE);
    }

    tft.pushSprite(0, 0);
    tft.deleteSprite();
}


/**
 * @brief Show the relative navigation page
 * @param [in] page_relative_navigation_data
 */
void page_relative_navigation(struct page_relative_navigation *page_relative_navigation_data)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 
    char string[40];

    esp_task_wdt_reset();
    tft.createSprite(320, 240);
    tft.fillSprite(NAVY);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(F("Relative Navigation"), 5, 5, 4);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TR_DATUM);
    sprintf(string, "%u/%u", page_relative_navigation_data->actual_page + 1, PAGE_TOTAL);
    tft.drawString(string, 315, 5, 4);
 
    if (page_relative_navigation_data->gnss_carr_soln != 0)
    {
        tft.fillRoundRect(5, 40, 310, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 40, 310, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 40, 310, 50, 10, DARKRED);
        tft.drawRoundRect(5, 40, 310, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(F("Length:"), 10, 65, 4);
    sprintf(string, "%.4f m", page_relative_navigation_data->gnss_rel_pos_length);
    tft.drawString(string, 125, 65, 4);
    tft.setTextDatum(CC_DATUM);
 
    if (page_relative_navigation_data->gnss_carr_soln != 0)
    {
        tft.fillRoundRect(5, 95, 310, 50, 10, DARKGREEN);
        tft.drawRoundRect(5, 95, 310, 50, 10, DARKGREY);
    }
    else
    {
        tft.fillRoundRect(5, 95, 310, 50, 10, DARKRED);
        tft.drawRoundRect(5, 95, 310, 50, 10, DARKGREY);
    }
    tft.setTextColor(WHITE);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(F("Accuracy:"), 10, 120, 4);
    sprintf(string, "%.4f m", page_relative_navigation_data->gnss_acc_length);
    tft.drawString(string, 125, 120, 4);
    tft.setTextDatum(CC_DATUM);

    if (page_relative_navigation_data->actual_page > 0)
    {
        tft.drawLine(52, 213, 42, 223, WHITE);
        tft.drawLine(42, 223, 52, 233, WHITE);
        tft.drawLine(62, 213, 52, 223, WHITE);
        tft.drawLine(52, 223, 62, 233, WHITE);
    }
    if (page_relative_navigation_data->play == true) tft.drawRect(150, 213, 20, 20, WHITE);
    else
    {
        tft.drawLine(150, 213, 150, 233, WHITE);
        tft.drawLine(150, 213, 170, 223, WHITE);
        tft.drawLine(170, 223, 150, 233, WHITE);
    }
    if (page_relative_navigation_data->actual_page < PAGE_TOTAL - 1)
    {
        tft.drawLine(262, 213, 272, 223, WHITE);
        tft.drawLine(272, 223, 262, 233, WHITE);
        tft.drawLine(252, 213, 262, 223, WHITE);
        tft.drawLine(262, 223, 252, 233, WHITE);
    }

    tft.pushSprite(0, 0);
    tft.deleteSprite();
}

/**
 * @brief Show the satellite info page
 * @param [in] page_satellite_info_data
 */
void page_satellite_info(struct page_satellite_info *page_satellite_info_data)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 
    uint8_t counter = 0;
    char string[40];
    int32_t offset_x = 0;
    int32_t offset_y = 0;

    esp_task_wdt_reset();
    tft.createSprite(320, 240);
    tft.fillSprite(NAVY);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(page_satellite_info_data->title, 5, 5, 4);
    tft.setTextColor(WHITE);
    tft.setTextDatum(TR_DATUM);
    sprintf(string, "%u/%u", page_satellite_info_data->actual_page + 1, PAGE_TOTAL);
    tft.drawString(string, 315, 5, 4);

    tft.setTextColor(WHITE);
    tft.setTextDatum(TC_DATUM);
    for (counter = 0; counter < 16; counter = counter + 1)
    {
        if (page_satellite_info_data->gnss_satellite_info_data[counter].sv_id > 0)
        {
            if (counter < 8)
            {
                offset_x = 0;
                offset_y = 0;
            }
            else
            {
                offset_x = 288;
                offset_y = 86;
            }
            tft.drawRect(25 + counter * 36 - offset_x, 40 + offset_y, 18, 62, DARKGREY);
            if (page_satellite_info_data->gnss_satellite_info_data[counter].sv_used == true) tft.fillRect(26 + counter * 36 - offset_x, 101 - (int32_t)((60.0f / 63.0f) * (float)page_satellite_info_data->gnss_satellite_info_data[counter].cno) + offset_y,
                16, (int32_t)((60.0f / 63.0f) * (float)page_satellite_info_data->gnss_satellite_info_data[counter].cno), DARKGREEN);
            else tft.fillRect(26 + counter * 36 - offset_x, 101 - (int32_t)((60.0f / 63.0f) * (float)page_satellite_info_data->gnss_satellite_info_data[counter].cno) + offset_y,
                16, (int32_t)((60.0f / 63.0f) * (float)page_satellite_info_data->gnss_satellite_info_data[counter].cno), DARKRED);
            sprintf(string, "%u", page_satellite_info_data->gnss_satellite_info_data[counter].sv_id);
            tft.drawString(string, 34 + counter * 36 - offset_x, 103 + offset_y, 2);
        }
    }

    if (page_satellite_info_data->actual_page > 0)
    {
        tft.drawLine(52, 213, 42, 223, WHITE);
        tft.drawLine(42, 223, 52, 233, WHITE);
        tft.drawLine(62, 213, 52, 223, WHITE);
        tft.drawLine(52, 223, 62, 233, WHITE);
    }
    if (page_satellite_info_data->play == true) tft.drawRect(150, 213, 20, 20, WHITE);
    else
    {
        tft.drawLine(150, 213, 150, 233, WHITE);
        tft.drawLine(150, 213, 170, 223, WHITE);
        tft.drawLine(170, 223, 150, 233, WHITE);
    }
    if (page_satellite_info_data->actual_page < PAGE_TOTAL - 1)
    {
        tft.drawLine(262, 213, 272, 223, WHITE);
        tft.drawLine(272, 223, 262, 233, WHITE);
        tft.drawLine(252, 213, 262, 223, WHITE);
        tft.drawLine(262, 223, 252, 233, WHITE);
    }

    tft.pushSprite(0, 0);
    tft.deleteSprite();
}

/**
 * @brief Show the error code on the page
 * @param [in] error_code
 */
void page_error(uint8_t error_code)
{
    TFT_eSprite tft = TFT_eSprite(&M5.Lcd); 

    tft.createSprite(320, 240);
    tft.fillSprite(NAVY); 
    tft.setTextColor(RED);
    tft.setTextDatum(MC_DATUM);
    switch (error_code)
    {
        case 0:
        tft.drawString(F("Missing data on"), 160, 100, 4);
        tft.drawString(F("the SD card!"), 160, 140, 4); 
        tft.pushSprite(0, 0);
        tft.deleteSprite();
        while(1)
        {
            esp_task_wdt_reset(); 
        }
        break;

        case 1:
        tft.drawString(F("Wrong config file on the"), 160, 100, 4);
        tft.drawString(F("SD card!"), 160, 140, 4); 
        tft.pushSprite(0, 0);
        tft.deleteSprite();
        while(1)
        {
            esp_task_wdt_reset(); 
        }
        break;

        case 2:
        tft.drawString(F("GNSS have"), 160, 100, 4);
        tft.drawString(F("a problem!"), 160, 140, 4); 
        tft.pushSprite(0, 0);
        tft.deleteSprite();
        while(1)
        {
            esp_task_wdt_reset(); 
        }
        break;

        case 3:
        tft.drawString(F("Bluetooth serial have"), 160, 100, 4);
        tft.drawString(F("a problem!"), 160, 140, 4); 
        tft.pushSprite(0, 0);
        tft.deleteSprite();
        while(1)
        {
            esp_task_wdt_reset(); 
        }
        break;

        default:
        tft.drawString(F("Unknown"), 160, 100, 4);
        tft.drawString(F("error!"), 160, 140, 4); 
        tft.pushSprite(0, 0);
        tft.deleteSprite();
        while(1)
        {
            esp_task_wdt_reset(); 
        }
        break;
    }
}
