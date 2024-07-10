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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <wonderful.h>
#include <ws.h>
#include <ws/display.h>
#include <ws/hardware.h>
#include <ws/keypad.h>
#include <ws/system.h>
#include "submenu.h"

#include "iram.h"

void submenu_init(submenu_state_t *state) {
    state->entry_count = 0;
    const submenu_entry_t __wf_rom *current_entry = state->entries;

    while (current_entry->draw) {
        current_entry++;
        state->entry_count++;
    }

    uint16_t tile_count = (state->width * state->entry_count) + 1;
    if (ws_system_mode_get() & 0x40) {
        // 4BPP color: use colors 14 and 15 of color palettes 10 and 11.
        uint32_t __wf_iram *tile_data = (uint32_t __wf_iram*) MEM_TILE_4BPP(state->tile_start);
        for (int i = 0; i < tile_count * 8; i++) {
            *(tile_data++) = 0xFFFFFF00;
        }
        MEM_COLOR_PALETTE(10)[14] = RGB( 0,  0,  0);
        MEM_COLOR_PALETTE(10)[15] = RGB(15, 15, 15);
        MEM_COLOR_PALETTE(11)[14] = RGB(15, 15, 15);
        MEM_COLOR_PALETTE(11)[15] = RGB( 0,  0,  0);
    } else {
        // 2BPP: use colors 0 and 1 of color palettes 10 and 11.
        memset(MEM_TILE(state->tile_start), 0, tile_count * 32);

        if (ws_system_color_active()) {
            // 2BPP color
            MEM_COLOR_PALETTE(10)[0] = RGB( 0,  0,  0);
            MEM_COLOR_PALETTE(10)[1] = RGB(15, 15, 15);
            MEM_COLOR_PALETTE(11)[0] = RGB(15, 15, 15);
            MEM_COLOR_PALETTE(11)[1] = RGB( 0,  0,  0);
        } else {
            // 2BPP mono
            outportw(IO_SCR_PAL_10, MONO_PAL_COLORS(7, 0, 0, 0));
            outportw(IO_SCR_PAL_11, MONO_PAL_COLORS(0, 7, 0, 0));
        }
    }

    if (state->x < 0) state->x += DISPLAY_WIDTH - (state->width + 2);
    if (state->y < 0) state->y += DISPLAY_HEIGHT - (state->entry_count + 2);

    // prepare tile data
    ws_screen_fill_tiles(
        screen_2, SCR_ATTR_PALETTE(10) + state->tile_start,
        state->x, state->y, state->width + 2, state->entry_count + 2);

    int i = state->tile_start + 1;
    for (int iy = 0; iy < state->entry_count; iy++) {
        for (int ix = 0; ix < state->width; ix++) {
            ws_screen_put_tile(
                screen_2, SCR_ATTR_PALETTE(10) + (i++),
                state->x + 1 + ix, state->y + 1 + iy);
        }
    }

    // configure window
    outportb(IO_SCR2_WIN_X1, state->x * 8 + 4);
    outportb(IO_SCR2_WIN_Y1, state->y * 8 + 4);
    outportb(IO_SCR2_WIN_X2, (state->x + 2 + state->width) * 8 - 5);
    outportb(IO_SCR2_WIN_Y2, (state->y + 2 + state->entry_count) * 8 - 5);
    outportb(IO_DISPLAY_CTRL, inportb(IO_DISPLAY_CTRL) | DISPLAY_SCR2_WIN_INSIDE);
}

__attribute__((noinline, optimize("-O0")))
static void submenu_draw_compiler_workaround(const submenu_entry_t __wf_rom *current_entry, int i, char *text, void *userdata) {
    current_entry->draw(i, text, userdata);
}

