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
#include <ws/system.h>

#include "iram.h"
#include "main.h"

static const uint16_t __wf_rom tile_stripe_patterns[] = {
	// Horizontal stripes
	0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000,
	// Vertical stripes
	0x5555, 0x5555, 0x5555, 0x5555, 0x5555, 0x5555, 0x5555, 0x5555,
	// Checkerboard
	0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA
};

static void display_stripe_start(void) {
	outportw(IO_DISPLAY_CTRL, 0);
}

#define PATTERN_KEY_MASK (KEY_A | KEY_X1 | KEY_X2 | KEY_X3 | KEY_X4)

static void display_stripe_end(void) {
	uint16_t pattern_id = 0;
	uint16_t flip_mask = 0;

	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));
	memset(MEM_TILE(0), 0, 16);
	memset(MEM_TILE(1), 0xFF, 16);
	memcpy(MEM_TILE(2), tile_stripe_patterns, 16);

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);
	uint16_t repeat_timer = 0;

	while (true) {
		vblank_wait();
		uint16_t keys_pressed = scan_keys();
		if (keys_pressed & (KEY_B | KEY_START)) break;

		if (curr_keys & PATTERN_KEY_MASK) {
			uint16_t pattern_keys = curr_keys;
			if (repeat_timer < 18) {
				repeat_timer++;
				pattern_keys = keys_pressed;
			}

			if (pattern_keys & PATTERN_KEY_MASK) {
				if (pattern_keys & KEY_A) pattern_id = (pattern_id + 1) % 3;
				if (pattern_keys & (KEY_X1 | KEY_X2 | KEY_X3 | KEY_X4)) flip_mask ^= 0xFFFF;

				uint16_t __wf_iram *tile_data = (uint16_t __wf_iram*) MEM_TILE(2);
				for (int i = 0; i < 8; i++)
					tile_data[i] = tile_stripe_patterns[8 * pattern_id + i] ^ flip_mask;
			}
		} else {
			repeat_timer = 0;
		}
	}
}

void display_color_bleed(void *userdata) {
	if (!ws_system_mode_set(WS_MODE_COLOR)) return;

	display_stripe_start();

	for (int i = 0; i < 8; i++)
		MEM_COLOR_PALETTE(i)[0] = RGB( 0,  0,  0);
	MEM_COLOR_PALETTE(0)[3] = RGB(15,  0,  0);
	MEM_COLOR_PALETTE(1)[3] = RGB( 0, 15,  0);
	MEM_COLOR_PALETTE(2)[3] = RGB( 0,  0, 15);
	MEM_COLOR_PALETTE(3)[3] = RGB(15, 15, 15);
	MEM_COLOR_PALETTE(4)[3] = RGB( 7,  7,  7);
	MEM_COLOR_PALETTE(5)[3] = RGB(15, 15,  0);
	MEM_COLOR_PALETTE(6)[3] = RGB( 0, 15, 15);
	MEM_COLOR_PALETTE(7)[3] = RGB(15,  0, 15);
	MEM_COLOR_PALETTE(11)[0] = RGB( 0,  0, 15);
	MEM_COLOR_PALETTE(11)[3] = RGB(15,  0,  0);

	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(0), 0, 0, 28, 18);

	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 0) + 2,  2,  2, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 1) + 2,  2,  5, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 2) + 2,  2,  8, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 3) + 2,  2, 11, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 4) + 2,  2, 14, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 5) + 2, 15,  2, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 6) + 2, 15,  5, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 7) + 2, 15,  8, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE( 3) + 2, 15, 11, 11, 2);
	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(11) + 2, 15, 14, 11, 2);

	display_stripe_end();
}

void display_stripes(void *userdata) {
	display_stripe_start();

	ws_display_set_shade_lut(SHADE_LUT_DEFAULT);
	outportw(IO_SCR_PAL_0, MONO_PAL_COLORS(7, 7, 7, 0));

	ws_screen_fill_tiles(screen_1, SCR_ENTRY_PALETTE(0) + 2, 0, 0, 28, 18);

	display_stripe_end();
}
