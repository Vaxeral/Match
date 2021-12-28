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

void match_texture_load(struct MatchTexture *texture, SDL_Renderer *renderer, const char *path)
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
	struct MatchGemState *current_gem = gem_board->state;
	for(int j = 0; j < gem_board->rows; j++)
	{
		for(int i = 0; i < gem_board->columns; i++)
		{
			if(match_gem_state_is_defined(current_gem)) {
				int x = i * MATCH_GEM_KIND_CLIP_RECT_WIDTH + current_gem->x_animation_offset + gem_board_x_offset;
				int y = j * MATCH_GEM_KIND_CLIP_RECT_HEIGHT + current_gem->y_animation_offset + gem_board_y_offset;
				match_renderer_copy_gem(renderer, current_gem->kind, gem_texture_atlas, x, y);
			}
			current_gem++;
		}
	}
}

	// struct Gem *current = &board->gems[0];
	// SDL_SetRenderTarget(match_renderer, visible_gems);
	// SDL_RenderClear(match_renderer);
	// SDL_Rect destination = (SDL_Rect){x, y, 0, 0};
	// SDL_QueryTexture(visible_gems, NULL, NULL, &destination.w, &destination.h);
	// int visible_offset = (board->rows - board->visible_rows) * GEM_CLIP_RECT_HEIGHT - (destination.h - board->visible_rows * GEM_CLIP_RECT_HEIGHT);
	// visible_offset *= -1;
	// for(int j = 0; j < board->rows; j++)
	// {
	// 	for(int i = 0; i < board->columns; i++)
	// 	{
	// 		if(match_gem_is_not_null(current)) {
	// 			int x = i * GEM_CLIP_RECT_WIDTH;
	// 			int y = j * GEM_CLIP_RECT_HEIGHT + current->y_offset + visible_offset;
	// 			match_renderer_draw_gem(current, x, y);
	// 		}
	// 		current++;
	// 	}
	// }
	// SDL_SetRenderTarget(match_renderer, NULL);
	// SDL_RenderCopy(match_renderer, visible_gems, NULL, &destination);