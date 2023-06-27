/**
 * @file real_time_clock.h
 *
 * @brief real time clock related functionality declaration.
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

#ifndef REALTIMECLOCK_H_
#define REALTIMECLOCK_H_

#include <Arduino.h>
#include <M5Core2.h>

#define REALTIMECLOCK_TIMEPULSE 35

struct real_time_clock
{
    time_t timestamp;
    bool gnss_fix_ok;
};

void real_time_clock_transfer(struct real_time_clock *real_time_clock_data);
bool real_time_clock_set(void);
void real_time_clock_init(struct real_time_clock *real_time_clock_data, struct sd_card_config1 *sd_card_config1_data);

#endif
