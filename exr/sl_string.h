#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "sl_type.h"
#include "sl_arena.h"

typedef struct {
	u8* text;
	usize length;
} String;

String sl_str_alloc(Arena* a, u32 length)
{
	return (String) {
		.text = (u8*) arena_alloc(a, length),
		.length = length
	};
}

String sl_str_make_size(Arena* a, char* cstring, usize length)
{
	String str = sl_str_alloc(a, length);
	memcpy(str.text, cstring, length);
	str.length = length;

	return str;
}

String sl_str_make(Arena* a, char* cstring)
{
	u32 length = 0;
	char ch = cstring[0];

	while (ch != '\0')
	{
		length += 1;
		ch = cstring[length];
	}

	return sl_str_make_size(a, cstring, length);
}

void sl_str_print(String str)
{
	for (usize i=0; i<str.length; i++)
	{
		putchar(str.text[i]);
	}
}

void sl_str_print_ln(String str)
{
	sl_str_print(str);
	putchar('\n');
}

