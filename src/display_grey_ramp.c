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

#include <string.h>
#include <wonderful.h>
#include <ws.h>
#include <ws/display.h>
#include <ws/hardware.h>

#ifndef __WONDERFUL_WWITCH__

#include "iram.h"
#include "main.h"

__attribute__((assume_ss_data, interrupt))
extern void __far grey_ramp_line_int_handler(void);

__attribute__((assume_ss_data, interrupt))
static void __far grey_ramp_vblank_int_handler(void) {
    ws_hwint_ack(HWINT_VBLANK);
    outportb(IO_LCD_SHADE_45, 0x0F);
    outportb(IO_LCD_INTERRUPT, 15);
	vbl_ticks++;
}

void display_grey_ramp(void *userdata) {
	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));

	memset(MEM_TILE(0), 0, 16);
    outportw(IO_SCR_PAL_0, MONO_PAL_COLORS(7, 0, 0, 0));
    outportw(IO_SCR_PAL_2, MONO_PAL_COLORS(4, 4, 4, 4));
    outportw(IO_SCR_PAL_3, MONO_PAL_COLORS(5, 5, 5, 5));
	ws_screen_fill_tiles(screen_1, 0, 0, 0, 28, 18);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(2), 2, 1, 12, 16);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(3), 14, 1, 12, 16);

	cpu_irq_disable();
	ws_hwint_set_handler(HWINT_IDX_LINE, grey_ramp_line_int_handler);
	ws_hwint_set_handler(HWINT_IDX_VBLANK, grey_ramp_vblank_int_handler);
	ws_hwint_set(HWINT_LINE | HWINT_VBLANK);
	cpu_irq_enable();

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	wait_keypress();
}

#endif
