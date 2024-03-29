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

#include QMK_KEYBOARD_H

// hall sensor static
//#define HAL1 F4 // pro micro pins A3 and A2
//#define HAL2 F5
#define HAL1 F4 // beetle pins
#define HAL2 D4 // F5 A2 crosstalk?
#define HAL1_M 4 // pro micro pins A3 and A2
#define HAL2_M 32
#define RNG_MOV 3
#define MAG_HI 680 // super close to pcb ?
#define MAG_LO 400 //
//#define MAG_HI 690 // highest and lowest from lily
//#define MAG_LO 290
//#define MAG_RV 550 // odd mag bit // not used

#ifndef OPT_DEBOUNCE
#    define OPT_DEBOUNCE 5  // (ms) 			Time between scroll events
#endif
#ifndef SCROLL_BUTT_DEBOUNCE
#    define SCROLL_BUTT_DEBOUNCE 100  // (ms) 			Time between scroll events
#endif
#ifndef OPT_THRES
#    define OPT_THRES 150  // (0-1024) 	Threshold for actication
#endif
#ifndef OPT_SCALE
#    define OPT_SCALE 1  // Multiplier for wheel
#endif
#ifndef PLOOPY_DPI_OPTIONS
#    define PLOOPY_DPI_OPTIONS { 12000, 12000, 12000 }
#    ifndef PLOOPY_DPI_DEFAULT
#        define PLOOPY_DPI_DEFAULT 1
#    endif
#endif
#ifndef PLOOPY_DPI_DEFAULT
#    define PLOOPY_DPI_DEFAULT 0
#endif
//#ifndef PLOOPY_DRAGSCROLL_DPI
//#    define PLOOPY_DRAGSCROLL_DPI 100 // Fixed-DPI Drag Scroll
//#endif
//#ifndef PLOOPY_DRAGSCROLL_MULTIPLIER
//#    define PLOOPY_DRAGSCROLL_MULTIPLIER 0.75 // Variable-DPI Drag Scroll
//#endif

keyboard_config_t keyboard_config;
uint16_t          dpi_array[] = PLOOPY_DPI_OPTIONS;
#define DPI_OPTION_SIZE (sizeof(dpi_array) / sizeof(uint16_t))

// TODO: Implement libinput profiles
// https://wayland.freedesktop.org/libinput/doc/latest/pointer-acceleration.html
// Compile time accel selection
// Valid options are ACC_NONE, ACC_LINEAR, ACC_CUSTOM, ACC_QUADRATIC

// Trackball State
bool     is_scroll_clicked = false;
bool     BurstState        = false;  // init burst state for Trackball module
uint16_t MotionStart       = 0;      // Timer for accel, 0 is resting state
uint16_t lastScroll        = 0;      // Previous confirmed wheel event
//uint8_t  OptLowPin         = OPT_ENC1;
bool     debug_encoder     = false;
//bool     is_drag_scroll    = false;


// hall sensor tracking vars
uint16_t phys_dir_1 = 0, phys_dir_2 = 0, last_dir_1 = 0;
uint16_t pole_dir_1 = 0, pole_dir_2 = 0;
uint16_t pole_last_dir_1 = 0, pole_last_dir_2 = 0; // last dir
uint16_t pole_prev_dir_1 = 0, pole_prev_dir_2 = 0; // last 2 dir
uint16_t pole_last_1 = 0, pole_last_2 = 0; // last val
uint16_t pole_prev_1 = 0, pole_prev_2 = 0; // last 2 back val
uint16_t ticks = 0; // buffer tick for physical oops
uint16_t skip_8 = 0; // fake 8mhz
uint16_t pressed_state = 0; // get col size

// orig
//unsigned long cur_clk = 0, last_clk = 0, delta_time = 0, poll_time = 0;
uint16_t cur_clk = 0, last_clk = 0, delta_time = 0, poll_time = 0;
// ploopys
uint16_t lastMidClick      = 0;      // Stops scrollwheel from being read if it was pressed

