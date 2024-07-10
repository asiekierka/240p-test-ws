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

    .arch   i186
    .code16
    .intel_syntax noprefix
    .global grey_ramp_line_int_handler

    .section .fartext.s.grey_ramp_line_int_handler, "a"
    .align 2
grey_ramp_line_int_handler:
    push ax

    // 0x0F -> 0x1E -> ... -> 0xF0
    in al, 0x1E
    add al, 0x0F
    out 0x1E, al

    in al, 0x03
    cmp al, 127
    jae __grey_ramp_no_more_lines
    add al, 8
    out 0x03, al

__grey_ramp_no_more_lines:
    mov al, 0x10
    out 0xB6, al
    pop ax
    iret
