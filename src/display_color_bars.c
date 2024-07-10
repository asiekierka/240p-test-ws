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

static const char __wf_rom text_red[] = "Red";
static const char __wf_rom text_green[] = "Green";
static const char __wf_rom text_blue[] = "Blue";
static const char __wf_rom text_white[] = "White";
static const char __wf_rom hex_digits[] = "0123456789ABCDEF";
static const uint32_t __wf_rom bar_data[24] = {
    0x00000000,
    0x0000000F,
    0x000000FF,
    0x0000FF00,
    0x0000FF0F,
    0x0000FFFF,
    0x00FF0000,
    0x00FF000F,
    0x00FF00FF,
    0x00FFFF00,
    0x00FFFF0F,
    0x00FFFFFF,
    0xFF000000,
    0xFF00000F,
    0xFF0000FF,
    0xFF00FF00,
    0xFF00FF0F,
    0xFF00FFFF,
    0xFFFF0000,
    0xFFFF000F,
    0xFFFF00FF,
    0xFFFFFF00,
    0xFFFFFF0F,
    0xFFFFFFFF
};

void display_color_bars(void *userdata) {
	if (!ws_system_is_color()) return;

	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));

	ws_system_mode_set(WS_MODE_COLOR_4BPP);
	MEM_COLOR_PALETTE(0)[0] = RGB(0, 0, 0);
	MEM_COLOR_PALETTE(0)[1] = RGB(15, 15, 15);
	for (int i = 1; i < 16; i++) {
		MEM_COLOR_PALETTE( 8)[i] = i * 0x100;
		MEM_COLOR_PALETTE( 9)[i] = i * 0x10;
		MEM_COLOR_PALETTE(10)[i] = i * 0x1;
		MEM_COLOR_PALETTE(11)[i] = i * 0x111;
	}

	ws_screen_fill_tiles(screen_1, 0, 0, 0, 32, 19);
	memset(MEM_TILE_4BPP(0), 0, 128 * 32);
	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 8; j++)
			((uint32_t __wf_iram*) MEM_TILE_4BPP(256 + i))[j] = bar_data[i];

		ws_screen_put_tile(screen_1, 32 + i, 3 + i, 2);
		ws_screen_put_tile(screen_1, 32 + i, 3 + i, 6);
		ws_screen_put_tile(screen_1, 32 + i, 3 + i, 10);
		ws_screen_put_tile(screen_1, 32 + i, 3 + i, 14);
		ws_screen_put_tile(screen_1, (256 + i) | SCR_ENTRY_PALETTE( 8), 3 + i, 3);
		ws_screen_put_tile(screen_1, (256 + i) | SCR_ENTRY_PALETTE( 8), 3 + i, 4);
		ws_screen_put_tile(screen_1, (256 + i) | SCR_ENTRY_PALETTE( 9), 3 + i, 7);
		ws_screen_put_tile(screen_1, (256 + i) | SCR_ENTRY_PALETTE( 9), 3 + i, 8);
		ws_screen_put_tile(screen_1, (256 + i) | SCR_ENTRY_PALETTE(10), 3 + i, 11);
		ws_screen_put_tile(screen_1, (256 + i) | SCR_ENTRY_PALETTE(10), 3 + i, 12);
		ws_screen_put_tile(screen_1, (256 + i) | SCR_ENTRY_PALETTE(11), 3 + i, 15);
		ws_screen_put_tile(screen_1, (256 + i) | SCR_ENTRY_PALETTE(11), 3 + i, 16);
	}
	for (int i = 0; i < 3; i++) {
		ws_screen_put_tile(screen_1, 80 + i, i, 3);
		ws_screen_put_tile(screen_1, 84 + i, i, 7);
		ws_screen_put_tile(screen_1, 88 + i, i, 11);
		ws_screen_put_tile(screen_1, 92 + i, i, 15);
	}
	for (int i = 0; i < 16; i++) {
		vwf8_draw_char((uint8_t __wf_iram*) MEM_TILE_4BPP(32), hex_digits[i], i * 12);
	}
	vwf8_draw_string((uint8_t __wf_iram*) MEM_TILE_4BPP(80), text_red, 0);
	vwf8_draw_string((uint8_t __wf_iram*) MEM_TILE_4BPP(84), text_green, 0);
	vwf8_draw_string((uint8_t __wf_iram*) MEM_TILE_4BPP(88), text_blue, 0);
	vwf8_draw_string((uint8_t __wf_iram*) MEM_TILE_4BPP(92), text_white, 0);

        outportb(IO_SCR1_SCRL_X, 252);
        outportb(IO_SCR1_SCRL_Y, 6);
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	wait_keypress();
        outportb(IO_SCR1_SCRL_X, 0);
        outportb(IO_SCR1_SCRL_Y, 0);
}

