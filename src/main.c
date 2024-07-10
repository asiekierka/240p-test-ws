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
#ifdef __WONDERFUL_WWITCH__
#include <sys/bios.h>
#endif

#include "../build/assets/fonts.h"

#define IRAM_IMPLEMENTATION
#include "iram.h"

// === General constants, IRQ handler ===

volatile uint16_t vbl_ticks = 0;
static uint16_t last_keys = 0;
uint16_t curr_keys = 0;

uint16_t scan_keys(void) {
	last_keys = curr_keys;
#ifdef __WONDERFUL_WWITCH__
	curr_keys = key_press_check();
#else
	curr_keys = ws_keypad_scan();
#endif
	return curr_keys & ~last_keys;
}

#ifndef __WONDERFUL_WWITCH__
__attribute__((assume_ss_data, interrupt))
void __far vblank_int_handler(void) {
        ws_hwint_ack(HWINT_VBLANK);
        vbl_ticks++;
}

void vblank_wait(void) {
        uint16_t vbl_ticks_last = vbl_ticks;

        while (vbl_ticks == vbl_ticks_last) {
                cpu_halt();
        }
}

void wait_keypress(void) {
	while (!scan_keys()) vblank_wait();
}
#else
void vblank_wait(void) {
	sys_wait(1);
}

void wait_keypress(void) {
	key_wait();
}

static const char __wf_rom display_exit_name[] = "Exit to Freya";
void display_exit(void *userdata) {
	bios_exit();
}
#endif

void tile_copy_2bpp_to_4bpp(uint8_t __wf_iram* dest, const uint8_t __wf_rom* src, size_t len) {
	uint16_t __wf_iram* dest16 = (uint16_t __wf_iram*) dest;
	const uint16_t __wf_rom* src16 = (const uint16_t __wf_rom*) src;

	while (len) {
		*(dest16++) = *(src16++);
		*(dest16++) = 0;
		len -= 2;
	}
}

// === Test patterns ===

static const char __wf_rom display_color_bars_name[] = "Color bars";
void display_color_bars(void *userdata);

static const char __wf_rom display_drop_shadow_name[] = "Drop shadow test";
void display_drop_shadow(void *userdata);

static const char __wf_rom display_full_color_name[] = "Full color palette";
void display_full_color(void *userdata);

static const char __wf_rom display_grid_name[] = "Grid";
void display_grid(void *userdata);

static const char __wf_rom display_grid_scroll_name[] = "Grid scroll test";
void display_grid_scroll(void *userdata);

static const char __wf_rom display_pluge_name[] = "PLUGE";
void display_pluge(void *userdata);

static const char __wf_rom display_solid_color_name[] = "Solid color";
void display_solid_color(void *userdata);

// === Menu system ===

#define MF_COLOR_ONLY (1 << 0)
typedef struct {
	const char __wf_rom *name;
	void (*action)(void*);
	void *userdata;
	uint16_t flags;
} menu_entry_t;
#define MENU_ENTRY_TITLE(title) { title ## _name, NULL, NULL, 0 }
#define MENU_ENTRY(func, data, flags) { func ## _name, func, data, flags }
#define MENU_ENTRY_END() { NULL, NULL, NULL, 0 }

static const char __wf_rom main_menu_name[] = "- 144p Test Suite for WS (0.2.0) -";
static const menu_entry_t __wf_rom main_menu_entries[] = {
	MENU_ENTRY_TITLE(main_menu),
	MENU_ENTRY(display_pluge, NULL, 0),
	MENU_ENTRY(display_color_bars, NULL, MF_COLOR_ONLY),
	MENU_ENTRY(display_solid_color, NULL, 0),
#ifndef __WONDERFUL_WWITCH__
	MENU_ENTRY(display_full_color, NULL, MF_COLOR_ONLY),
#endif
	MENU_ENTRY(display_grid, NULL, 0),
	MENU_ENTRY(display_drop_shadow, NULL, 0),
	MENU_ENTRY(display_grid_scroll, NULL, 0),
#ifdef __WONDERFUL_WWITCH__
	MENU_ENTRY(display_exit, NULL, 0),
#endif
	MENU_ENTRY_END()
};

int vwf8_get_string_width(const char __wf_rom* s) {
    int w = 0;

    while (*s) {
        uint8_t chr = *(s++);
        const uint8_t __wf_rom* font = font8_bitmap + ((chr - 0x20) * 9);
        w += *font;
    }

    return w;
}

static const uint8_t __wf_rom font_width_mask[] = {
	0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF
};

