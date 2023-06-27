/**
 * @file page.h
 *
 * @brief page related functionality declaration.
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

#ifndef PAGE_H_
#define PAGE_H_

#include <Arduino.h>
#include <M5Core2.h>
#include "gnss.h"

#define LIGHTBLUE    0xB6DF
#define LIGHTTEAL    0xBF5F
#define LIGHTGREEN   0xBFF7
#define LIGHTCYAN    0xC7FF
#define LIGHTRED     0xFD34
#define LIGHTMAGENTA 0xFD5F
#define LIGHTYELLOW  0xFFF8
#define LIGHTORANGE  0xFE73
#define LIGHTPINK    0xFDDF
#define LIGHTPURPLE  0xCCFF
#define DARKBLUE     0x000D
#define DARKTEAL     0x020C
#define DARKRED      0x6000
#define DARKBROWN    0x2020
#define DARKMAGENTA  0x8008
#define DARKYELLOW   0x8400
#define DARKORANGE   0x8200
#define DARKPINK     0x9009
#define DARKPURPLE   0x4010

#define PAGE_TOTAL 11

#define PAGE_CLOCK_X 200
#define PAGE_CLOCK_Y 110
#define PAGE_CLOCK_SCALE 1

struct page_clock
{
    uint8_t actual_page;
    bool play;
    time_t timestamp;
    bool gnss_fix_ok;
};

struct page_status1
{
    uint8_t actual_page;
    bool play;
    bool wlan_client_active;
    bool assist_now_client_active;
    bool ntrip_client_active;
    uint8_t gnss_fix_type;
    bool diff_soln;
    uint8_t gnss_carr_soln;
    uint8_t gnss_num_sv;
};

struct page_status2
{
    uint8_t actual_page;
    bool play;
    uint8_t gnss_a_status;
    bool bluetooth_serial_active;
    float battery_level;
    bool battery_charging;
};

struct page_navigation1
{
    uint8_t actual_page;
    bool play;
    bool gnss_fix_ok;
    double gnss_lon;
    double gnss_lat;
    double gnss_height;
};

struct page_navigation2
{
    uint8_t actual_page;
    bool play;
    bool gnss_fix_ok;
    double gnss_g_speed;
    double gnss_head_mot;
    double gnss_p_acc;
};

struct page_relative_navigation
{
    uint8_t actual_page;
    bool play;
    uint8_t gnss_carr_soln;
    double gnss_rel_pos_length;
    double gnss_acc_length;
};

struct page_satellite_info
{
    uint8_t actual_page;
    bool play;
    char title[40];
    struct gnss_satellite_info gnss_satellite_info_data[16];
};

void page(struct sd_card_config1 *sd_card_config1_data, int *button, struct real_time_clock *real_time_clock_data, struct battery *battery_data, struct gnss *gnss_data, struct bluetooth_serial *bluetooth_serial_data, struct wlan_client *wlan_client_data, struct assist_now_client *assist_now_client_data, struct ntrip_client *ntrip_client_data);
void page_clock(struct page_clock *page_clock_data);
void page_status1(struct page_status1 *page_status1_data);
void page_status2(struct page_status2 *page_status2_data);
void page_navigation1(struct page_navigation1 *page_navigation1_data);
void page_navigation2(struct page_navigation2 *page_navigation2_data);
void page_relative_navigation(struct page_relative_navigation *page_relative_navigation_data);
void page_satellite_info(struct page_satellite_info *page_satellite_info_data);
void page_error(uint8_t error_code);

#endif
