/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2020 Ploopy Corporation
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

//#include "config_common.h"

/* USB Device descriptor parameter */
// ffs - moved to info.json
//#define VENDOR_ID 0x5043
//#define PRODUCT_ID 0x4D6F
//#define DEVICE_VER 0x0001
//#define MANUFACTURER PloopyCo
//#define PRODUCT Mouse

/* key matrix size */
#define MATRIX_ROWS 1
#define MATRIX_COLS 6

/*
 * Keyboard Matrix Assignments
 *
 * Change this to how you wired your keyboard
 * COLS: AVR pins used for columns, left to right
 * ROWS: AVR pins used for rows, top to bottom
 * DIODE_DIRECTION: COL2ROW = COL = Anode (+), ROW = Cathode (-, marked on diode)
 *                  ROW2COL = ROW = Anode (+), COL = Cathode (-, marked on diode)
 *
 */
// LC, M, RC, BK, FW, ENT
// 1,  3,  2, 4,   5, ENT
// myne
//        { D8, D7, D9, D2, D1, D6 }  // ??
//        { B4, E6, B5, D1, D2, D7 }  // pro micro
//        { B7, B5, D1, D0, D3, D2 }  // beetle
// have to use 'avr' pins https://deskthority.net/wiki/Arduino_Pro_Micro#Pinout
// wipe the 'P'
#define DIRECT_PINS                        \
    {                                      \
        { B4, E6, B5, D3, D2, D7 } \
    }

// These pins are not broken out, and cannot be used normally.
// They are set as output and pulled high, by default
#define UNUSED_PINS \
    { B6, F7, D0, F6, F5 } // TODO: more

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* Much more so than a keyboard, speed matters for a mouse. So we'll go for as high
   a polling rate as possible. */
#define USB_POLLING_INTERVAL_MS 1
#define USB_MAX_POWER_CONSUMPTION 100

/* define if matrix has ghost (lacks anti-ghosting diodes) */
//#define MATRIX_HAS_GHOST

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

// DEPRECATED
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

/* Bootmagic Lite key configuration */
#define BOOTMAGIC_LITE_ROW 0
#define BOOTMAGIC_LITE_COLUMN 4

//#define RGB_DI_PIN B5
//#define RGBLED_NUM 4
//#define RGBLIGHT_LIMIT_VAL 40
//#define RGBLIGHT_EFFECT_BREATHING
//#define RGBLIGHT_EFFECT_RAINBOW_MOOD
//#define RGBLIGHT_EFFECT_RAINBOW_SWIRL

// #define DEBUG_LED_PIN F7

/* PMW3360 Settings */
// myne
// this is SS ?
// 2023 aug 10
// this isn't getting picked up now? tries to compile pmw3360 first...
// need 33XX (CAPITAL) now ?
//#define PMW3360_CS_PIN B6
#define PMW33XX_CS_PIN B6
