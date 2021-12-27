#include "match_gem.h"

SDL_Rect match_gem_kind_clip_rect(int gem_kind)
{
	return (SDL_Rect){
		0,
		(gem_kind - 1) * MATCH_GEM_KIND_CLIP_RECT_WIDTH,
		MATCH_GEM_KIND_CLIP_RECT_WIDTH,
		MATCH_GEM_KIND_CLIP_RECT_HEIGHT
	};
}

