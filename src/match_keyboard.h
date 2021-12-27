#ifndef MATCH_KEYBOARD_HEADER_202112270858
#define MATCH_KEYBOARD_HEADER_202112270858

#include "match_definitions.h"

#include <SDL_types.h>
#include <SDL_scancode.h>
#include <SDL_keyboard.h>

struct MatchKeyboard
{
	const Uint8 *was_pressed_currently;
	Uint8 was_pressed_previously[SDL_NUM_SCANCODES];
};

void match_keyboard_init(struct MatchKeyboard *keyboard);

/*
	Call before polling events.
*/
void match_keyboard_update(struct MatchKeyboard *keyboard);
int match_keyboard_just_pressed(struct MatchKeyboard *keyboard, int scancode);
int match_keyboard_pressed(struct MatchKeyboard *keyboard, int scancode);
int match_keyboard_released(struct MatchKeyboard *keyboard, int scancode);

#endif