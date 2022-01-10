#ifndef MATCH_KEYBOARD_HEADER_202112270858
#define MATCH_KEYBOARD_HEADER_202112270858

#include "match_definitions.h"

#include "SDL_types.h"
#include "SDL_scancode.h"
#include "SDL_keyboard.h"

#include <memory.h>

struct MatchKeyboard;

extern struct MatchKeyboard match_keyboard;

struct MatchKeyboard
{
	const Uint8 *was_pressed_currently;
	Uint8 was_pressed_previously[SDL_NUM_SCANCODES];
};

void match_keyboard_init(void);

/*
	Call before polling events.
*/
void match_keyboard_update(void);
int match_keyboard_just_pressed(int scancode);
int match_keyboard_pressed(int scancode);
int match_keyboard_released(int scancode);

#endif