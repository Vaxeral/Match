#include "match.h"

#include "SDL.h"
#include "SDL_image.h"

#include <time.h>
#include <stdlib.h>
// push animations into a queue.
// code board logic and verify operations.

// figure out when animations are done.  animation queue.
int match_init(void);
int match_events_handle(void);

#include <memory.h>

enum MatchGemBoardEdge
{
    MATCH_GEM_BOARD_EDGE_NONE,
    MATCH_GEM_BOARD_EDGE_CENTER,
    MATCH_GEM_BOARD_EDGE_LEFT,
    MATCH_GEM_BOARD_EDGE_RIGHT,
    MATCH_GEM_BOARD_EDGE_TOP,
    MATCH_GEM_BOARD_EDGE_BOTTOM,
    MATCH_GEM_BOARD_EDGE_TOP_LEFT,
    MATCH_GEM_BOARD_EDGE_TOP_RIGHT,
    MATCH_GEM_BOARD_EDGE_BOTTOM_LEFT,
    MATCH_GEM_BOARD_EDGE_BOTTOM_RIGHT 
};

enum MatchStreakConfig
{
    MATCH_STREAK_CONFIG_NONE,
    MATCH_STREAK_CONFIG_START,
    MATCH_STREAK_CONFIG_MIDDLE,
    MATCH_STREAK_CONFIG_END
};

enum MatchStreakOrientation
{
    MATCH_STREAK_ORIENTATION_NONE,
    MATCH_STREAK_ORIENTATION_HORIZONTAL,
    MATCH_STREAK_ORIENTATION_VERTICAL
};

struct MatchGemPattern
{
    int kind;
    int config;
};

struct MatchGemSwap
{
    struct MatchGemState *gem;
    struct MatchGemPattern pattern;
    int direction;
    int edge;
    int streak_potential;
    int streak_orientation;
};

enum MatchGemSwapDirection
{
    MATCH_GEM_SWAP_DIRECTION_NONE,
    MATCH_GEM_SWAP_DIRECTION_LEFT = 1 << 0,
    MATCH_GEM_SWAP_DIRECTION_RIGHT = 1 << 1,
    MATCH_GEM_SWAP_DIRECTION_TOP = 1 << 2,
    MATCH_GEM_SWAP_DIRECTION_BOTTOM = 1 << 3    
};

struct MatchGemSwapArray
{
    struct MatchGemSwap *data;
    int size;
    int capacity;
};

void match_gem_swap_array_init(struct MatchGemSwapArray *array)
{
    array->capacity = 2;
    array->data = calloc(array->capacity, sizeof(struct MatchGemSwap));
    array->size = 0;
}

void match_gem_swap_array_push(struct MatchGemSwapArray *array, struct MatchGemSwap value)
{
    if(array->size >= array->capacity)
    {
        array->capacity *= 2;
        void *result = realloc(array->data, sizeof(struct MatchGemSwap) * array->capacity);
        ASSERT(result, "Could not allocate for MatchGemSwapArray.");
        array->data = result;
    }
    array->data[array->size++] = value;
}

void match_gem_swap_array_free(struct MatchGemSwapArray *array)
{
    free(array->data);
}
    /*
        if(no streak)
            skip all

        if(prev != none)
            skip stack push

        if(current != (abb || bba))
            streak = last / 2 + current

            !only if kind is the same
            if(prev != (abb || bba))
                skip stack push and new candidate
                candidate += 1
        else
            streak = current
            
            !only if kind is the same
            if(prev != (abb || bba))
               candidate += 1
    */
/*
    streak: majority in window and majority >= threshold - 1 is streak to test for.
    1234567
    abbabba

    abb candidate <- { position: 1, streak: current(2), kind: b }
    bba stack <- candidate, candidate <- { position: 4, streak: current(2), kind: b }
    bab stack <- candidate, candidate <- { position: 4, streak: last / 2 + current (3), kind: b }
    abb candidate += 1, stack <- candidate, candidate <- { position: 4, streak: current(2), kind b } 
    bba stack <- candidate, candidate <- { position: 7, streak: current(2), kind: b }
    end stack <- candidate

    TODO: need to test streaks greater than 3.
    
    if(no streak)
        skip all

    if(prev != none)
        skip stack push

    if(current != (abb || bba))
        streak = last / 2 + current

        !only if kind is the same
        if(prev != (abb || bba))
            skip stack push and new candidate
            candidate += 1
    else
        streak = current
        
        !only if kind is the same
        if(prev != (abb || bba))
           candidate += 1

    



    abb candidate <- { position: 1, streak: (2), kind: b }
    bba stack <- candidate, candidate <- { position: 4, streak: (2), kind: b }

    bab stack <- candidate, candidate <- { position: 4, streak: (last / 2 + current), kind: b }
    abb stack <- candidate, candidate <- { position: 4, streak: current, kind: b }
    bba
     
    1234567
    abababa
    












    baaa
    aaab
    aaba


    babb
*/

