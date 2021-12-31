#include "match_renderer.h"

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
	SDL_Renderer *renderer, 
	const char *path)
{
	texture->raw = IMG_LoadTexture(renderer, path);
	ASSERT(texture->raw, IMG_GetError());
	SDL_QueryTexture(texture->raw, NULL, NULL, &texture->width, &texture->height);
}

void match_texture_init(
	struct MatchTexture *texture, 
	SDL_Renderer *renderer, 
	Uint32 format, 
	int access, 
	int width, int height)
{
	texture->raw = SDL_CreateTexture(
		renderer,
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
	SDL_Renderer *renderer, 
	int gem_kind, 
	struct MatchTexture *gem_texture_atlas, 
	int x, int y)
{
	SDL_Rect source_rect = match_gem_kind_clip_rect(gem_kind);
	SDL_Rect destination_rect = {x, y, MATCH_GEM_KIND_CLIP_RECT_WIDTH, MATCH_GEM_KIND_CLIP_RECT_HEIGHT};
	SDL_RenderCopy(renderer, gem_texture_atlas->raw, &source_rect, &destination_rect);
}

void match_renderer_copy_gem_board(
	SDL_Renderer *renderer, 
	struct MatchGemBoard *gem_board, 
	struct MatchTexture *gem_texture_atlas, 
	int gem_board_x_offset, int gem_board_y_offset)
{
	int j, i;
	struct MatchGemState *current_gem = gem_board->state;
	for(j = 0; j < gem_board->rows; j++)
	{
		for(i = 0; i < gem_board->columns; i++)
		{
			if(match_gem_state_not_null(current_gem)) {
				int x = i 
				* MATCH_GEM_KIND_CLIP_RECT_WIDTH 
				+ current_gem->x_animation_offset 
				+ gem_board_x_offset;
				int y = j 
				* MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
				+ current_gem->y_animation_offset 
				+ gem_board_y_offset;
				match_renderer_copy_gem(
					renderer, 
					current_gem->kind, 
					gem_texture_atlas, 
					x, y);
			}
			current_gem++;
		}
	}
}