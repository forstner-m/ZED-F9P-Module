/**
 * @file gnss.h
 *
 * @brief GNSS related functionality declaration.
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

#ifndef GNSS_H_
#define GNSS_H_

#include <Arduino.h>
#include <M5Core2.h>

#define GNSS_EN 19

struct gnss_satellite_info
{
    uint8_t sv_id;
    uint8_t cno;
    bool sv_used;
};

struct gnss
{
    bool update;
    uint8_t fix_type;
    bool gnss_fix_ok;
    bool diff_soln;
    uint8_t carr_soln;
    uint8_t num_sv;
    double g_speed;
    double head_mot;
    uint8_t a_status;
    double lon;
    double lat;
    double height;
    double p_acc;
    double rel_pos_length;
    double acc_length;
    struct gnss_satellite_info gnss_satellite_info_gps_data[16];
    struct gnss_satellite_info gnss_satellite_info_galileo_data[16];
    struct gnss_satellite_info gnss_satellite_info_glonass_data[16];
    struct gnss_satellite_info gnss_satellite_info_beidou_data[16];
    struct gnss_satellite_info gnss_satellite_info_sbas_data[16];
};

void gnss_transfer(struct gnss *gnss_data);
void gnss(struct gnss *gnss_data);
bool gnss_init(struct gnss *gnss_data);

#endif
