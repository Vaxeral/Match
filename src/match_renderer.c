#include "match_renderer.h"

void match_texture_load(struct MatchTexture *texture, SDL_Renderer *renderer, const char *path)
{
	texture->raw = IMG_LoadTexture(renderer, path);
	ASSERT(texture->raw, IMG_GetError());
	SDL_QueryTexture(texture->raw, NULL, NULL, &texture->width, &texture->height);
}

void match_renderer_copy_gem_state(SDL_Renderer *renderer, struct MatchGemState *gem_state, struct MatchTexture *gem_texture_atlas)
{

}

void match_renderer_copy_gem_board(SDL_Renderer *renderer, struct MatchGemBoard *gem_board, struct MatchTexture *gem_texture_atlas)
{

}