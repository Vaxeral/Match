#ifndef MATCH_RENDERER_HEADER_202112271209
#define MATCH_RENDERER_HEADER_202112271209

#include "match_gem.h"

#include <SDL_render.h>
#include <SDL_image.h>

struct MatchTexture
{
	SDL_Texture *raw;
	int width;
	int height;
};

void match_texture_load(struct MatchTexture *texture, SDL_Renderer *renderer, const char *path);


void match_renderer_copy_gem_state(SDL_Renderer *renderer, struct MatchGemState *gem_state, struct MatchTexture *gem_texture_atlas);
void match_renderer_copy_gem_board(SDL_Renderer *renderer, struct MatchGemBoard *gem_board, struct MatchTexture *gem_texture_atlas);

#endif