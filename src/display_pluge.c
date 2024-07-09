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

#include "iram.h"
#include "main.h"

void display_pluge(void __far* userdata) {
	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));

	ws_system_mode_set(WS_MODE_MONO);
        ws_display_set_shade_lut(~SHADE_LUT(0, 1, 2, 4, 7, 11, 15, 0));
	outportw(IO_SCR_PAL_0, 0x0000);
	outportw(IO_SCR_PAL_1, 0x1111);
	outportw(IO_SCR_PAL_2, 0x2222);
	outportw(IO_SCR_PAL_3, 0x3333);
	outportw(IO_SCR_PAL_4, 0x4444);
	outportw(IO_SCR_PAL_5, 0x5555);
	outportw(IO_SCR_PAL_6, 0x6666);
	outportw(IO_SCR_PAL_7, 0x7777);

	memset(MEM_TILE(0), 0xFF, 16);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(0), 0, 0, 28, 18);

	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(6), 10, 3, 8, 3);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(5), 10, 6, 8, 3);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(4), 10, 9, 8, 3);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(3), 10, 12, 8, 3);

	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(2), 2, 3, 2, 12);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(1), 6, 3, 2, 12);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(1), 20, 3, 2, 12);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(2), 24, 3, 2, 12);

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	wait_keypress();
}

