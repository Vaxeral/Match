#include "match.h"

#include <SDL.h>
#include <SDL_image.h>

#include <time.h>

int match_init(SDL_Window **window, SDL_Renderer **renderer);
int match_events_handle(void);

int main(int argc, char const *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;

	match_init(&window, &renderer);

	struct MatchTexture texture; 
	match_texture_load(&texture, renderer, "resources/yellow-wolly-wog.png");

	struct MatchKeyboard keyboard;
	match_keyboard_init(&keyboard);

	int is_running = 1;
	int x, y;
	x = y = 0;
	double start = time(NULL);
	double end = 0;
	while(is_running)
	{
		match_keyboard_update(&keyboard);
		is_running = match_events_handle();

		x = (x + 1) % (640 - texture.width);
		if(match_keyboard_pressed(&keyboard, SDL_SCANCODE_SPACE))
		{
			y = (y + 1) % (480 - texture.height);
		}

		SDL_SetRenderDrawColor(renderer, COLOR_CREAM);
		SDL_RenderClear(renderer);
		SDL_Rect source_rect = {0, 0, texture.width, texture.height};
		SDL_Rect destination_rect = {x, y, texture.width, texture.height};
		SDL_RenderCopy(renderer, texture.raw, &source_rect, &destination_rect);
		SDL_RenderPresent(renderer);
		end = time(NULL);
		double diff = end - start;
		if(diff >= 1)
		{
			char buffer[512];
			snprintf(buffer, sizeof(buffer), "%i", x);
			LOGMSG(buffer);
			start = time(NULL);
		}
	}
	
	return EXIT_SUCCESS;
}

int match_init(SDL_Window **window, SDL_Renderer **renderer)
{
	ASSERT_NO_ERROR(
		SDL_InitSubSystem(SDL_INIT_VIDEO), SDL_GetError());

	int img_flags = IMG_INIT_PNG;
	int img_flags_set = IMG_Init(img_flags);
	ASSERT(img_flags == img_flags_set, IMG_GetError());
	
	*window = SDL_CreateWindow(
		"Hello, SDL2!", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		640, 480, 
		0
	);

	ASSERT(window, SDL_GetError());

	*renderer = SDL_CreateRenderer(
		*window, 
		-1, 
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	ASSERT(renderer, SDL_GetError());

	return EXIT_SUCCESS;
}

int match_events_handle(void)
{
	int state = 1;
	
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			state = 0;
		} 
		else if(event.type == SDL_WINDOWEVENT)
		{
			if(event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				state = 0;
			}
		}
	}
	return state;
}

#include "match_gem.c"
#include "match_keyboard.c"
#include "match_renderer.c"