#define match_gem_board_window_histogram(stride)\
for(k = 0; k < MATCH_GEM_KIND_COUNT; k++)       \
    histogram[k] = 0;                           \
for(k = 0; k < streak_threshold; k++)           \
    histogram[(window + (k * stride))->kind]++;  

#define match_gem_board_window_pattern(stride)                                                              \
majority = 0;                                                                                               \
highest = 0;                                                                                                \
for(k = 0; k < MATCH_GEM_KIND_COUNT; k++)                                                                   \
{                                                                                                           \
    if(histogram[k] > highest)                                                                              \
    {                                                                                                       \
        highest = histogram[k];                                                                             \
        majority = k;                                                                                       \
    }                                                                                                       \
}                                                                                                           \
if(highest >= streak_threshold - 1)                                                                         \
{                                                                                                           \
    pattern_current.kind = majority;                                                                        \
    if(window[0].kind == window[(streak_threshold - 1) * stride].kind)                                      \
    {                                                                                                       \
        pattern_current.config = MATCH_STREAK_CONFIG_MIDDLE;                                                \
    }                                                                                                       \
    else if(window[(streak_threshold - 2) * stride].kind != window[(streak_threshold - 1) * stride].kind)   \
    {                                                                                                       \
        pattern_current.config = MATCH_STREAK_CONFIG_END;                                                   \
    }                                                                                                       \
    else if(window[0].kind != window[1 * stride].kind)                                                      \
    {                                                                                                       \
        pattern_current.config = MATCH_STREAK_CONFIG_START;                                                 \
    }                                                                                                       \
}                                                                                                           \
else                                                                                                        \
{                                                                                                           \
    pattern_current.config = MATCH_STREAK_CONFIG_NONE;                                                      \
    pattern_current.kind = 0;                                                                               \
}

