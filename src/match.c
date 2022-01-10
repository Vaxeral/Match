#include "match.h"

#include "SDL.h"
#include "SDL_image.h"

#include <time.h>
#include <stdlib.h>
// push animations into a queue.
// code board logic and verify operations.
int match_init(void);
int match_events_handle(void);

void collapse_board(char* board, int rows, int columns)
{
    int empty_index;
    int i, j;

    for(i = 0; i < columns; i++)
    {
        empty_index = -1;
        for(j = rows - 1; j >= 0; j--)
        {
            if(board[i + j * columns] == ' ')
            {
                if(empty_index == -1) empty_index = j;
            }
            else
            {
                if(empty_index != -1)
                {
                    int tmp = board[i + j * columns];
                    board[i + j * columns] = ' ';
                    board[i + empty_index * columns] = tmp;
                    empty_index--;
                }
            }
        }
    }
}


#define match_gem_board_matches_vertical_mark                           \
streak = 1;                                                             \
                                                                        \
for(i = 0; i < columns; i++)                                            \
{                                                                       \
    for(j = 0; j < rows - 1; j++)                                       \
    {                                                                   \
        previous = state + i + j * columns;                             \
        current = state + i + (j + 1) * columns;                        \
        if(previous->kind == current->kind)                             \
        {                                                               \
            streak++;                                                   \
        }                                                               \
        else                                                            \
        {                                                               \
            if(streak >= streak_max)                                    \
            {                                                           \
                int offset;                                             \
                for(offset = 0; offset < streak; offset++)              \
                    matches[(previous - offset * columns) - state] = 1; \
            }                                                           \
            streak = 1;                                                 \
        }                                                               \
    }                                                                   \
                                                                        \
    if(streak >= streak_max)                                            \
    {                                                                   \
        int offset;                                                     \
        for(offset = 0; offset < streak; offset++)                      \
            matches[(current - offset * columns) - state] = 1;          \
    }                                                                   \
    streak = 1;                                                         \
}                                                                       

#define match_gem_board_matches_horizontal_mark                 \
streak = 1;                                                     \
                                                                \
for(j = 0; j < rows; j++)                                       \
{                                                               \
    for(i = 0; i < columns - 1; i++)                            \
    {                                                           \
        previous = state + i + j * columns;                     \
        current = state + (i + 1) + j * columns;                \
        if(previous->kind == current->kind)                     \
        {                                                       \
            streak++;                                           \
        }                                                       \
        else                                                    \
        {                                                       \
            if(streak >= streak_max)                            \
            {                                                   \
                int offset;                                     \
                for(offset = 0; offset < streak; offset++)      \
                    matches[(previous - offset) - state] = 1;   \
            }                                                   \
            streak = 1;                                         \
        }                                                       \
    }                                                           \
    if(streak >= streak_max)                                    \
    {                                                           \
        int offset;                                             \
        for(offset = 0; offset < streak; offset++)              \
            matches[(current - offset) - state] = 1;            \
    }                                                           \
    streak = 1;                                                 \
}                                                               

void match_gem_board_matches_clear(struct MatchGemBoard *board, int *cleared)
{
    int i, j, streak;
    
    struct MatchGemState const *previous;
    struct MatchGemState const *current;

    int const rows = board->rows;
    int const columns = board->columns;
    int const size = board->rows * board->columns;
    int matches[size];
    for(i = 0; i < size; i++)
        matches[i] = 0;

    int const streak_max = 3;

    struct MatchGemState *const state = board->state;

    match_gem_board_matches_vertical_mark
    match_gem_board_matches_horizontal_mark

    for(i = 0; i < columns; i++)
        cleared[i] = 0;

    for(i = 0; i < size; i++)
        if(matches[i])
        {
            state[i] = (struct MatchGemState){0};
            cleared[i % columns]++;
        }
}

