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

static const uint16_t __far tile_grid[8]  = { 0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81 };

void display_grid(void __far* userdata) {
	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));

	if (ws_system_is_color()) {
		ws_system_mode_set(WS_MODE_COLOR);
		MEM_COLOR_PALETTE(0)[0] = RGB(0, 0, 0);
		MEM_COLOR_PALETTE(0)[1] = RGB(15, 15, 15);
		MEM_COLOR_PALETTE(1)[0] = RGB(0, 0, 0);
		MEM_COLOR_PALETTE(1)[1] = RGB(15, 0, 0);
	} else {
		ws_display_set_shade_lut(SHADE_LUT_DEFAULT);
		outportw(IO_SCR_PAL_0, MONO_PAL_COLORS(0, 4, 0, 0));
		outportw(IO_SCR_PAL_1, MONO_PAL_COLORS(0, 7, 0, 0));
	}

	memcpy(MEM_TILE(0), tile_grid, sizeof(tile_grid));

	for (int iy = 0; iy < 18; iy++)
		for (int ix = 0; ix < 28; ix++) {
			ws_screen_put_tile(screen_1,
				((ix & 1) ? SCR_ATTR_FLIP_H : 0)
				| ((iy & 1) ? SCR_ATTR_FLIP_V : 0)
				| ((ix < 2 || ix >= 26 || iy < 2 || iy >= 16) ? SCR_ENTRY_PALETTE(1) : 0),
				ix, iy);
		}

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	wait_keypress();
}

