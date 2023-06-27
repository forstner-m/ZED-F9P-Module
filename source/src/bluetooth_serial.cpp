/**
 * @file bluetooth_serial.cpp
 *
 * @brief Bluetooth serial related functionality implementation.
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
#include <BluetoothSerial.h>
#include "bluetooth_serial.h"

portMUX_TYPE bluetooth_serial_taskmux = portMUX_INITIALIZER_UNLOCKED; 
BluetoothSerial bt_serial;
bool bluetooth_serial_active = false;
extern SFE_UBLOX_GNSS_SERIAL gnss_serial;

/**
 * @brief Transfer data from the Bluetooth serial
 * @param [in] bluetooth_serial_data
 */
void bluetooth_serial_transfer(struct bluetooth_serial *bluetooth_serial_data)
{
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
 
    esp_task_wdt_reset();
    curr_millis = millis();
    if ((unsigned long)(curr_millis - last_millis) > 1000)
    {
        portENTER_CRITICAL(&bluetooth_serial_taskmux);
        if (bluetooth_serial_data->active != bluetooth_serial_active)
        {
            bluetooth_serial_data->active = bluetooth_serial_active;
            bluetooth_serial_data->update = true;
        }
        portEXIT_CRITICAL(&bluetooth_serial_taskmux);
        last_millis = curr_millis;
    }
}

/**
 * @brief Transfer data from the bluetooth serial
 */
void bluetooth_serial(void)
{
    esp_task_wdt_reset();
    if (bt_serial.hasClient() == true)
    {
        gnss_serial.checkUblox();

        portENTER_CRITICAL(&bluetooth_serial_taskmux);
        bluetooth_serial_active = true;
        portEXIT_CRITICAL(&bluetooth_serial_taskmux);
    }
    else
    {
        portENTER_CRITICAL(&bluetooth_serial_taskmux);
        bluetooth_serial_active = false;
        portEXIT_CRITICAL(&bluetooth_serial_taskmux);
    }
}

/**
 * @brief Initialize the Bluetooth serial
 * @param [in] gnss_data
 * @return error
 */
bool bluetooth_serial_init(struct bluetooth_serial *bluetooth_serial_data)
{
    bool error = false;

    esp_task_wdt_reset();
    error = !bt_serial.begin("ZED-F9P");
    bluetooth_serial_data->active = false;

    return error;
}