int main(int argc, char const *argv[])
{
	match_init();

	struct MatchTexture texture;
	match_texture_load(&texture, "resources/yellow-wolly-wog.png");
	
	match_texture_load(&gem_texture_atlas, "resources/match-gem-texture-atlas-export.png");

    struct MatchGemBoard gem_board64;
    struct MatchGemBoard gem_board256;

    match_gem_board_init(&gem_board64, 16, 8, 8);
	match_gem_board_init(&gem_board256, 20, 10, 10);

    gem_board = gem_board256;

	int match_gem_board_top_gap = MATCH_GEM_KIND_CLIP_RECT_HEIGHT / 2;

	struct MatchTexture gem_board_texture_buffer;
	match_texture_init(
		&gem_board_texture_buffer,
		SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET, 
		
		MATCH_GEM_KIND_CLIP_RECT_WIDTH 
		* gem_board.columns,

		MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
		* gem_board.visible_rows 
		+ match_gem_board_top_gap);
	int i;

	for(i = 0; i < gem_board.columns * gem_board.rows; i++)
	{
		gem_board.state[i] = (struct MatchGemState){
			(0) ? 0 : (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1,
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
	double iii = .25;
	int below[16] = {0};
	int belowc[16] = {0};
	double diff;
	int starts = 0;
    int object_animation_count = 0;
	while(is_running)
	{
		match_keyboard_update();
		is_running = match_events_handle();

		// if(!starts && match_keyboard_just_pressed(SDL_SCANCODE_A))
		// {
  //           for(i = 0; i < 16; i++) below[i] = 0;
		// 	for(i = 0; i < gem_board.rows * gem_board.columns; i++)
		// 	{
		// 		if(match_gem_state_is_null(&gem_board.state[i]))
		// 		{
		// 			below[i % gem_board.columns]++;
		// 		}
		// 	}
		// 	starts = 1;
		// }
        
        if(match_keyboard_just_pressed(SDL_SCANCODE_P))  
        {
            match_gem_board_matches_clear(&gem_board, below);
            starts = 1;
        }      
        if(match_keyboard_just_pressed(SDL_SCANCODE_B))
        {
            iii = .25;
            starts = 0;
            gem_board = gem_board256;
            SDL_DestroyTexture(gem_board_texture_buffer.raw);
                match_texture_init(
        &gem_board_texture_buffer,
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        
        MATCH_GEM_KIND_CLIP_RECT_WIDTH 
        * gem_board.columns,

        MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
        * gem_board.visible_rows 
        + match_gem_board_top_gap);
            for(i = 0; i < gem_board.columns * gem_board.rows; i++)
            {
                gem_board.state[i] = (struct MatchGemState){
                    (0) ? 0 : (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1,
                    1,
                    0,
                    0
                };
            }
        }
        if(match_keyboard_just_pressed(SDL_SCANCODE_C))
        {
            iii = .25;
            starts = 0;
            gem_board = gem_board64;
            SDL_DestroyTexture(gem_board_texture_buffer.raw);
                match_texture_init(
        &gem_board_texture_buffer,
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        
        MATCH_GEM_KIND_CLIP_RECT_WIDTH 
        * gem_board.columns,

        MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
        * gem_board.visible_rows 
        + match_gem_board_top_gap);
            for(i = 0; i < gem_board.columns * gem_board.rows; i++)
            {
                gem_board.state[i] = (struct MatchGemState){
                    (0) ? 0 : (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1,
                    1,
                    0,
                    0
                };
            }
        }
        if(object_animation_count == 256)
        {
            printf("Done animating\n");
            object_animation_count = 0;
        }
		if(starts)
		{		
            memcpy(&belowc[0], &below[0], sizeof(int) * 16);
			iii *= 1.03;
			for(i = 0; i < gem_board.rows * gem_board.columns; i++)
			{
				if(gem_board.state[i].flags && gem_board.state[i].flags != 2 && belowc[i % gem_board.columns] > 0)
				{
                    /*
                        Used for staggered drop effect at start.
                    gem_board.state[i].y_animation_offset += iii * ((i + 16) / gem_board.columns) / 8 * diff; 
                    */

                    gem_board.state[i].y_animation_offset += iii * diff;
					if(gem_board.state[i].y_animation_offset > belowc[i % gem_board.columns] * MATCH_GEM_KIND_CLIP_RECT_HEIGHT)
					{
						gem_board.state[i].y_animation_offset = belowc[i % gem_board.columns] * MATCH_GEM_KIND_CLIP_RECT_HEIGHT;
						gem_board.state[i].flags = 2;
                        object_animation_count++;
                        printf("%d\n", object_animation_count);
					}
				}
				if(match_gem_state_is_null(&gem_board.state[i]))
				{
					belowc[i % gem_board.columns]--;
				}
			}
		}
		x = (x + 1) % (1024 - texture.width);
		SDL_SetRenderDrawColor(match_renderer, hex_to_rgba(0x22222222));
		SDL_RenderClear(match_renderer);
		
		SDL_Rect source_rect = {0, 0, texture.width, texture.height};
		SDL_Rect destination_rect = {x, y, texture.width, texture.height};
		SDL_RenderCopy(match_renderer, texture.raw, &source_rect, &destination_rect);

		SDL_SetRenderTarget(match_renderer, gem_board_texture_buffer.raw);
			SDL_SetRenderDrawColor(match_renderer, hex_to_rgba(0x00000000));
			SDL_RenderClear(match_renderer);
			match_renderer_copy_gem_board(0, -MATCH_GEM_KIND_CLIP_RECT_HEIGHT * (gem_board.rows - gem_board.visible_rows) + match_gem_board_top_gap);        
		SDL_SetRenderTarget(match_renderer, NULL);
		
        
		source_rect = (SDL_Rect){0, 0, gem_board_texture_buffer.width, gem_board_texture_buffer.height};
		destination_rect = (SDL_Rect){0, 0, gem_board_texture_buffer.width, gem_board_texture_buffer.height};
		SDL_RenderCopy(match_renderer, gem_board_texture_buffer.raw, &source_rect, &destination_rect);
		
		SDL_RenderPresent(match_renderer);

		end = SDL_GetTicks();
		diff = end - start;
		start = end;
	}
	
	return EXIT_SUCCESS;
}

int match_init(void)
{
    srand(time(NULL));

	ASSERT_NO_ERROR(
	   SDL_InitSubSystem(SDL_INIT_VIDEO), SDL_GetError());

    match_keyboard_init();

	int img_flags = IMG_INIT_PNG;
	int img_flags_set = IMG_Init(img_flags);
	ASSERT(img_flags == img_flags_set, IMG_GetError());
	
	match_window = SDL_CreateWindow(
		"Hello, SDL2!", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		1024, 512 + 512, 
		0
	);

	ASSERT(match_window, SDL_GetError());

	match_renderer = SDL_CreateRenderer(
		match_window, 
		-1, 
		SDL_RENDERER_ACCELERATED 
		| SDL_RENDERER_PRESENTVSYNC 
		| SDL_RENDERER_TARGETTEXTURE
	);

	ASSERT(match_renderer, SDL_GetError());

    #ifdef DEBUG
    SDL_version sdl_compiled;
    SDL_version sdl_linked;
    SDL_version img_compiled;

    SDL_VERSION(&sdl_compiled);
    SDL_GetVersion(&sdl_linked);
    SDL_IMAGE_VERSION(&img_compiled);

    printf("We compiled against SDL version %d.%d.%d ...\n",
           sdl_compiled.major, sdl_compiled.minor, sdl_compiled.patch);
    printf("But we are linking against SDL version %d.%d.%d.\n",
           sdl_linked.major, sdl_linked.minor, sdl_linked.patch);
    printf("We compiled against SDL_image version %d.%d.%d ...\n",
           img_compiled.major, img_compiled.minor, img_compiled.patch);
    #endif

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