void match_gem_board_matches_has_possible(struct MatchGemBoard *board)
{
    int const columns = board->columns;
    int const rows = board->rows;
    int const size = rows * columns;
    int const streak_threshold = 3;

    struct MatchGemState *state = board->state;

    int i, j, k;
    int majority, highest;
    int highest_previous;
    int push_skip;
    int subject_skip;
    int subject_streak;

    struct MatchGemSwap subject;
    struct MatchGemSwapArray candidates;

    match_gem_swap_array_init(&candidates);

    struct MatchGemState *window;

    int histogram[MATCH_GEM_KIND_COUNT];

    struct MatchGemPattern pattern_previous;
    struct MatchGemPattern pattern_current;

    for(j = 0; j < rows; j++)
    {
        window = state + 0 + j * columns;
        match_gem_board_window_histogram(1)

        subject = (struct MatchGemSwap){0};
        pattern_previous = (struct MatchGemPattern){0};
        highest_previous = 0;
        subject_streak = 0;

        for(i = 0; i < columns - (streak_threshold - 1); i++)
        {
            window = state + i + j * columns;
            if(i != 0)
            {
                histogram[(window + streak_threshold - 1)->kind]++;
            }

            match_gem_board_window_pattern(1)

            push_skip = 0;
            subject_skip = 0;

            if(pattern_current.config == MATCH_STREAK_CONFIG_NONE)
            {
                subject_skip = 1;
            }

            if(pattern_previous.config == MATCH_STREAK_CONFIG_NONE)
            {
                push_skip = 1;
            }

            if(pattern_current.config == MATCH_STREAK_CONFIG_MIDDLE)
            {
                subject_streak = highest_previous / 2 + highest;

                if(
                    pattern_current.kind == pattern_previous.kind
                    && pattern_previous.config == MATCH_STREAK_CONFIG_MIDDLE)
                {

                    subject.streak_potential += 1;

                    subject_skip = 1;
                    push_skip = 1;
                }
            }
            else
            {
                subject_streak = highest;

                if(
                    pattern_current.kind == pattern_previous.kind
                    && pattern_previous.config == MATCH_STREAK_CONFIG_MIDDLE)
                {
                    subject.streak_potential += 1;
                }
            }   

            if(!push_skip)
            {
                match_gem_swap_array_push(&candidates, subject);
            }
            if(!subject_skip)
            {
                struct MatchGemState *gem = NULL;
                if(pattern_current.config == MATCH_STREAK_CONFIG_START)
                {
                    gem = window;
                }
                else if(pattern_current.config == MATCH_STREAK_CONFIG_MIDDLE)
                {
                    gem = window + (streak_threshold - 2);
                }
                else if(pattern_current.config == MATCH_STREAK_CONFIG_END)
                {
                    gem = window + (streak_threshold - 1);
                }

                int edge = MATCH_GEM_BOARD_EDGE_NONE;
                int index = gem - state;
                int left = 0, right = 0, top = 0, bottom = 0;

                if(index % columns == 0)
                    left = 1;
                if(index % columns == columns - 1)
                    right = 1;
                if(index / columns == 0)
                    top = 1;
                if(index / columns == rows - 1)
                    bottom = 1;

                if(top && left)
                    edge = MATCH_GEM_BOARD_EDGE_TOP_LEFT;
                else if(top && right)
                    edge = MATCH_GEM_BOARD_EDGE_TOP_RIGHT;
                else if(bottom && left)
                    edge = MATCH_GEM_BOARD_EDGE_BOTTOM_LEFT;
                else if(bottom && right)
                    edge = MATCH_GEM_BOARD_EDGE_BOTTOM_RIGHT;
                else if(top)
                    edge = MATCH_GEM_BOARD_EDGE_TOP;
                else if(bottom)
                    edge = MATCH_GEM_BOARD_EDGE_BOTTOM;
                else if(left)
                    edge = MATCH_GEM_BOARD_EDGE_LEFT;
                else if(right)
                    edge = MATCH_GEM_BOARD_EDGE_RIGHT;
                else
                    edge = MATCH_GEM_BOARD_EDGE_CENTER;

                subject = (struct MatchGemSwap){
                    gem,
                    pattern_current,
                    MATCH_GEM_SWAP_DIRECTION_NONE, // directions to swap.  not know at this stage.
                    edge,
                    subject_streak + 1,
                    MATCH_STREAK_ORIENTATION_HORIZONTAL
                };  
            }

            highest_previous = highest;
            histogram[window->kind]--;
            pattern_previous = pattern_current;
        }
        if(pattern_current.config != MATCH_STREAK_CONFIG_NONE)
        {
            match_gem_swap_array_push(&candidates, subject);
        }
    }

    for(i = 0; i < columns; i++)
    {
        window = state + i + 0 * columns;
        match_gem_board_window_histogram(columns)

        subject = (struct MatchGemSwap){0};
        pattern_previous = (struct MatchGemPattern){0};
        highest_previous = 0;
        subject_streak = 0;

        for(j = 0; j < rows - (streak_threshold - 1); j++)
        {
            window = state + i + j * columns;
            if(j != 0)
            {
                histogram[(window + (streak_threshold - 1) * columns)->kind]++;
            }

            match_gem_board_window_pattern(columns)

            push_skip = 0;
            subject_skip = 0;

            if(pattern_current.config == MATCH_STREAK_CONFIG_NONE)
            {
                subject_skip = 1;
            }

            if(pattern_previous.config == MATCH_STREAK_CONFIG_NONE)
            {
                push_skip = 1;
            }

            if(pattern_current.config == MATCH_STREAK_CONFIG_MIDDLE)
            {
                subject_streak = highest_previous / 2 + highest;

                if(
                    pattern_current.kind == pattern_previous.kind
                    && pattern_previous.config == MATCH_STREAK_CONFIG_MIDDLE)
                {

                    subject.streak_potential += 1;

                    subject_skip = 1;
                    push_skip = 1;
                }
            }
            else
            {
                subject_streak = highest;

                if(
                    pattern_current.kind == pattern_previous.kind
                    && pattern_previous.config == MATCH_STREAK_CONFIG_MIDDLE)
                {
                    subject.streak_potential += 1;
                }
            }   

            if(!push_skip)
            {
                match_gem_swap_array_push(&candidates, subject);
            }
            if(!subject_skip)
            {
                struct MatchGemState *gem = NULL;
                if(pattern_current.config == MATCH_STREAK_CONFIG_START)
                {
                    gem = window;
                }
                else if(pattern_current.config == MATCH_STREAK_CONFIG_MIDDLE)
                {
                    gem = window + (streak_threshold - 2) * columns;
                }
                else if(pattern_current.config == MATCH_STREAK_CONFIG_END)
                {
                    gem = window + (streak_threshold - 1) * columns;
                }

                int edge = MATCH_GEM_BOARD_EDGE_NONE;
                int index = gem - state;
                int left = 0, right = 0, top = 0, bottom = 0;

                if(index % columns == 0)
                    left = 1;
                if(index % columns == columns - 1)
                    right = 1;
                if(index / columns == 0)
                    top = 1;
                if(index / columns == rows - 1)
                    bottom = 1;

                if(top && left)
                    edge = MATCH_GEM_BOARD_EDGE_TOP_LEFT;
                else if(top && right)
                    edge = MATCH_GEM_BOARD_EDGE_TOP_RIGHT;
                else if(bottom && left)
                    edge = MATCH_GEM_BOARD_EDGE_BOTTOM_LEFT;
                else if(bottom && right)
                    edge = MATCH_GEM_BOARD_EDGE_BOTTOM_RIGHT;
                else if(top)
                    edge = MATCH_GEM_BOARD_EDGE_TOP;
                else if(bottom)
                    edge = MATCH_GEM_BOARD_EDGE_BOTTOM;
                else if(left)
                    edge = MATCH_GEM_BOARD_EDGE_LEFT;
                else if(right)
                    edge = MATCH_GEM_BOARD_EDGE_RIGHT;
                else
                    edge = MATCH_GEM_BOARD_EDGE_CENTER;

                subject = (struct MatchGemSwap){
                    gem,
                    pattern_current,
                    MATCH_GEM_SWAP_DIRECTION_NONE, // directions to swap.  not know at this stage.
                    edge,
                    subject_streak + 1,
                    MATCH_STREAK_ORIENTATION_VERTICAL
                };  
            }

            highest_previous = highest;
            histogram[window->kind]--;
            pattern_previous = pattern_current;
        }
        if(pattern_current.config != MATCH_STREAK_CONFIG_NONE)
        {
            match_gem_swap_array_push(&candidates, subject);
        }
    }

    int n;
    for(n = 0; n < candidates.size; n++)
    {
        struct MatchGemSwap *swap = &candidates.data[n];

        struct MatchGemState *left = swap->gem - 1;
        struct MatchGemState *right = swap->gem + 1;
        struct MatchGemState *top = swap->gem - columns;
        struct MatchGemState *bottom = swap->gem + columns;

        struct MatchGemState *a;
        struct MatchGemState *b;
        struct MatchGemState *c;
        struct MatchGemState *d;

        int swap_right = MATCH_GEM_SWAP_DIRECTION_NONE;
        int swap_left = MATCH_GEM_SWAP_DIRECTION_NONE;
        int swap_top = MATCH_GEM_SWAP_DIRECTION_NONE;
        int swap_bottom = MATCH_GEM_SWAP_DIRECTION_NONE;

        // assert edge
        switch(swap->edge)
        {
            case MATCH_GEM_BOARD_EDGE_LEFT:
                left = NULL;
            break;
            case MATCH_GEM_BOARD_EDGE_RIGHT:
                right = NULL;
            break;
            case MATCH_GEM_BOARD_EDGE_TOP:
                top = NULL;
            break;
            case MATCH_GEM_BOARD_EDGE_BOTTOM:
                bottom = NULL;
            break;
            case MATCH_GEM_BOARD_EDGE_TOP_LEFT:
                top = NULL;
                left = NULL;
            break;
            case MATCH_GEM_BOARD_EDGE_TOP_RIGHT:
                top = NULL;
                right = NULL;
            break;
            case MATCH_GEM_BOARD_EDGE_BOTTOM_LEFT:
                bottom = NULL;
                left = NULL;
            break;
            case MATCH_GEM_BOARD_EDGE_BOTTOM_RIGHT:
                bottom = NULL;
                right = NULL;
            break;
            case MATCH_GEM_BOARD_EDGE_CENTER:
                // All swap directions can be checked.
            break;

            default:
                ASSERT(0, "swap edge case not handled.");
            break;
        };

        ASSERT(swap->streak_orientation, "streak orientation was set to zero.");
        int is_horizontal = swap->streak_orientation == MATCH_STREAK_ORIENTATION_HORIZONTAL;
        

        if(is_horizontal)
        {
            a = left;
            b = right;
            c = top;
            d = bottom;
            swap_left = MATCH_GEM_SWAP_DIRECTION_LEFT;
            swap_right = MATCH_GEM_SWAP_DIRECTION_RIGHT;
            swap_top = MATCH_GEM_SWAP_DIRECTION_TOP;
            swap_bottom = MATCH_GEM_SWAP_DIRECTION_BOTTOM;
        }
        else
        {
            a = top;
            b = bottom;
            c = left;
            d = right;
            swap_left = MATCH_GEM_SWAP_DIRECTION_TOP;
            swap_right = MATCH_GEM_SWAP_DIRECTION_BOTTOM;
            swap_top = MATCH_GEM_SWAP_DIRECTION_LEFT;
            swap_bottom = MATCH_GEM_SWAP_DIRECTION_RIGHT;
        } 

        ASSERT(swap->pattern.config, "pattern config was set to zero.");
        if(swap->pattern.config == MATCH_STREAK_CONFIG_START)
        {
            if(a && a->kind == swap->pattern.kind)
                swap->direction |= swap_left;
            if(c && c->kind == swap->pattern.kind)
                swap->direction |= swap_top;
            if(d && d->kind == swap->pattern.kind)
                swap->direction |= swap_bottom;
        }
        else if(swap->pattern.config == MATCH_STREAK_CONFIG_END)
        {
            if(b && b->kind == swap->pattern.kind)
                swap->direction |= swap_right;
            if(c && c->kind == swap->pattern.kind)
                swap->direction |= swap_top;
            if(d && d->kind == swap->pattern.kind)
                swap->direction |= swap_bottom;
        }
        else if(swap->pattern.config == MATCH_STREAK_CONFIG_MIDDLE)
        {
            if(c && c->kind == swap->pattern.kind)
                swap->direction |= swap_top;
            if(d && d->kind == swap->pattern.kind)
                swap->direction |= swap_bottom;
        }
    }

    for(i = 0; i < candidates.size; i++)
    {
        if(
            candidates.data[i].direction != MATCH_GEM_SWAP_DIRECTION_NONE 
            )
        {
            printf(
                "position: %lu, kind: %d, potential: %d, config: %d, orientation: %d, edge: %d\n", 
                candidates.data[i].gem - state, 
                candidates.data[i].pattern.kind, 
                candidates.data[i].streak_potential,
                candidates.data[i].pattern.config,
                candidates.data[i].streak_orientation,
                candidates.data[i].edge);
            candidates.data[i].gem->flags = 6;
        }
    }

    match_gem_swap_array_free(&candidates);
}

