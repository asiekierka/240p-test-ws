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
#ifdef __WONDERFUL_WWITCH__
#include <sys/bios.h>
#endif

#include "../build/assets/fonts.h"

void tile_copy_2bpp_to_4bpp(uint8_t __wf_iram* dest, const uint8_t __wf_rom* src, size_t len) {
	uint16_t __wf_iram* dest16 = (uint16_t __wf_iram*) dest;
	const uint16_t __wf_rom* src16 = (const uint16_t __wf_rom*) src;

	while (len) {
		*(dest16++) = *(src16++);
		*(dest16++) = 0;
		len -= 2;
	}
}
