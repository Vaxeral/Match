#include "match_keyboard.h"

struct MatchKeyboard match_keyboard;

void match_keyboard_init(void)
{
	match_keyboard.was_pressed_currently = SDL_GetKeyboardState(NULL);
}

/*
	Call before polling events.
*/
void match_keyboard_update(void)
{
	memcpy(match_keyboard.was_pressed_previously, match_keyboard.was_pressed_currently, SDL_NUM_SCANCODES * sizeof(Uint8));
}

int match_keyboard_just_pressed(int scancode)
{
	return !match_keyboard.was_pressed_previously[scancode] && match_keyboard.was_pressed_currently[scancode];
}

int match_keyboard_pressed(int scancode)
{
	return match_keyboard.was_pressed_currently[scancode];
}

int match_keyboard_released(int scancode)
{
	return match_keyboard.was_pressed_previously[scancode] && !match_keyboard.was_pressed_currently[scancode];
}