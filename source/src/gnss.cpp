/**
 * @file gnss.cpp
 *
 * @brief GNSS related functionality implementation.
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
#include "gnss.h"
#include "real_time_clock.h"
#include "bluetooth_serial.h"

portMUX_TYPE gnss_taskmux = portMUX_INITIALIZER_UNLOCKED;
SFE_UBLOX_GNSS gnss_i2c;
SFE_UBLOX_GNSS_SERIAL gnss_serial;
uint8_t fix_type = 0;
bool gnss_fix_ok = false;
bool diff_soln = false;
uint8_t carr_soln = 0;
uint8_t num_sv = 0;
double g_speed = 0.0;
double head_mot = 0.0;
uint8_t a_status = 1;
double lon = 0.0;
double lat = 0.0;
double height = 0.0;
double p_acc = 0.0;
double rel_pos_length = 0.0;
double acc_length = 0.0;
struct gnss_satellite_info gnss_satellite_info_gps_data[16];
struct gnss_satellite_info gnss_satellite_info_galileo_data[16];
struct gnss_satellite_info gnss_satellite_info_glonass_data[16];
struct gnss_satellite_info gnss_satellite_info_beidou_data[16];
struct gnss_satellite_info gnss_satellite_info_sbas_data[16];
extern BluetoothSerial bt_serial;

/**
 * @brief Transfer data from the GNSS
 * @param [in] wlan_client_data
 */
void gnss_transfer(struct gnss *gnss_data)
{
    static uint8_t counter1 = UINT8_MAX;
    uint8_t counter2 = 0;
    unsigned long curr_millis = 0;
    static unsigned long last_millis = millis();
 
    esp_task_wdt_reset();
    curr_millis = millis();
    if ((unsigned long)(curr_millis - last_millis) > 1000)
    {
        counter1 = 0;
        last_millis = curr_millis;
    }

    switch (counter1)
    {
        case 0:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->fix_type = fix_type;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 1:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->gnss_fix_ok = gnss_fix_ok;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 2:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->diff_soln = diff_soln;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 3:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->carr_soln = carr_soln;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 4:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->num_sv = num_sv;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 5:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->g_speed = g_speed;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 6:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->head_mot = head_mot;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 7:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->a_status = a_status;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 8:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->lon = lon;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 9:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->lat = lat;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 10:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->height = height;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 11:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->p_acc = p_acc;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 12:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->rel_pos_length = rel_pos_length;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 13:
        portENTER_CRITICAL(&gnss_taskmux);
        gnss_data->acc_length = acc_length;
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 14:
        portENTER_CRITICAL(&gnss_taskmux);
        for (counter2 = 0; counter2 < 16; counter2 = counter2 + 1)
        {
            gnss_data->gnss_satellite_info_gps_data[counter2].sv_id = gnss_satellite_info_gps_data[counter2].sv_id;
            gnss_data->gnss_satellite_info_gps_data[counter2].cno = gnss_satellite_info_gps_data[counter2].cno;
            gnss_data->gnss_satellite_info_gps_data[counter2].sv_used = gnss_satellite_info_gps_data[counter2].sv_used;
        }
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 15:
        portENTER_CRITICAL(&gnss_taskmux);
        for (counter2 = 0; counter2 < 16; counter2 = counter2 + 1)
        {
            gnss_data->gnss_satellite_info_galileo_data[counter2].sv_id = gnss_satellite_info_galileo_data[counter2].sv_id;
            gnss_data->gnss_satellite_info_galileo_data[counter2].cno = gnss_satellite_info_galileo_data[counter2].cno;
            gnss_data->gnss_satellite_info_galileo_data[counter2].sv_used = gnss_satellite_info_galileo_data[counter2].sv_used;
        }
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 16:
        portENTER_CRITICAL(&gnss_taskmux);
        for (counter2 = 0; counter2 < 16; counter2 = counter2 + 1)
        {
            gnss_data->gnss_satellite_info_glonass_data[counter2].sv_id = gnss_satellite_info_glonass_data[counter2].sv_id;
            gnss_data->gnss_satellite_info_glonass_data[counter2].cno = gnss_satellite_info_glonass_data[counter2].cno;
            gnss_data->gnss_satellite_info_glonass_data[counter2].sv_used = gnss_satellite_info_glonass_data[counter2].sv_used;
        }
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 17:
        portENTER_CRITICAL(&gnss_taskmux);
        for (counter2 = 0; counter2 < 16; counter2 = counter2 + 1)
        {
            gnss_data->gnss_satellite_info_beidou_data[counter2].sv_id = gnss_satellite_info_beidou_data[counter2].sv_id;
            gnss_data->gnss_satellite_info_beidou_data[counter2].cno = gnss_satellite_info_beidou_data[counter2].cno;
            gnss_data->gnss_satellite_info_beidou_data[counter2].sv_used = gnss_satellite_info_beidou_data[counter2].sv_used;
        }
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        break;

        case 18:
        portENTER_CRITICAL(&gnss_taskmux);
        for (counter2 = 0; counter2 < 16; counter2 = counter2 + 1)
        {
            gnss_data->gnss_satellite_info_sbas_data[counter2].sv_id = gnss_satellite_info_sbas_data[counter2].sv_id;
            gnss_data->gnss_satellite_info_sbas_data[counter2].cno = gnss_satellite_info_sbas_data[counter2].cno;
            gnss_data->gnss_satellite_info_sbas_data[counter2].sv_used = gnss_satellite_info_sbas_data[counter2].sv_used;
        }
        portEXIT_CRITICAL(&gnss_taskmux);
        counter1 = counter1 + 1;
        gnss_data->update = true;
        break;

        default:
        break;
    }
}

