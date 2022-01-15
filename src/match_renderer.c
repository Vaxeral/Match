#include "match_renderer.h"

SDL_Renderer *match_renderer;
SDL_Window *match_window;
struct MatchTexture gem_texture_atlas;

SDL_Rect match_gem_kind_clip_rect(int gem_kind)
{
	return (SDL_Rect){
		0,
		(gem_kind - 1) * MATCH_GEM_KIND_CLIP_RECT_WIDTH,
		MATCH_GEM_KIND_CLIP_RECT_WIDTH,
		MATCH_GEM_KIND_CLIP_RECT_HEIGHT
	};
}

void match_texture_load(
	struct MatchTexture *texture,
	const char *path)
{
	texture->raw = IMG_LoadTexture(match_renderer, path);
	ASSERT(texture->raw, IMG_GetError());
	SDL_QueryTexture(texture->raw, NULL, NULL, &texture->width, &texture->height);
}

void match_texture_init(
	struct MatchTexture *texture, 
	Uint32 format, 
	int access, 
	int width, int height)
{
	texture->raw = SDL_CreateTexture(
		match_renderer,
		format,
		access,
		width,
		height
	);
	ASSERT(texture, SDL_GetError());
	texture->width = width;
	texture->height = height;
}

void match_renderer_copy_gem(
	int gem_kind,  
	int x, int y)
{
	SDL_Rect source_rect = match_gem_kind_clip_rect(gem_kind);
	SDL_Rect destination_rect = {x, y, MATCH_GEM_KIND_CLIP_RECT_WIDTH, MATCH_GEM_KIND_CLIP_RECT_HEIGHT};
	SDL_RenderCopy(match_renderer, gem_texture_atlas.raw, &source_rect, &destination_rect);
}

void match_renderer_copy_gem_board(
	int gem_board_x_offset, int gem_board_y_offset)
{
	int j, i;
	struct MatchGemState *current_gem = gem_board.state;
	for(j = 0; j < gem_board.rows; j++)
	{
		for(i = 0; i < gem_board.columns; i++)
		{
			SDL_SetRenderDrawColor(match_renderer, 255, 255, 255, 0);
			if(!match_gem_state_is_null(current_gem)) {
				int x = i 
				* MATCH_GEM_KIND_CLIP_RECT_WIDTH 
				+ current_gem->x_animation_offset 
				+ gem_board_x_offset;
				int y = j 
				* MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
				+ current_gem->y_animation_offset 
				+ gem_board_y_offset;
				match_renderer_copy_gem(current_gem->kind, x, y);
			}
			SDL_SetRenderDrawColor(match_renderer, 0, 255, 0, 100);
			if(current_gem->flags == 6)
			{
				int x = i 
				* MATCH_GEM_KIND_CLIP_RECT_WIDTH 
				+ current_gem->x_animation_offset 
				+ gem_board_x_offset;
				int y = j 
				* MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
				+ current_gem->y_animation_offset 
				+ gem_board_y_offset;
				SDL_Rect rect;
				rect.x = x;
				rect.y = y;
				rect.w = MATCH_GEM_KIND_CLIP_RECT_WIDTH;
				rect.h = MATCH_GEM_KIND_CLIP_RECT_HEIGHT;
				SDL_RenderFillRect(match_renderer, &rect);
			}
			current_gem++;
		}
	}
}