void process_wheel(void) {
    // myne - hall init

    // fake 8mhz
    if (skip_8 == 0) {
      skip_8 += 1;
    }
    else {
      skip_8 = 0;
      return;
    }

    // proper qmk way to read is with mux
    uint16_t val1 = analogReadPin(HAL1);
    //_delay_ms(100);
    uint16_t val2 = analogReadPin(HAL2);
    //val2 = analogReadPin(HAL2);
    //uint16_t val1 = adc_read(HAL1_M);
    //uint16_t val2 = adc_read(HAL2_M);
    cur_clk = timer_read();

    dprintf("%u - %u\n",val1, val2);
    //dprintf("%d - %d\n",pinToMux(HAL1), pinToMux(HAL2) );

    if (pressed_state != 0)
      return;

    // TODO: Replace this with interrupt driven code,  polling is S L O W
    // Lovingly ripped from the Ploopy Source

    // ignore hall sensor stuff here, if any input, return
    // may want to do a "double" init, though it could slow down scrolling

    // If the mouse wheel was just released, do not scroll.
    if (timer_elapsed(lastMidClick) < SCROLL_BUTT_DEBOUNCE) {
        return;
    }

    // Limit the number of scrolls per unit time.
    if (timer_elapsed(lastScroll) < OPT_DEBOUNCE) {
        return;
    }

    // Don't scroll if the middle button is depressed.
    if (is_scroll_clicked) {
#ifndef IGNORE_SCROLL_CLICK
        return;
#endif
    }


    if (val1 > pole_last_1 + RNG_MOV) {
      pole_dir_1 = 1;
    } else if (val1 < pole_last_1 - RNG_MOV) {
      pole_dir_1 = 2;
    } else {
      pole_dir_1 = 0;
    }

    if (val2 > pole_last_2 + RNG_MOV) {
      pole_dir_2 = 1;
    } else if (val2 < pole_last_2 - RNG_MOV) {
      pole_dir_2 = 2;
    } else {
      pole_dir_2 = 0;
    }

    // dir tick and check
    if (phys_dir_1 == 0 && (val1 > MAG_HI || val1 < MAG_LO) ) {
      cur_clk = timer_read();
      if (pole_last_dir_1 != pole_dir_1) {
        if (pole_prev_dir_1 != pole_dir_1)  {
          if        ((val1 > MAG_HI && pole_dir_2 == 2) || \
                     (val1 < MAG_LO && pole_dir_2 == 1) ) {
            phys_dir_1 = 1;// up
          } else if ((val1 > MAG_HI && pole_dir_2 == 1) || \
                     (val1 < MAG_LO && pole_dir_2 == 2) ) {
            phys_dir_1 = 2; // down
          } // mag dir
        } // pole_prev_dir != pole_dir
      } // pole_last != pole_dir

      if (phys_dir_1 != 0) {
        ticks += 1;

        delta_time = cur_clk - last_clk;
        if (delta_time < 150 && last_dir_1 != 0) {
          phys_dir_1 = last_dir_1; // logic?
          //Serial.println("TIME FLIP");
        }

        if (ticks > 0) {
          if (phys_dir_1 == 1) {
            dprintf("up.\n");
            tap_code(KC_WH_U);
          } else if(phys_dir_1 == 2) {
            dprintf("down.\n");
            tap_code(KC_WH_D);
          }
        }

        last_clk = cur_clk;
        last_dir_1 = phys_dir_1;
      } // phys_dir != 0
    } //  MAG_HI < > MAG_LO
    else if ( (phys_dir_1 == 1 && val1 < MAG_HI) || \
              (phys_dir_1 == 2 && val1 > MAG_LO) ) {
      phys_dir_1 = 0;
      ticks = 0;
    }

    if (pole_dir_1 != 0 || pole_dir_2 != 0) {
      pole_prev_dir_1 = pole_last_dir_1;
      pole_last_dir_1 = pole_dir_1;
      pole_prev_1 = pole_last_1;
      pole_last_1 = val1;
      pole_prev_2 = pole_last_2;
      pole_last_2 = val2;
    }

    // myne is last_clk
    //lastScroll  = timer_read();

    //if (dir == 0) return;
    //process_wheel_user(mouse_report, mouse_report->h, (int)(mouse_report->v + (dir * OPT_SCALE)));
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    process_wheel();

//    if (is_drag_scroll) {
//        mouse_report.h = mouse_report.x;
//#ifdef PLOOPY_DRAGSCROLL_INVERT
//        // Invert vertical scroll direction
//        mouse_report.v = -mouse_report.y;
//#else
//        mouse_report.v = mouse_report.y;
//#endif
//        mouse_report.x = 0;
//        mouse_report.y = 0;
//    }

    // myne - oops invert / reverse / upside down
    mouse_report.x = mouse_report.x * -1;
    mouse_report.y = mouse_report.y * -1;

    return pointing_device_task_user(mouse_report);
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
//    if (debug_mouse) {
//        dprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);
//    }

    // Update Timer to prevent accidental scrolls
    if ((record->event.key.col == 1) && (record->event.key.row == 0)) {
        lastMidClick      = timer_read();
        is_scroll_clicked = record->event.pressed;
    }

    if (!process_record_user(keycode, record)) {
        return false;
    }

    if (keycode == DPI_CONFIG && record->event.pressed) {
        keyboard_config.dpi_config = (keyboard_config.dpi_config + 1) % DPI_OPTION_SIZE;
        eeconfig_update_kb(keyboard_config.raw);
        pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
    }

//    if (keycode == DRAG_SCROLL) {
//#ifndef PLOOPY_DRAGSCROLL_MOMENTARY
//        if (record->event.pressed)
//#endif
//        {
//            is_drag_scroll ^= 1;
//        }
//#ifdef PLOOPY_DRAGSCROLL_FIXED
//        pointing_device_set_cpi(is_drag_scroll ? PLOOPY_DRAGSCROLL_DPI : dpi_array[keyboard_config.dpi_config]);
//#else
//        pointing_device_set_cpi(is_drag_scroll ? (dpi_array[keyboard_config.dpi_config] * PLOOPY_DRAGSCROLL_MULTIPLIER) : dpi_array[keyboard_config.dpi_config]);
//#endif
//    }

    return true;
}