void submenu_draw(submenu_state_t *state, uint16_t selected) {
    char text[47];
    uint16_t i = 0;
    uint16_t tile = state->tile_start + 1;
    const submenu_entry_t __wf_rom *current_entry = state->entries;

    while (current_entry->draw) {
        submenu_draw_compiler_workaround(current_entry, i, text, state->userdata);
        if (ws_system_mode_get() & 0x40) {
            for (int i = 0; i < state->width * 8; i++) {
                MEM_TILE_4BPP(tile)[i * 4] = 0;
            }
            vwf8_draw_string(MEM_TILE_4BPP(tile), text, 1);
        } else {
            memset(MEM_TILE(tile), 0, 16 * state->width);
            vwf8_draw_string(MEM_TILE(tile), text, 1);
        }
        ws_screen_modify_tiles(
            screen_2, ~SCR_ATTR_PALETTE_MASK, (i == selected) ? SCR_ATTR_PALETTE(11) : SCR_ATTR_PALETTE(10),
            state->x + 1, state->y + 1 + i, state->width, 1);
        
        current_entry++;
        i++;
        tile += state->width;
    }
}

__attribute__((noinline, optimize("-O0")))
static void submenu_key_compiler_workaround(submenu_state_t *state, int id, const submenu_entry_t __wf_rom* entry, uint16_t keys) {
    entry->key(id, keys, state->userdata);
}

__attribute__((noinline, optimize("-O0")))
static void submenu_tick_compiler_workaround(submenu_state_t *state, bool submenu_active) {
    if (state->tick) state->tick(state->userdata, submenu_active);
}

uint16_t submenu_loop(submenu_state_t *state) {
    uint16_t submenu_option = 0;
    bool submenu_active = false;
    bool submenu_redraw = false;

	while (true) {
        if (submenu_redraw) {
            submenu_draw(state, submenu_option);
            outportw(IO_DISPLAY_CTRL, inportw(IO_DISPLAY_CTRL) | DISPLAY_SCR2_ENABLE);
            submenu_redraw = false;
        }
        vblank_wait();
        submenu_tick_compiler_workaround(state, submenu_active);

		uint16_t keys_pressed = scan_keys();
        if (submenu_active) {
            if (keys_pressed & KEY_B) {
                submenu_active = false;
                outportw(IO_DISPLAY_CTRL, inportw(IO_DISPLAY_CTRL) & ~DISPLAY_SCR2_ENABLE);
                continue;
            }

            if (keys_pressed & KEY_X1) {
                do {
                    if (submenu_option == 0) submenu_option = state->entry_count;
                    submenu_option--;
                } while (/*!can_use_entry(entries + menu_y)*/0);
                submenu_redraw = true;
                continue;
            }
            
            if (keys_pressed & KEY_X3) {
                do {
                    submenu_option++;
                    if (submenu_option >= state->entry_count) submenu_option = 0;
                } while (/*!can_use_entry(entries + menu_y)*/0);
                submenu_redraw = true;
                continue;
            }

            if (keys_pressed) {
                submenu_key_compiler_workaround(state, submenu_option, state->entries + submenu_option, keys_pressed);
                submenu_redraw = true;
            }
        } else {
            if (keys_pressed & KEY_A) {
                submenu_option = 0;
                submenu_active = true;
                submenu_redraw = true;
            } else if (keys_pressed) {
                return keys_pressed;
            }
        }
    }

    return 0;
}

static const char __wf_rom vrr_format[] = "%d lines";

void submenu_vrr_draw(int id, char* buf, void* userdata) {
    sprintf(buf, vrr_format, inportb(0x16) + 1);
}

void submenu_vrr_key(int id, uint16_t keys, void* userdata) {
    uint16_t curr_value = inportb(0x16);
    int vsync_offset = inportb(0x17) - curr_value;

    if (keys & KEY_X4) {
        curr_value -= 2;
    } else if (keys & KEY_X2) {
        curr_value += 2;
    }
    if (curr_value >= 158 && curr_value <= 254) {
        while (inportb(IO_LCD_LINE) >= 144);
        outportb(0x16, curr_value);
        outportb(0x17, curr_value + vsync_offset);
    }
}

const submenu_entry_t __wf_rom submenu_vrr[] = {
    SUBMENU_ENTRY(submenu_vrr, 0),
    SUBMENU_ENTRY_END()
};