int vwf8_draw_char(uint8_t __wf_iram* tile, uint8_t chr, int x) {
    const uint8_t __wf_rom* font = font8_bitmap + ((chr - 0x20) * 9);

    int width = *(font++);
    int next_x = x + width;
    int x_fine = (17 - width - (x & 0x7));

    uint16_t mask = ~(font_width_mask[width] << x_fine);

    if (ws_system_mode_get() & 0x40) {
        tile += (x >> 3) << 5;

        for (int i = 0; i < 8; i++, font++, tile += 4) {
            uint16_t shift = (*font) << x_fine;
            tile[0] = ((tile[0] & (mask >> 8)) | (shift >> 8)) & 0xFF;
            tile[32] = ((tile[32] & mask) | shift) & 0xFF;
        }
    } else {
        tile += (x >> 3) << 4;

        for (int i = 0; i < 8; i++, font++, tile += 2) {
            uint16_t shift = (*font) << x_fine;
            tile[0] = ((tile[0] & (mask >> 8)) | (shift >> 8)) & 0xFF;
            tile[16] = ((tile[16] & mask) | shift) & 0xFF;
        }
    }

    return next_x;
}

int vwf8_draw_string(uint8_t __wf_iram* tile, const char __wf_rom* s, int x) {
	while (*s) {
		x = vwf8_draw_char(tile, *(s++), x);
	}
	return x;
}

static const uint16_t __wf_rom tile_bg[8] = { 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFFFF, 0x00 };

static bool can_use_entry(const menu_entry_t __wf_rom* entry) {
	if (entry->flags & MF_COLOR_ONLY) {
		if (!ws_system_is_color()) {
			return false;
		}
	}
	return true;
}

__attribute__((noinline))
static int display_menu_init(const menu_entry_t __wf_rom* entries, int menu_y) {
#ifndef __WONDERFUL_WWITCH__
	cpu_irq_disable();
	ws_hwint_set_handler(HWINT_IDX_VBLANK, vblank_int_handler);
	ws_hwint_set(HWINT_VBLANK);
	cpu_irq_enable();
#endif

	outportw(IO_DISPLAY_CTRL, 0x0700);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));
	ws_system_mode_set(WS_MODE_MONO);

	int menu_count = 0;

	memset(MEM_TILE(0), 0, 0x2000 - 16);
	memcpy(MEM_TILE(511), tile_bg, sizeof(tile_bg));

	ws_display_set_shade_lut(SHADE_LUT_DEFAULT);
	outportw(IO_SCR_PAL_0, MONO_PAL_COLORS(1, 7, 3, 0));
	outportw(IO_SCR_PAL_1, MONO_PAL_COLORS(7, 0, 3, 0));
	outportw(IO_SCR_PAL_2, MONO_PAL_COLORS(1, 3, 3, 0));

	// Initialize tile layout
	for (int ix = 0; ix < 28; ix++) {
		ws_screen_put_tile(screen_1, 511, ix, 0);
		ws_screen_put_tile(screen_1, 511 | SCR_ATTR_FLIP_V, ix, 17);

		for (int iy = 0; iy < 16; iy++)
			ws_screen_put_tile(screen_1, ((iy << 5) + ix), ix, iy + 1);
	}
	ws_screen_modify_tiles(screen_1, 0xFFFF, SCR_ENTRY_PALETTE(1), 0, menu_y + 2, 28, 1);

	// Draw title
	{
		const char __wf_rom *title = (entries - 1)->name;
		int title_w = vwf8_get_string_width(title);
		int mx = (DISPLAY_WIDTH_PX - title_w) >> 1;
		vwf8_draw_string((uint8_t __wf_iram*) MEM_TILE(0), title, mx);
	}

	const menu_entry_t __wf_rom* drawn_entry = entries;

	// Draw entries
	{
		int my = 32;
		while (drawn_entry->action) {
			if (!can_use_entry(drawn_entry))
				ws_screen_modify_tiles(screen_1, 0xFFFF, SCR_ENTRY_PALETTE(2), 0, (my >> 5) + 1, 28, 1);
			vwf8_draw_string((uint8_t __wf_iram*) MEM_TILE(my), drawn_entry->name, 4);
			drawn_entry++;
			menu_count++;
			my += 32;
		}
	}

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);
	return menu_count;
}

__attribute__((optimize("-O0")))
void display_menu(const menu_entry_t __wf_rom* entries) {
	int menu_y = 0;
	entries++;

menu_redraw:
	; int menu_count = display_menu_init(entries, menu_y);

	while (true) {
		vblank_wait();
		int last_menu_y = menu_y;
		uint16_t keys_pressed = scan_keys();
		if (keys_pressed & KEY_A) {
			entries[menu_y].action(entries[menu_y].userdata);
			goto menu_redraw;
		}
		if (keys_pressed & KEY_X1) {
			if (menu_y > 0) {
				menu_y--;
				while (!can_use_entry(entries + menu_y)) menu_y--;
			}
		}
		if (keys_pressed & KEY_X3) {
			if (menu_y < (menu_count - 1)) {
				menu_y++;
				while (!can_use_entry(entries + menu_y)) menu_y++;
			}
		}
		if (menu_y != last_menu_y) {
			ws_screen_modify_tiles(screen_1, 0x01FF, SCR_ENTRY_PALETTE(0), 0, last_menu_y + 2, 28, 1);
			ws_screen_modify_tiles(screen_1, 0x01FF, SCR_ENTRY_PALETTE(1), 0, menu_y + 2, 28, 1);
		}
	}
};

void main(void) {
	display_menu(main_menu_entries);
	while(1);
}