/**
 * @brief Read the data from the GNSS
 * @param [in] gnss_data
 */
void gnss(struct gnss *gnss_data)
{
    uint8_t counter1 = 0;
    uint8_t counter2 = 0;

    esp_task_wdt_reset();
    if (gnss_i2c.getPVT() == true)
    {
        portENTER_CRITICAL(&gnss_taskmux);
        fix_type = gnss_i2c.packetUBXNAVPVT->data.fixType;
        gnss_fix_ok = (bool)gnss_i2c.packetUBXNAVPVT->data.flags.bits.gnssFixOK;
        diff_soln = (bool)gnss_i2c.packetUBXNAVPVT->data.flags.bits.diffSoln;
        carr_soln = gnss_i2c.packetUBXNAVPVT->data.flags.bits.carrSoln;
        num_sv = gnss_i2c.packetUBXNAVPVT->data.numSV;
        g_speed = (double)gnss_i2c.packetUBXNAVPVT->data.gSpeed * 1E-3;
        head_mot = (double)gnss_i2c.packetUBXNAVPVT->data.headMot * 1E-5;
        portEXIT_CRITICAL(&gnss_taskmux);
        gnss_i2c.packetUBXNAVPVT->moduleQueried.moduleQueried1.bits.all = false;
    }
    if (gnss_i2c.getMONHW() == true)
    {
        portENTER_CRITICAL(&gnss_taskmux);
        a_status = gnss_i2c.packetUBXMONHW->data.aStatus;
        portEXIT_CRITICAL(&gnss_taskmux);
        gnss_i2c.flushMONHW();
    }
    if (gnss_i2c.getHPPOSLLH() == true)
    {
        portENTER_CRITICAL(&gnss_taskmux);
        lon = ((double)gnss_i2c.packetUBXNAVHPPOSLLH->data.lon + (double)gnss_i2c.packetUBXNAVHPPOSLLH->data.lonHp * 1E-2) * 1E-7;
        lat = ((double)gnss_i2c.packetUBXNAVHPPOSLLH->data.lat + (double)gnss_i2c.packetUBXNAVHPPOSLLH->data.latHp * 1E-2) * 1E-7;
        height = ((double)gnss_i2c.packetUBXNAVHPPOSLLH->data.height + (double)gnss_i2c.packetUBXNAVHPPOSLLH->data.heightHp * 1E-1) * 1E-3;
        portEXIT_CRITICAL(&gnss_taskmux);
        gnss_i2c.flushHPPOSLLH();
    }
    if (gnss_i2c.getNAVHPPOSECEF() == true)
    {
        portENTER_CRITICAL(&gnss_taskmux);
        p_acc = (double)gnss_i2c.packetUBXNAVHPPOSECEF->data.pAcc * 1E-4;
        portEXIT_CRITICAL(&gnss_taskmux);
        gnss_i2c.flushNAVHPPOSECEF(); 
    }
    if(gnss_i2c.getRELPOSNED() == true)
    {
        portENTER_CRITICAL(&gnss_taskmux);
        rel_pos_length = ((double)gnss_i2c.packetUBXNAVRELPOSNED->data.relPosLength + (double)gnss_i2c.packetUBXNAVRELPOSNED->data.relPosHPLength * 1E-2) * 1E-2;
        acc_length = (double)gnss_i2c.packetUBXNAVRELPOSNED->data.accLength * 1E-4;
        portEXIT_CRITICAL(&gnss_taskmux);
        gnss_i2c.flushNAVRELPOSNED();
    } 
    if (gnss_i2c.getNAVSAT() == true)
    {
        portENTER_CRITICAL(&gnss_taskmux);
        counter1 = 0;
        for (counter2 = 0; counter2 < gnss_i2c.packetUBXNAVSAT->data.header.numSvs; counter2 = counter2 + 1)
        {
            if (gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].gnssId == 0)
            {
                gnss_satellite_info_gps_data[counter1].sv_id = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].svId;
                gnss_satellite_info_gps_data[counter1].cno = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].cno;
                gnss_satellite_info_gps_data[counter1].sv_used = (bool)gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].flags.bits.svUsed;
                counter1 = counter1 + 1;
                if (counter1 >= 16) break;
            }
        }
        while (counter1 < 16)
        {
            gnss_satellite_info_gps_data[counter1].sv_id = 0;
            counter1 = counter1 + 1;
        } 
        counter1 = 0;
        for (counter2 = 0; counter2 < gnss_i2c.packetUBXNAVSAT->data.header.numSvs; counter2 = counter2 + 1)
        {
            if (gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].gnssId == 2)
            {
                gnss_satellite_info_galileo_data[counter1].sv_id = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].svId;
                gnss_satellite_info_galileo_data[counter1].cno = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].cno;
                gnss_satellite_info_galileo_data[counter1].sv_used = (bool)gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].flags.bits.svUsed;
                counter1 = counter1 + 1;
                if (counter1 >= 16) break;
            }
        }
        while (counter1 < 16)
        {
            gnss_satellite_info_galileo_data[counter1].sv_id = 0;
            counter1 = counter1 + 1;
        }
        counter1 = 0;
        for (counter2 = 0; counter2 < gnss_i2c.packetUBXNAVSAT->data.header.numSvs; counter2 = counter2 + 1)
        {
            if (gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].gnssId == 6)
            {
                gnss_satellite_info_glonass_data[counter1].sv_id = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].svId;
                gnss_satellite_info_glonass_data[counter1].cno = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].cno;
                gnss_satellite_info_glonass_data[counter1].sv_used = (bool)gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].flags.bits.svUsed;
                counter1 = counter1 + 1;
                if (counter1 >= 16) break;
            }
        }
        while (counter1 < 16)
        {
            gnss_satellite_info_glonass_data[counter1].sv_id = 0;
            counter1 = counter1 + 1;
        }
        counter1 = 0;
        for (counter2 = 0; counter2 < gnss_i2c.packetUBXNAVSAT->data.header.numSvs; counter2 = counter2 + 1)
        {
            if (gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].gnssId == 3)
            {
                gnss_satellite_info_beidou_data[counter1].sv_id = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].svId;
                gnss_satellite_info_beidou_data[counter1].cno = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].cno;
                gnss_satellite_info_beidou_data[counter1].sv_used = (bool)gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].flags.bits.svUsed;
                counter1 = counter1 + 1;
                if (counter1 >= 16) break;
            }
        }
        while (counter1 < 16)
        {
            gnss_satellite_info_beidou_data[counter1].sv_id = 0;
            counter1 = counter1 + 1;
        }
        counter1 = 0;
        for (counter2 = 0; counter2 < gnss_i2c.packetUBXNAVSAT->data.header.numSvs; counter2 = counter2 + 1)
        {
            if (gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].gnssId == 1)
            {
                gnss_satellite_info_sbas_data[counter1].sv_id = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].svId;
                gnss_satellite_info_sbas_data[counter1].cno = gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].cno;
                gnss_satellite_info_sbas_data[counter1].sv_used = (bool)gnss_i2c.packetUBXNAVSAT->data.blocks[counter2].flags.bits.svUsed;
                counter1 = counter1 + 1;
                if (counter1 >= 16) break;
            }
        }
        while (counter1 < 16)
        {
            gnss_satellite_info_sbas_data[counter1].sv_id = 0;
            counter1 = counter1 + 1;
        }
        portEXIT_CRITICAL(&gnss_taskmux);
        gnss_i2c.flushNAVSAT();
    }
}

