#ifndef MATCH_RENDERER_HEADER_202112271209
#define MATCH_RENDERER_HEADER_202112271209

#include "match_gem.h"

#include "SDL_types.h"
#include "SDL_render.h"
#include "SDL_rect.h"
#include "SDL_image.h"

struct MatchTexture;

extern SDL_Renderer *match_renderer;
extern SDL_Window *match_window;
extern struct MatchTexture gem_texture_atlas;

struct MatchTexture
{
	SDL_Texture *raw;
	int width;
	int height;
};

SDL_Rect match_gem_kind_clip_rect(int gem_kind);

void match_texture_load(struct MatchTexture *texture, const char *path);
void match_texture_init(struct MatchTexture *texture, Uint32 format, int access, int width, int height);

void match_renderer_copy_gem(int gem_kind, int x, int y);
void match_renderer_copy_gem_board(int gem_board_x_offset, int gem_board_y_offset);

#endif