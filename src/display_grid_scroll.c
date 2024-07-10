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
#include <ws/keypad.h>

#include "iram.h"
#include "main.h"
#include "submenu.h"

static const uint16_t __wf_rom tile_grid[] = { 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF };

typedef struct {
	int8_t dx, dy;
} grid_scroll_userdata_t;

static void display_grid_scroll_tick(void *ud, bool submenu_active) {
	grid_scroll_userdata_t *userdata = (grid_scroll_userdata_t*) ud;
	outportb(IO_SCR1_SCRL_X, inportb(IO_SCR1_SCRL_X) + userdata->dx);
	outportb(IO_SCR1_SCRL_Y, inportb(IO_SCR1_SCRL_Y) + userdata->dy);
}

void display_grid_scroll(void *userdata) {
	grid_scroll_userdata_t grid_scroll_userdata;
	submenu_state_t submenu;
	submenu.x = 0;
	submenu.y = 0;
	submenu.tile_start = 1;
	submenu.width = 6;
	submenu.entries = submenu_vrr;
	submenu.tick = display_grid_scroll_tick;
	submenu.userdata = &grid_scroll_userdata;

	grid_scroll_userdata.dx = 1;
	grid_scroll_userdata.dy = 0;

	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));

	memcpy(MEM_TILE(0), tile_grid, 16);
	outportw(IO_SCR_PAL_0, MONO_PAL_COLORS(7, 0, 0, 0));
	ws_screen_fill_tiles(screen_1, 0, 0, 0, 32, 32);

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);
	submenu_init(&submenu);

	while (true) {
		uint16_t keys_pressed = submenu_loop(&submenu);
		if (keys_pressed & KEY_X4) grid_scroll_userdata.dx--;
		if (keys_pressed & KEY_X2) grid_scroll_userdata.dx++;
		if (keys_pressed & KEY_X1) grid_scroll_userdata.dy--;
		if (keys_pressed & KEY_X3) grid_scroll_userdata.dy++;
		if (keys_pressed & (KEY_Y1 | KEY_Y2 | KEY_Y3 | KEY_Y4)) {
			grid_scroll_userdata.dx = 0;
			grid_scroll_userdata.dy = 0;
		}
		if (keys_pressed & (KEY_A | KEY_B | KEY_START)) break;
	}
}

