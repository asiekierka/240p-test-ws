/**
 * Copyright (c) 2024, 2025 Adrian Siekierka
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
#include <wsx/zx0.h>

#include "assets/mono/controller.h"

#include "iram.h"
#include "main.h"
#include "submenu.h"

static inline void highlight(bool cond, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
	ws_screen_modify_tiles(screen_1, ~SCR_ATTR_PALETTE_MASK, SCR_ATTR_PALETTE(cond ? 1 : 0), x, y, width, height);
}

void input_test(void *userdata) {
	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1));

	// Configure background, tile data.
	ws_display_set_shade_lut(SHADE_LUT_DEFAULT);
	outportw(IO_SCR_PAL_0, MONO_PAL_COLORS(0, 7, 4, 5));
	outportw(IO_SCR_PAL_1, MONO_PAL_COLORS(0, 7, 4, 1));
	wsx_zx0_decompress(MEM_TILE(0), gfx_mono_controller_tiles);
	wsx_zx0_decompress(screen_1, gfx_mono_controller_map);

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	int i = 0;

	while (true) {
		vblank_wait();
		scan_keys();

		if (curr_keys && curr_keys == last_keys) {
			i++;
			if (i > 75*6) break;
		} else {
			i = 0;
		}

		highlight(curr_keys & KEY_Y1, 4, 3, 1, 1);
		highlight(curr_keys & KEY_Y2, 5, 4, 1, 1);
		highlight(curr_keys & KEY_Y3, 4, 5, 1, 1);
		highlight(curr_keys & KEY_Y4, 3, 4, 1, 1);
		highlight(curr_keys & KEY_X1, 4, 10, 1, 1);
		highlight(curr_keys & KEY_X2, 5, 11, 1, 1);
		highlight(curr_keys & KEY_X3, 4, 12, 1, 1);
		highlight(curr_keys & KEY_X4, 3, 11, 1, 1);
		highlight(curr_keys & KEY_START, 11, 14, 3, 1);
		highlight(curr_keys & KEY_A, 25, 11, 1, 1);
		highlight(curr_keys & KEY_B, 24, 12, 1, 1);
	}
}

