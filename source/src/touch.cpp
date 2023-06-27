/**
 * @file touch.cpp
 *
 * @brief touch related functionality implementation.
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
#include "tough.h"

Button left(0, 0, 106, 280, "left");
Button middle(107, 0, 106, 280, "middle");
Button right(214, 0, 106, 280, "right");
int button = 0;

/**
 * @brief Left side of touch pressed
 * @param [in] e
 */
void touch_left(Event& e)
{
    button = -1;
}

/**
 * @brief Middle of touch pressed
 * @param [in] e
 */
void touch_middle(Event& e)
{
    button = 2;
}

/**
 * @brief Right side of touch pressed
 * @param [in] e
 */
void touch_right(Event& e)
{
    button = 1;
}

/**
 * @brief Initialize the touch
 */
void touch_init(void)
{
    esp_task_wdt_reset(); 
    left.addHandler(touch_left, E_TAP + E_PRESSED);
    middle.addHandler(touch_middle, E_TAP + E_PRESSED);
    right.addHandler(touch_right, E_TAP + E_PRESSED);
}
