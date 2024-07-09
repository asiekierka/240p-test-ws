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
    .global full_color_line_offset
    .global full_color_line_int_handler

    .section .bss
    .align 2
full_color_line_offset:
    .word 0

    .section .fartext.s.full_color_line_int_handler, "a"
    .align 2
full_color_line_int_handler:
    push ax
    push ds
    push ss
    pop ds
    push si

    mov ax, 128
    mov si, word ptr [full_color_line_offset]
.rept 8
    add word ptr [si + 0], ax
    add word ptr [si + 2], ax
    add word ptr [si + 4], ax
    add word ptr [si + 6], ax
    add word ptr [si + 8], ax
    add word ptr [si + 10], ax
    add word ptr [si + 12], ax
    add word ptr [si + 14], ax
    add si, 0x20
.endr
    xor word ptr [full_color_line_offset], 0x100

    in al, 0x03
    cmp al, 134
    jae __full_color_no_more_lines
    add al, 2
    out 0x03, al

__full_color_no_more_lines:
    mov al, 0x10
    out 0xB6, al
    pop si
    pop ds
    pop ax
    iret