void collapse_board(struct MatchGemBoard* board, int rows, int columns)
{
    int empty_index;
    int i, j;

    for(i = 0; i < columns; i++)
    {
        empty_index = -1;
        for(j = rows - 1; j >= 0; j--)
        {
            if(board->state[i + j * columns].kind == 0)
            {
                if(empty_index == -1) empty_index = j;
            }
            else
            {
                if(empty_index != -1)
                {
                    struct MatchGemState tmp = board->state[i + j * columns];
                    board->state[i + j * columns] = (struct MatchGemState){0};
                    board->state[i + empty_index * columns] = tmp;
                    empty_index--;
                }
            }
        }
    }
}


#define match_gem_board_matches_vertical_mark                           \
streak = 1;                                                             \
                                                                        \
for(i = 0; i < columns; i++)                                            \
{                                                                       \
    for(j = 0; j < rows - 1; j++)                                       \
    {                                                                   \
        previous = state + i + j * columns;                             \
        current = state + i + (j + 1) * columns;                        \
        if(previous->kind == current->kind)                             \
        {                                                               \
            streak++;                                                   \
        }                                                               \
        else                                                            \
        {                                                               \
            if(streak >= streak_max)                                    \
            {                                                           \
                int offset;                                             \
                for(offset = 0; offset < streak; offset++)              \
                    matches[(previous - offset * columns) - state] = 1; \
            }                                                           \
            streak = 1;                                                 \
        }                                                               \
    }                                                                   \
                                                                        \
    if(streak >= streak_max)                                            \
    {                                                                   \
        int offset;                                                     \
        for(offset = 0; offset < streak; offset++)                      \
            matches[(current - offset * columns) - state] = 1;          \
    }                                                                   \
    streak = 1;                                                         \
}                                                                       

