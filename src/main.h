/**
 * Copyright (c) 2024 Adrian Siekierka
 *
 * 240p-test-ws is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * 240p-test-ws is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with 240p-test-ws. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include <stddef.h>
#include <wonderful.h>
#include <ws.h>
#include "util.h"
#include "vwf8.h"

extern volatile uint16_t vbl_ticks;
extern uint16_t curr_keys;
extern uint16_t last_keys;
uint16_t scan_keys(void);

void vblank_wait(void);
void wait_keypress(void);

#endif
