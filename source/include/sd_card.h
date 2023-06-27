/**
 * @file sd_card.h
 *
 * @brief sd_card related functionality declaration.
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

#ifndef SDCARD_H_
#define SDCARD_H_

#include <Arduino.h>
#include <M5Core2.h>

struct sd_card_config1
{
    char timezone[30];
    uint8_t display_backlight;
    uint8_t display_rotation;
	uint8_t display_play;
    uint8_t display_timer;
    uint16_t display_time_on;
    uint16_t display_time_off;
    uint16_t display_timeout;
};

struct sd_card_config2
{
    char wlan_ssid[256];
    char wlan_password[256];
    char assist_now_server[256];
    char assist_now_token[30];
    char ntrip_server[256];
    uint16_t ntrip_port;
    char ntrip_mount_point[256];
    char ntrip_user[256];
    char ntrip_password[256];
};

bool sd_card_config_read(struct sd_card_config1 *sd_card_config1_data, struct sd_card_config2 *sd_card_config2_data);
bool sd_card_check_startup(void);
bool sd_card_check(void);

#endif
