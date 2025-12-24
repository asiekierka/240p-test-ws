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

#include "iram.h"
#include "main.h"

static const uint32_t __wf_rom bar_data[3] = {
	0xFF00031C,
	0xFF0FF071,
	0xFFFF3FC7
};

extern uint16_t full_color_line_offset;

#ifdef __WONDERFUL_WWITCH__
__attribute__((section(".text")))
#else
__attribute__((assume_ss_data, interrupt))
#endif
extern void __far full_color_line_int_handler(void);

#ifdef __WONDERFUL_WWITCH__
__attribute__((section(".text")))
#else
__attribute__((assume_ss_data, interrupt))
#endif
static void __far full_color_vblank_int_handler(void) {
	ws_hwint_ack(HWINT_VBLANK);
	full_color_line_offset = 0xFE10;
	outportb(IO_LCD_INTERRUPT, 11);
	for (int i = 0; i < 128; i++)
		MEM_COLOR_PALETTE(0)[(i & 7) + 8 + ((i & ~7) << 1)] = i;
	vbl_ticks++;
}

void display_full_color(void *userdata) {
#ifdef __WONDERFUL_WWITCH__
	intvector_t line_vectors[2];
	intvector_t vblank_vectors[2];
#endif
	
	if (!ws_system_is_color()) return;

	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));

	ws_system_mode_set(WS_MODE_COLOR_4BPP);
	MEM_COLOR_PALETTE(0)[0] = 0;

	ws_screen_fill_tiles(screen_1, 0, 0, 0, 28, 18);
	ws_screen_fill_tiles(screen_2, 0, 0, 0, 28, 32);
	memset(MEM_TILE_4BPP(0), 0, 4 * 32);
	for (int i = 0; i < 3; i++) {
		((uint32_t __wf_iram*) MEM_TILE_4BPP(1 + i))[0] = bar_data[i];
		((uint32_t __wf_iram*) MEM_TILE_4BPP(1 + i))[1] = bar_data[i];
		((uint32_t __wf_iram*) MEM_TILE_4BPP(1 + i))[4] = bar_data[i];
		((uint32_t __wf_iram*) MEM_TILE_4BPP(1 + i))[5] = bar_data[i];
	}
	for (int i = 0; i < 24; i++) {
		ws_screen_fill_tiles(screen_1, (1 + (i % 3)) | SCR_ENTRY_PALETTE(     i / 3 ), 2 + i, 1, 1, 16);
		ws_screen_fill_tiles(screen_2, (1 + (i % 3)) | SCR_ENTRY_PALETTE(8 + (i / 3)), 2 + i, 1, 1, 16);
	}

#ifdef __WONDERFUL_WWITCH__
	line_vectors[0].callback = (void*) FP_OFF(full_color_line_int_handler);
	line_vectors[0].cs = _CS;
	line_vectors[0].ds = _DS;
	vblank_vectors[0].callback = (void*) FP_OFF(full_color_vblank_int_handler);
	vblank_vectors[0].cs = _CS;
	vblank_vectors[0].ds = _DS;
	sys_interrupt_set_hook(SYS_INT_DISPLINE, line_vectors + 0, line_vectors + 1);
	sys_interrupt_set_hook(SYS_INT_VBLANK, vblank_vectors + 0, vblank_vectors + 1);
#else
	cpu_irq_disable();
	ws_int_set_handler(HWINT_IDX_LINE, full_color_line_int_handler);
	ws_int_set_handler(HWINT_IDX_VBLANK, full_color_vblank_int_handler);
	ws_int_set_enabled(HWINT_LINE | HWINT_VBLANK);
	cpu_irq_enable();
#endif

	outportb(IO_SCR2_SCRL_Y, -2);
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE | DISPLAY_SCR2_ENABLE);

	wait_keypress();
	outportb(IO_SCR2_SCRL_Y, 0);

#ifdef __WONDERFUL_WWITCH__
	sys_interrupt_reset_hook(SYS_INT_DISPLINE, line_vectors + 1);
	sys_interrupt_reset_hook(SYS_INT_VBLANK, vblank_vectors + 1);
#endif
}