#define match_gem_board_matches_horizontal_mark                 \
streak = 1;                                                     \
                                                                \
for(j = 0; j < rows; j++)                                       \
{                                                               \
    for(i = 0; i < columns - 1; i++)                            \
    {                                                           \
        previous = state + i + j * columns;                     \
        current = state + (i + 1) + j * columns;                \
        if(previous->kind == current->kind)                     \
        {                                                       \
            streak++;                                           \
        }                                                       \
        else                                                    \
        {                                                       \
            if(streak >= streak_max)                            \
            {                                                   \
                int offset;                                     \
                for(offset = 0; offset < streak; offset++)      \
                    matches[(previous - offset) - state] = 1;   \
            }                                                   \
            streak = 1;                                         \
        }                                                       \
    }                                                           \
    if(streak >= streak_max)                                    \
    {                                                           \
        int offset;                                             \
        for(offset = 0; offset < streak; offset++)              \
            matches[(current - offset) - state] = 1;            \
    }                                                           \
    streak = 1;                                                 \
}                                                               

void match_gem_board_matches_clear(struct MatchGemBoard *board, int *cleared)
{
    int i, j, streak;
    
    struct MatchGemState const *previous;
    struct MatchGemState const *current;

    int const rows = board->rows;
    int const columns = board->columns;
    int const size = board->rows * board->columns;
    int matches[size];
    for(i = 0; i < size; i++)
        matches[i] = 0;

    int const streak_max = 3;

    struct MatchGemState *const state = board->state;

    match_gem_board_matches_vertical_mark
    match_gem_board_matches_horizontal_mark

    for(i = 0; i < columns; i++)
        cleared[i] = 0;

    for(i = 0; i < size; i++)
        if(matches[i])
        {
            state[i] = (struct MatchGemState){0};
            cleared[i % columns]++;
        }
}

