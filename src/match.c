#include "match.h"

#include "SDL.h"
#include "SDL_image.h"

#include <time.h>
#include <stdlib.h>

int match_init(SDL_Window **window, SDL_Renderer **renderer);
int match_events_handle(void);

int main(int argc, char const *argv[])
{
	int hghg;
	int c = 0;
	SDL_Window *window;
	SDL_Renderer *renderer;
	match_init(&window, &renderer);

SDL_version compiled;
SDL_version linked;
SDL_version img_compiled;
SDL_VERSION(&compiled);
SDL_IMAGE_VERSION(&img_compiled);
SDL_GetVersion(&linked);
printf("We compiled against SDL version %d.%d.%d ...\n",
       compiled.major, compiled.minor, compiled.patch);
printf("But we are linking against SDL version %d.%d.%d.\n",
       linked.major, linked.minor, linked.patch);
printf("We compiled against SDL img version %d.%d.%d ...\n",
       img_compiled.major, img_compiled.minor, img_compiled.patch);


	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	struct MatchTexture texture; 
	match_texture_load(
		&texture, 
		renderer, 
		"resources/yellow-wolly-wog.png");
	
	struct MatchTexture gem_texture_atlas;
	match_texture_load(
		&gem_texture_atlas, 
		renderer, 
		"resources/match-gem-texture-atlas.png");

	struct MatchKeyboard keyboard;
	match_keyboard_init(&keyboard);

	struct MatchGemBoard gem_board;
	match_gem_board_init(&gem_board, 32, 16, 16);

	int match_gem_board_top_gap = MATCH_GEM_KIND_CLIP_RECT_HEIGHT / 2;

	struct MatchTexture gem_board_texture_buffer;
	match_texture_init(
		&gem_board_texture_buffer, 
		renderer, 
		SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET, 
		
		MATCH_GEM_KIND_CLIP_RECT_WIDTH 
		* gem_board.columns,

		MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
		* gem_board.visible_rows 
		+ match_gem_board_top_gap);

	srand(time(NULL));
	int i;

	for(i = 0; i < gem_board.columns * (gem_board.rows - gem_board.visible_rows); i++)
	{
		gem_board.state[i] = (struct MatchGemState){
			(rand() % 50 > 25) ? 0 : (rand() % MATCH_GEM_KIND_COUNT - 1) + 1,
			1,
			0,
			0
		};
	}

	int is_running = 1;
	int x, y;
	x = y = 0;
	double start = time(NULL);
	double end = 0;
	double iii = .1;
	int below[16] = {0};
	int belowc[16] = {0};
	double diff;
	int starts = 0;
	while(is_running)
	{
		match_keyboard_update(&keyboard);
		is_running = match_events_handle();

		if(!starts && match_keyboard_just_pressed(&keyboard, SDL_SCANCODE_A))
		{
			for(i = 0; i < gem_board.rows * gem_board.columns; i++)
			{
				if(!match_gem_state_not_null(&gem_board.state[i]))
				{
					below[(i + 16) % gem_board.columns]++;
				}
			}
			starts = 1;
		}
		if(starts)
		{		
			iii *= 1.05;
			for(i = 0; i < gem_board.rows * gem_board.columns; i++)
			{
				if(gem_board.state[i].flags != 2 && belowc[(i + 16) % gem_board.columns] > 0)
				{
					gem_board.state[i].y_animation_offset += iii * ((i + 16) / gem_board.columns) / 2 * diff;
					if(gem_board.state[i].y_animation_offset >= belowc[(i + 16) % gem_board.columns] * 32)
					{
						gem_board.state[i].y_animation_offset = belowc[(i + 16) % gem_board.columns] * 32;
						gem_board.state[i].flags = 2;
					}
				}
				if(!match_gem_state_not_null(&gem_board.state[i]) && belowc[(i + 16) % gem_board.columns] > 0)
				{
					belowc[(i + 16) % gem_board.columns]--;
				}
			}
		}
		x = (x + 1) % (1024 - texture.width);
		memcpy(&belowc[0], &below[0], sizeof(int) * 16);
		SDL_SetRenderDrawColor(renderer, hex_to_rgba(0x22222222));
		SDL_RenderClear(renderer);
		
		SDL_Rect source_rect = {0, 0, texture.width, texture.height};
		SDL_Rect destination_rect = {x, y, texture.width, texture.height};
		SDL_RenderCopy(renderer, texture.raw, &source_rect, &destination_rect);

		SDL_SetRenderTarget(renderer, gem_board_texture_buffer.raw);
			SDL_SetRenderDrawColor(renderer, hex_to_rgba(0x00000000));
			SDL_RenderClear(renderer);
			match_renderer_copy_gem_board(
				renderer, 
				&gem_board, 
				&gem_texture_atlas, 
				0, 
				-MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
				* (gem_board.rows - gem_board.visible_rows) 
				+ match_gem_board_top_gap);
		SDL_SetRenderTarget(renderer, NULL);
		
		source_rect = (SDL_Rect){0, 0, gem_board_texture_buffer.width, gem_board_texture_buffer.height};
		destination_rect = (SDL_Rect){32*4, 32*4, gem_board_texture_buffer.width, gem_board_texture_buffer.height};
		SDL_RenderCopy(renderer, gem_board_texture_buffer.raw, &source_rect, &destination_rect);
		SDL_RenderPresent(renderer);
		
		end = SDL_GetTicks();
		diff = end - start;
		start = end;
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
		1024, 512 + 512 / 2, 
		0
	);

	ASSERT(window, SDL_GetError());

	*renderer = SDL_CreateRenderer(
		*window, 
		-1, 
		SDL_RENDERER_ACCELERATED 
		| SDL_RENDERER_PRESENTVSYNC 
		| SDL_RENDERER_TARGETTEXTURE
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
