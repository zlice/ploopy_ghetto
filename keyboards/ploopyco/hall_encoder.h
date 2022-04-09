/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
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

#include <stdbool.h>

#ifndef SCROLLER_AR_SIZE
#    define SCROLLER_AR_SIZE 31
#endif

#ifndef SCROLL_THRESH_RANGE_LIM
#    define SCROLL_THRESH_RANGE_LIM 10
#endif

#define HAL1 A0
#define PWR1 5
#define HAL2 A1
#define PWR2 9
#define RNG_MOV 5
#define MAG_HI 690
#define MAG_LO 290
#define MAG_RV 550 // odd mag bit

enum State { HIHI, HILO, LOLO, LOHI };

extern enum State state;

extern int phys_dir_1 = 0, phys_dir_2 = 0, last_dir_1 = 0;
extern int pole_dir_1 = 0, pole_dir_2 = 0;
extern int pole_last_dir_1 = 0, pole_last_dir_2 = 0; // last dir
extern int pole_prev_dir_1 = 0, pole_prev_dir_2 = 0; // last 2 dir
extern int pole_last_1 = 0, pole_last_2 = 0; // last val
extern int pole_prev_1 = 0, pole_prev_2 = 0; // last 2 back val
extern int ticks = 0;
// 1 up, 2 down, 0 same

void hall_encoder_init(void);
int  hall_encoder_handler(void);