// struct MatchGemSwap
// {
//     int a, b;
//     int streak;
// };    

int main(int argc, char const *argv[])
{
	match_init();

	struct MatchTexture texture;
	match_texture_load(&texture, "resources/yellow-wolly-wog.png");
	
	match_texture_load(&gem_texture_atlas, "resources/match-gem-texture-atlas-export.png");

    struct MatchGemBoard gem_board64;
    struct MatchGemBoard gem_board256;

    match_gem_board_init(&gem_board64, 16, 8, 8);
	match_gem_board_init(&gem_board256, 20, 10, 10);

    gem_board = gem_board256;

	int match_gem_board_top_gap = MATCH_GEM_KIND_CLIP_RECT_HEIGHT / 2;

	struct MatchTexture gem_board_texture_buffer;
	match_texture_init(
		&gem_board_texture_buffer,
		SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET, 
		
		MATCH_GEM_KIND_CLIP_RECT_WIDTH 
		* gem_board.columns,

		MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
		* gem_board.visible_rows 
		+ match_gem_board_top_gap);

    SDL_SetRenderDrawBlendMode(match_renderer, SDL_BLENDMODE_BLEND);

	int i;

    int prev;
    int curr = 0;
    int rand_gem;
    int streak = 1;
	for(i = 0; i < gem_board.columns * gem_board.rows; i++)
	{
        if(prev == curr) streak++;
        else streak = 1;
        if(streak >= 2)
        {
            rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
            while(rand_gem == prev)
            {
                rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
            }
        }
        else
        {
            rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
        }
        prev = curr;
        curr = rand_gem;
		gem_board.state[i] = (struct MatchGemState){
            curr,
			1,
			0,
			0
		};
	}
    // struct MatchGemState *last = gem_board.state + (gem_board.rows - 1) * gem_board.columns;

    // last++->kind = MATCH_GEM_KIND_MANA_RED;
    // last++->kind = MATCH_GEM_KIND_MANA_BLUE;
    // last++->kind = MATCH_GEM_KIND_MANA_RED;
    // last++->kind = MATCH_GEM_KIND_MANA_RED;
    // last++->kind = MATCH_GEM_KIND_SKULL;
    // last++->kind = MATCH_GEM_KIND_MANA_YELLOW;
    // last++->kind = MATCH_GEM_KIND_MANA_BLUE;
    // last++->kind = MATCH_GEM_KIND_MANA_RED;
    // last++->kind = MATCH_GEM_KIND_SKULL;
    // last++->kind = MATCH_GEM_KIND_MANA_RED;

    // last = gem_board.state + (gem_board.rows - 2) * gem_board.columns;

    // last++->kind = MATCH_GEM_KIND_SKULL;
    // last++->kind = MATCH_GEM_KIND_COINS;
    // last++->kind = MATCH_GEM_KIND_SKULL;

    // last = gem_board.state + (gem_board.columns - 2) + (gem_board.rows - 3) * gem_board.columns;
    // last++->kind = MATCH_GEM_KIND_COINS;
    // last++->kind = MATCH_GEM_KIND_COINS;



    match_gem_board_matches_has_possible(&gem_board);
	int below[16] = {0};
	int belowc[16] = {0};
    int is_running = 1;
	int x, y;
	x = y = 0;
	double start = time(NULL);
	double end = 0;
	double iii = .25;
	double diff;
	int starts = 0;
    int object_animation_count = 0;
	while(is_running)
	{
		match_keyboard_update();
		is_running = match_events_handle();

		// if(!starts && match_keyboard_just_pressed(SDL_SCANCODE_A))
		// {
  //           for(i = 0; i < 16; i++) below[i] = 0;
		// 	for(i = 0; i < gem_board.rows * gem_board.columns; i++)
		// 	{
		// 		if(match_gem_state_is_null(&gem_board.state[i]))
		// 		{
		// 			below[i % gem_board.columns]++;
		// 		}
		// 	}
		// 	starts = 1;
		// }
        if(match_keyboard_just_pressed(SDL_SCANCODE_T))
            match_gem_board_matches_has_possible(&gem_board);
        if(match_keyboard_just_pressed(SDL_SCANCODE_P))  
        {
            match_gem_board_matches_clear(&gem_board, below);
            starts = 1;
        }      
        if(match_keyboard_just_pressed(SDL_SCANCODE_B))
        {
            iii = .25;
            starts = 0;
            gem_board = gem_board256;
            SDL_DestroyTexture(gem_board_texture_buffer.raw);
                match_texture_init(
        &gem_board_texture_buffer,
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        
        MATCH_GEM_KIND_CLIP_RECT_WIDTH 
        * gem_board.columns,

        MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
        * gem_board.visible_rows 
        + match_gem_board_top_gap);
    for(i = 0; i < gem_board.columns * gem_board.rows; i++)
    {
        if(prev == curr) streak++;
        else streak = 1;
        if(streak >= 2)
        {
            rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
            while(rand_gem == prev)
            {
                rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
            }
        }
        else
        {
            rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
        }
        prev = curr;
        curr = rand_gem;
        gem_board.state[i] = (struct MatchGemState){
            curr,
            1,
            0,
            0
        };
    }
        }
        if(match_keyboard_just_pressed(SDL_SCANCODE_C))
        {
            iii = .25;
            starts = 0;
            gem_board = gem_board64;
            SDL_DestroyTexture(gem_board_texture_buffer.raw);
                match_texture_init(
        &gem_board_texture_buffer,
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        
        MATCH_GEM_KIND_CLIP_RECT_WIDTH 
        * gem_board.columns,

        MATCH_GEM_KIND_CLIP_RECT_HEIGHT 
        * gem_board.visible_rows 
        + match_gem_board_top_gap);
    for(i = 0; i < gem_board.columns * gem_board.rows; i++)
    {
        if(prev == curr) streak++;
        else streak = 1;
        if(streak >= 2)
        {
            rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
            while(rand_gem == prev)
            {
                rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
            }
        }
        else
        {
            rand_gem = (rand() % (MATCH_GEM_KIND_COUNT - 1)) + 1;
        }
        prev = curr;
        curr = rand_gem;
        gem_board.state[i] = (struct MatchGemState){
            curr,
            1,
            0,
            0
        };
    }
        }
        if(object_animation_count == 256)
        {
            printf("Done animating\n");
            object_animation_count = 0;
        }
		if(starts)
		{		
            memcpy(&belowc[0], &below[0], sizeof(int) * 16);
			iii *= 1.03;
			for(i = 0; i < gem_board.rows * gem_board.columns; i++)
			{
				if(gem_board.state[i].flags && gem_board.state[i].flags != 2 && belowc[i % gem_board.columns] > 0)
				{
                    /*
                        Used for staggered drop effect at start.
                    gem_board.state[i].y_animation_offset += iii * ((i + 16) / gem_board.columns) / 8 * diff; 
                    */

                    gem_board.state[i].y_animation_offset += iii * diff;
					if(gem_board.state[i].y_animation_offset > belowc[i % gem_board.columns] * MATCH_GEM_KIND_CLIP_RECT_HEIGHT)
					{
						gem_board.state[i].y_animation_offset = belowc[i % gem_board.columns] * MATCH_GEM_KIND_CLIP_RECT_HEIGHT;
						gem_board.state[i].flags = 2;
                        object_animation_count++;
                        printf("%d\n", object_animation_count);
					}
				}
				if(match_gem_state_is_null(&gem_board.state[i]))
				{
					belowc[i % gem_board.columns]--;
				}
			}
		}
		x = (x + 1) % (1024 - texture.width);
		SDL_SetRenderDrawColor(match_renderer, hex_to_rgba(0x22222222));
		SDL_RenderClear(match_renderer);
		
		SDL_Rect source_rect = {0, 0, texture.width, texture.height};
		SDL_Rect destination_rect = {x, y, texture.width, texture.height};
		SDL_RenderCopy(match_renderer, texture.raw, &source_rect, &destination_rect);

		SDL_SetRenderTarget(match_renderer, gem_board_texture_buffer.raw);
			SDL_SetRenderDrawColor(match_renderer, hex_to_rgba(0x00000000));
			SDL_RenderClear(match_renderer);
			match_renderer_copy_gem_board(0, -MATCH_GEM_KIND_CLIP_RECT_HEIGHT * (gem_board.rows - gem_board.visible_rows) + match_gem_board_top_gap);        
		SDL_SetRenderTarget(match_renderer, NULL);
		
        
		source_rect = (SDL_Rect){0, 0, gem_board_texture_buffer.width, gem_board_texture_buffer.height};
		destination_rect = (SDL_Rect){0, 0, gem_board_texture_buffer.width, gem_board_texture_buffer.height};
		SDL_RenderCopy(match_renderer, gem_board_texture_buffer.raw, &source_rect, &destination_rect);
		
		SDL_RenderPresent(match_renderer);

		end = SDL_GetTicks();
		diff = end - start;
		start = end;
	}
	
	return EXIT_SUCCESS;
}

