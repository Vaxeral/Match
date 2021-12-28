#include "match_gem.h"

int match_gem_state_is_defined(struct MatchGemState *gem_state)
{
	return gem_state->kind && gem_state->flags;
}

void match_gem_board_init(struct MatchGemBoard *gem_board, int rows, int columns, int visible_rows)
{
	gem_board->state = calloc(rows * columns, sizeof(*gem_board->state));
	gem_board->rows = rows;
	gem_board->columns = columns;
	gem_board->visible_rows = visible_rows;
}