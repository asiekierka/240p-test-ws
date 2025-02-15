#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <wonderful.h>

#define DEFINE_STRING_LOCAL(name, value) static const char __wf_rom name[] = value
#ifdef STRINGS_H_IMPLEMENTATION
#define DEFINE_STRING(name, value) const char __wf_rom name[] = value
#else
#define DEFINE_STRING(name, value) extern const char __wf_rom name[]
#endif

DEFINE_STRING(s_red_format, "Red: %d");
DEFINE_STRING(s_green_format, "Green: %d");
DEFINE_STRING(s_blue_format, "Blue: %d");
DEFINE_STRING(s_shade_format, "Shade: %d");
DEFINE_STRING(s_red, "Red");
DEFINE_STRING(s_green, "Green");
DEFINE_STRING(s_blue, "Blue");
DEFINE_STRING(s_white, "White");
DEFINE_STRING(s_hex_digits, "0123456789ABCDEF");

#endif /* _STRINGS_H_ */