int match_init(void)
{
    srand(time(NULL));

	ASSERT_NO_ERROR(
	   SDL_InitSubSystem(SDL_INIT_VIDEO), SDL_GetError());

    match_keyboard_init();

	int img_flags = IMG_INIT_PNG;
	int img_flags_set = IMG_Init(img_flags);
	ASSERT(img_flags == img_flags_set, IMG_GetError());
	
	match_window = SDL_CreateWindow(
		"Hello, SDL2!", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		1024, 512 + 512, 
		0
	);

	ASSERT(match_window, SDL_GetError());

	match_renderer = SDL_CreateRenderer(
		match_window, 
		-1, 
		SDL_RENDERER_ACCELERATED 
		| SDL_RENDERER_PRESENTVSYNC 
		| SDL_RENDERER_TARGETTEXTURE
	);

	ASSERT(match_renderer, SDL_GetError());

    #ifdef DEBUG
    SDL_version sdl_compiled;
    SDL_version sdl_linked;
    SDL_version img_compiled;

    SDL_VERSION(&sdl_compiled);
    SDL_GetVersion(&sdl_linked);
    SDL_IMAGE_VERSION(&img_compiled);

    printf("We compiled against SDL version %d.%d.%d ...\n",
           sdl_compiled.major, sdl_compiled.minor, sdl_compiled.patch);
    printf("But we are linking against SDL version %d.%d.%d.\n",
           sdl_linked.major, sdl_linked.minor, sdl_linked.patch);
    printf("We compiled against SDL_image version %d.%d.%d ...\n",
           img_compiled.major, img_compiled.minor, img_compiled.patch);
    #endif

	return EXIT_SUCCESS;
}

int match_events_handle(void)
{
	int state = 1;
	
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			state = 0;
		} 
		else if(event.type == SDL_WINDOWEVENT)
		{
			if(event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				state = 0;
			}
		}
	}
	return state;
}

#include "match_gem.c"
#include "match_keyboard.c"
#include "match_renderer.c"