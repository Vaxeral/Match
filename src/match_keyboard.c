#include "match_keyboard.h"

void match_keyboard_init(struct MatchKeyboard *keyboard)
{
	keyboard->was_pressed_currently = SDL_GetKeyboardState(NULL);
}

/*
	Call before polling events.
*/
void match_keyboard_update(struct MatchKeyboard *keyboard)
{
	memcpy(keyboard->was_pressed_previously, keyboard->was_pressed_currently, SDL_NUM_SCANCODES * sizeof(Uint8));
}

int match_keyboard_just_pressed(struct MatchKeyboard *keyboard, int scancode)
{
	return !keyboard->was_pressed_previously[scancode] && keyboard->was_pressed_currently[scancode];
}

int match_keyboard_pressed(struct MatchKeyboard *keyboard, int scancode)
{
	return keyboard->was_pressed_currently[scancode];
}

int match_keyboard_released(struct MatchKeyboard *keyboard, int scancode)
{
	return keyboard->was_pressed_previously[scancode] && !keyboard->was_pressed_currently[scancode];
}