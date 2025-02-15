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

#ifndef __SUBMENU_H__
#define __SUBMENU_H__

#include <wonderful.h>
#include <ws.h>
#include "main.h"

typedef struct {
	void (*draw)(int, char*, void*);
	void (*key)(int, uint16_t, void*);
	uint16_t flags;
} submenu_entry_t;
#define SUBMENU_ENTRY(name, flags) { name ## _draw, name ## _key, flags }
#define SUBMENU_ENTRY_END() { NULL, NULL, 0 }

typedef struct {
    int8_t x, y; /* provided, can be negative */
    uint16_t tile_start; /* provided */
    uint8_t width; /* provided, excluding padding */
    uint8_t entry_count; /* filled */
    const submenu_entry_t __wf_rom* entries; /* provided */
    void (*tick)(void*, bool); /* provided */
    void *userdata; /* provided */
} submenu_state_t;

extern void submenu_rgb_draw(int id, char* buf, void* userdata);
extern void submenu_rgb_key(int id, uint16_t keys, void* userdata);
extern void submenu_shade_draw(int id, char* buf, void* userdata);
extern void submenu_shade_key(int id, uint16_t keys, void* userdata);
extern void submenu_vrr_draw(int id, char* buf, void* userdata);
extern void submenu_vrr_key(int id, uint16_t keys, void* userdata);
extern const submenu_entry_t __wf_rom submenu_vrr[];

/**
 * Initialize submenu.
 *
 * Should be called after setting mono/color mode.
 *
 * Submenu reserves color palettes 10 and 11, and
 * (width * entry_count + 1) tiles.
 */
void submenu_init(submenu_state_t *state);

/**
 * Run submenu handling loop. 
 *
 * Key A is reserved by the submenu. Other keys are passed straight to the
 * caller.
 */
uint16_t submenu_loop(submenu_state_t *state);

#endif