// Hardware Setup
void keyboard_pre_init_kb(void) {
    // debug_enable  = true;
    // debug_matrix  = true;
    // debug_mouse   = true;
    // debug_encoder = true;

    setPinInput(HAL1);
    setPinInput(HAL2);

    /* Ground all output pins connected to ground. This provides additional
     * pathways to ground. If you're messing with this, know this: driving ANY
     * of these pins high will cause a short. On the MCU. Ka-blooey.
     */
#ifdef UNUSABLE_PINS
    const pin_t unused_pins[] = UNUSABLE_PINS;

    for (uint8_t i = 0; i < ARRAY_SIZE(unused_pins); i++) {
        setPinOutput(unused_pins[i]);
        writePinLow(unused_pins[i]);
    }
#endif

    // This is the debug LED.
#if defined(DEBUG_LED_PIN)
    setPinOutput(DEBUG_LED_PIN);
    writePin(DEBUG_LED_PIN, debug_enable);
#endif

    keyboard_pre_init_user();
}

void pointing_device_init_kb(void) {
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
    // initialize the scroll wheel's optical encoder
//    opt_encoder_init(); // HAL sensor covered in init_kb ^
}

void eeconfig_init_kb(void) {
    keyboard_config.dpi_config = PLOOPY_DPI_DEFAULT;
    eeconfig_update_kb(keyboard_config.raw);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    // is safe to just read DPI setting since matrix init
    // comes before pointing device init.
    keyboard_config.raw = eeconfig_read_kb();
    if (keyboard_config.dpi_config > DPI_OPTION_SIZE) {
        eeconfig_init_kb();
    }
    matrix_init_user();
}
