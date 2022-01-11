#ifndef MATCH_GEM_HEADER_202112270607
#define MATCH_GEM_HEADER_202112270607

#include "match_definitions.h"

#include "SDL_rect.h"
#include "SDL_render.h"

#include <memory.h>

struct MatchGemState;
struct MatchGemBoard;

extern struct MatchGemBoard gem_board;

int match_gem_state_is_null(struct MatchGemState *gem_state);
void match_gem_board_init(struct MatchGemBoard *gem_board, int rows, int columns, int visible_rows);

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
	MATCH_GEM_KIND_COUNT,
	MATCH_GEM_KIND_SKULL_RED,
	MATCH_GEM_KIND_WILD,
};

struct MatchGemState
{
	int kind;
	int flags;
	double x_animation_offset;
	double y_animation_offset;
};

struct MatchGemBoard
{
	struct MatchGemState *state;
	int columns;
	int rows;
	int visible_rows;
};

#endif