/**
 * @brief Initialize the GNSS
 * @param [in] gnss_data
 * @return error
 */
bool gnss_init(struct gnss *gnss_data)
{
    bool error = false;
    uint8_t counter = 0;

    esp_task_wdt_reset();
    Serial2.begin(115200);
    Serial2.flush();
    pinMode(GNSS_EN, OUTPUT);
    digitalWrite(GNSS_EN, LOW);
    delay(500);
    digitalWrite(GNSS_EN, HIGH);
    delay(1000);
    if (Wire.setClock(400000) == true)
    {
        if (gnss_i2c.begin(Wire, kUBLOXGNSSDefaultAddress) == true)
        {
            error = !gnss_i2c.setI2COutput(COM_TYPE_UBX);
            if (error == false) error = !gnss_i2c.setI2COutput(COM_TYPE_UBX);
            if (error == false) error = !gnss_i2c.setI2CInput(COM_TYPE_UBX);
            if (error == false) error = !gnss_i2c.setSerialRate(115200, COM_PORT_UART1);
            if (error == false) error = !gnss_i2c.setUART1Output(COM_TYPE_UBX | COM_TYPE_NMEA);
            if (error == false) error = !gnss_i2c.setUART1Input(COM_TYPE_UBX | COM_TYPE_RTCM3);
            if (error == false) error = !gnss_i2c.setUART2Output(0);
            if (error == false) error = !gnss_i2c.setUART2Input(0);
            if (error == false) error = !gnss_i2c.setSPIOutput(0);
            if (error == false) error = !gnss_i2c.setSPIInput(0);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_TP_POL_TP1, 0);                     //Set time pulse polarity to falling edge
            if (error == false) error = !gnss_i2c.setVal32 (UBLOX_CFG_TP_PERIOD_LOCK_TP1, 2000000);     //Set time pulse periode to 2sek
            if (error == false) error = !gnss_i2c.setVal32(UBLOX_CFG_TP_LEN_LOCK_TP1, 1000000);         //Set time pulse length to 1sek
            if (error == false) error = !gnss_i2c.setVal32 (UBLOX_CFG_TP_TIMEGRID_TP1, 1);              //Set time grid to GPS
            if (error == false) error = !gnss_i2c.clearGeofences();
            if (error == false) error = !gnss_i2c.addGeofence(0, 0, 1, 5, true, 3);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_HW_ANT_CFG_SHORTDET, 1);            //Enable short antenna detection flag
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_HW_ANT_CFG_OPENDET, 1);             //Enable open antenna detection flag
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_HW_ANT_CFG_VOLTCTRL, 1);            //Enable active antenna voltage control flag
            if (error == false) error = !gnss_serial.begin(Serial2);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_MSGOUT_NMEA_ID_GGA_UART1, 1);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_MSGOUT_NMEA_ID_GLL_UART1, 0);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_MSGOUT_NMEA_ID_GSA_UART1, 1);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_MSGOUT_NMEA_ID_GST_UART1, 1);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_MSGOUT_NMEA_ID_GSV_UART1, 1);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_MSGOUT_NMEA_ID_RMC_UART1, 1);
            if (error == false) error = !gnss_i2c.setVal8(UBLOX_CFG_MSGOUT_NMEA_ID_VTG_UART1, 0);
            if (error == false) error = !gnss_i2c.setHighPrecisionMode(true); 
            if (error == false) error = !gnss_i2c.setPacketCfgPayloadSize(UBX_NAV_SAT_MAX_LEN); 
            if (error == false) error = !gnss_i2c.setNavigationFrequency(1);          
            if (error == false) error = !gnss_i2c.setAutoPVT(true);
            if (error == false) error = !gnss_i2c.setAutoMONHW(true);
            if (error == false) error = !gnss_i2c.setAutoHPPOSLLH(true);
            if (error == false) error = !gnss_i2c.setAutoNAVHPPOSECEF(true);
            if (error == false) error = !gnss_i2c.setAutoNAVSAT(true);
            if (error == false) error = !gnss_i2c.setAutoRELPOSNED(true);
            gnss_serial.setNMEAOutputPort(bt_serial);
        }
        else error = true;
    }
    else error = true;
    gnss_data->fix_type = 0;
    gnss_data->carr_soln = 0;
    gnss_data->num_sv = 0;
    gnss_data->a_status = SFE_UBLOX_ANTENNA_STATUS_DONTKNOW;
    gnss_data->gnss_fix_ok = false;
    gnss_data->lon = 0.0;
    gnss_data->lat = 0.0;
    gnss_data->height = 0.0;
    gnss_data->g_speed = 0.0;
    gnss_data->head_mot = 0.0;
    gnss_data->p_acc = 0.0;
    for (counter = 0; counter < 16; counter = counter + 1)
    {
        gnss_data->gnss_satellite_info_gps_data[counter].sv_id = 0;
        gnss_data->gnss_satellite_info_galileo_data[counter].sv_id = 0;
        gnss_data->gnss_satellite_info_glonass_data[counter].sv_id = 0;
        gnss_data->gnss_satellite_info_beidou_data[counter].sv_id = 0;
        gnss_data->gnss_satellite_info_sbas_data[counter].sv_id = 0;
    }

    return error;
}
