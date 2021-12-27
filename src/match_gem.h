#ifndef MATCH_GEM_HEADER_202112270607
#define MATCH_GEM_HEADER_202112270607

#include "match_definitions.h"

#include <SDL_rect.h>
#include <SDL_render.h>

struct MatchGemState;
struct MatchGemBoard;

SDL_Rect match_gem_kind_clip_rect(int gem_kind);

enum MatchGemKind
{
	MATCH_GEM_KIND_NULL,
	MATCH_GEM_KIND_MANA_BLUE,
	MATCH_GEM_KIND_MANA_RED,
	MATCH_GEM_KIND_MANA_GREEN,
	MATCH_GEM_KIND_MANA_YELLOW,
	MATCH_GEM_KIND_COINS,
	MATCH_GEM_KIND_EXPERIENCE,
	MATCH_GEM_KIND_SKULL,
	MATCH_GEM_KIND_SKULL_RED,
	MATCH_GEM_KIND_WILD,
	MATCH_GEM_KIND_COUNT
};

struct MatchGemState
{
	int kind;
	int flags;
};

struct MatchGemBoard
{
	struct MatchGemState *state;
	int columns;
	int rows;
};

#endif