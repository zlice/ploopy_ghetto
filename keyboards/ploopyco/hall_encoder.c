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
#include "opt_encoder.h"

enum State state;

/* Variables used for scroll wheel functionality. */
int phys_dir_1, phys_dir_2, last_dir_1;
int pole_dir_1, pole_dir_2;
int pole_last_dir_1, pole_last_dir_2; // last dir
int pole_prev_dir_1, pole_prev_dir_2; // last 2 dir
int pole_last_1, pole_last_2; // last val
int pole_prev_1, pole_prev_2; // last 2 back val
int ticks;
// 1 up, 2 down, 0 same


/* Setup function for the scroll wheel. Initializes
   the relevant variables. */
void hal_encoder_init(void) {
  phys_dir_1 = phys_dir_2 = last_dir_1 = 0 \
  pole_dir_1 = pole_dir_2 = 0 \
  pole_last_dir_1 = pole_last_dir_2 = 0 \
  pole_prev_dir_1 = pole_prev_dir_2 = 0 \
  pole_last_1 = pole_last_2 = 0 \
  pole_prev_1 = pole_prev_2 = 0 \
  ticks = 0;

  setPinInput(HAL1);
  setPinInput(HAL2);
}

int opt_encoder_handler(int curA, int curB) {
    if (lowOverflowA == false || highOverflowA == false) calculateThresholdA(curA);
    if (lowOverflowB == false || highOverflowB == false) calculateThresholdB(curB);

    bool LO = false;
    bool HI = true;
    bool sA, sB;
    int  ret = 0;

    if (curA < scrollThresholdA)
        sA = LO;
    else
        sA = HI;

    if (curB < scrollThresholdB)
        sB = LO;
    else
        sB = HI;

    if (state == HIHI) {
        if (sA == LO && sB == HI) {
            state = LOHI;
            if (hilof) {
                ret   = 1;
                hilof = false;
            }
        } else if (sA == HI && sB == LO) {
            state = HILO;
            if (lohif) {
                ret   = -1;
                lohif = false;
            }
        }
    }

    else if (state == HILO) {
        if (sA == HI && sB == HI) {
            state = HIHI;
            hilof = true;
            lohif = false;
        } else if (sA == LO && sB == LO) {
            state = LOLO;
            hilof = true;
            lohif = false;
        }
    }

    else if (state == LOLO) {
        if (sA == HI && sB == LO) {
            state = HILO;
            if (lohif) {
                ret   = 1;
                lohif = false;
            }
        } else if (sA == LO && sB == HI) {
            state = LOHI;
            if (hilof) {
                ret   = -1;
                hilof = false;
            }
        }
    }

    else {  // state must be LOHI
        if (sA == HI && sB == HI) {
            state = HIHI;
            lohif = true;
            hilof = false;
        } else if (sA == LO && sB == LO) {
            state = LOLO;
            lohif = true;
            hilof = false;
        }
    }

    return ret;
}

void calculateThresholdA(int curA) { scrollThresholdA = calculateThreshold(curA, &lowA, &highA, &cLowA, &cHighA, arLowA, arHighA, &lowIndexA, &highIndexA, &lowOverflowA, &highOverflowA); }

void calculateThresholdB(int curB) { scrollThresholdB = calculateThreshold(curB, &lowB, &highB, &cLowB, &cHighB, arLowB, arHighB, &lowIndexB, &highIndexB, &lowOverflowB, &highOverflowB); }

int calculateThreshold(int cur, int* low, int* high, bool* cLow, bool* cHigh, int arLow[], int arHigh[], int* lowIndex, int* highIndex, bool* lowOverflow, bool* highOverflow) {
    if (cur < *low) *low = cur;
    if (cur > *high) *high = cur;

    int curThresh = thresholdEquation(*low, *high);
    int range     = *high - *low;

    // The range is enforced to be over a certain limit because noise
    // can cause erroneous readings, making these calculations unstable.
    if (range >= SCROLL_THRESH_RANGE_LIM) {
        if (cur < curThresh) {
            if (*cHigh == true) {
                // We were just high, and now we crossed to low.
                // high reflects a sample of a high reading.
                arHigh[*highIndex] = *high;
                incrementIndex(highIndex, highOverflow);
                int midpoint = ((*high - *low) / 2) + *low;
                *low         = midpoint;
                *high        = midpoint;
                *cLow        = false;
                *cHigh       = false;
            } else {
                *cLow = true;
            }
        }
        if (cur > curThresh) {
            if (*cLow == true) {
                // We were just low, and now we crossed to high.
                // low reflects a sample of a low reading.
                arLow[*lowIndex] = *low;
                incrementIndex(lowIndex, lowOverflow);
                int midpoint = ((*high - *low) / 2) + *low;
                *low         = midpoint;
                *high        = midpoint;
                *cLow        = false;
                *cHigh       = false;
            } else {
                *cHigh = true;
            }
        }
    }

    int calcHigh = 0;
    if (*highOverflow == true) {
        for (int i = 0; i < SCROLLER_AR_SIZE; i++) {
            calcHigh += arHigh[i];
        }
        calcHigh = calcHigh / SCROLLER_AR_SIZE;
    } else if (*highIndex != 0) {
        for (int i = 0; i < *highIndex; i++) {
            calcHigh += arHigh[i];
        }
        calcHigh = calcHigh / *highIndex;
    } else {
        calcHigh = *high;
    }

    int calcLow = 0;
    if (*lowOverflow == true) {
        for (int i = 0; i < SCROLLER_AR_SIZE; i++) {
            calcLow += arLow[i];
        }
        calcLow = calcLow / SCROLLER_AR_SIZE;
    } else if (*lowIndex != 0) {
        for (int i = 0; i < *lowIndex; i++) {
            calcLow += arLow[i];
        }
        calcLow = calcLow / *lowIndex;
    } else {
        calcLow = *low;
    }

    return thresholdEquation(calcLow, calcHigh);
}

int thresholdEquation(int lo, int hi) { return ((hi - lo) / 3) + lo; }

void incrementIndex(int* index, bool* ovflw) {
    if (*index < SCROLLER_AR_SIZE - 1)
        (*index)++;
    else {
        *index = 0;
        *ovflw = true;
    }
}
