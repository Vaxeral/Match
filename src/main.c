#include <SDL2/SDL.h>

#include <stdio.h> // fprintf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#define LOGERR(message) fprintf(stderr, "%s:%i error: %s\n", __FILE__, __LINE__, message)

int main(int argc, char const *argv[])
{
	int sdl_error = 0;
	
	sdl_error = SDL_InitSubSystem(SDL_INIT_VIDEO);

	if(sdl_error)
	{
		LOGERR(SDL_GetError());
		return EXIT_FAILURE;
	}
	
	SDL_Window *window = SDL_CreateWindow(
		"Hello, SDL2!", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		640, 
		480, 
		0
	);

	if(window == NULL)
	{
		LOGERR(SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(
		window, 
		-1, 
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if(renderer == NULL)
	{
		LOGERR(SDL_GetError());
		return EXIT_FAILURE;
	}

	int running = 1;

	while(running)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{
				running = 0;
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
