#include <stdio.h>
#include <stdlib.h>

void PrintArray(int *arr, int size, int width)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(!(i % width) && i)
        {
            printf("\n");
        }
        if(arr[i] < 10)
            printf(" ");
        printf("%d, ", arr[i]);
    }
    printf("\n");
}

void PrintPairs(int *arr, int size, int width)
{
    int *arrLast = NULL;
    int *baseRow = arr;
    int height = size / width;
    int cWidth = width;
    while(cWidth--)
    {
        int i;
        for(i = 0; i < height; i++)
        {
            if(arrLast)
            {
                printf("%d, %d\n", *arrLast, *arr);
            }
            arrLast = arr;
            arr += width;
        }
        arr = ++baseRow;
    }
}

#define WIDTH 8
#define SIZE (WIDTH * WIDTH)
int Array[SIZE];

#include <memory.h>

#include <stdlib.h>
#include <time.h>
int main()
{
    srand(time(NULL));
    int i;
    for(i = 0; i < SIZE; i++)
        Array[i] = rand() % 4 + 1;
    PrintArray(Array, SIZE, WIDTH);
    board_matches_remove(Array, WIDTH, WIDTH);
    printf("\n");
    PrintArray(Array, SIZE, WIDTH);

    return 0;
}








#define match_gem_board_matches_vertical_mark                                                   \
streak = 1;                                                                                     \
                                                                                                \
for(i = 0; i < columns; i++)                                                                    \
{                                                                                               \
    for(j = 0; j < rows - 1; j++)                                                               \
    {                                                                                           \
        previous = state + i + j * columns;                                                     \
        current = state + i + (j + 1) * columns;                                                \
        if(previous->kind == current->kind)                                                     \
        {                                                                                       \
            streak++;                                                                           \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            if(streak >= streak_max)                                                            \
            {                                                                                   \
                int offset;                                                                     \
                for(offset = 0; offset < streak; offset++)                                      \
                    matches_vertical[matches_vertical_count++] = previous - offset * columns;   \
            }                                                                                   \
            streak = 1;                                                                         \
        }                                                                                       \
    }                                                                                           \
                                                                                                \
    if(streak >= streak_max)                                                                    \
    {                                                                                           \
        int offset;                                                                             \
        for(offset = 0; offset < streak; offset++)                                              \
            matches_vertical[matches_vertical_count++] = current - offset * columns;            \
    }                                                                                           \
    streak = 1;                                                                                 \
}                                                                       

#define match_gem_board_matches_horizontal_mark                                         \
streak = 1;                                                                             \
                                                                                        \
for(j = 0; j < rows; j++)                                                               \
{                                                                                       \
    for(i = 0; i < columns - 1; i++)                                                    \
    {                                                                                   \
        previous = state + i + j * columns;                                             \
        current = state + (i + 1) + j * columns;                                        \
        if(previous->kind == current->kind)                                             \
        {                                                                               \
            streak++;                                                                   \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            if(streak >= streak_max)                                                    \
            {                                                                           \
                int offset;                                                             \
                for(offset = 0; offset < streak; offset++)                              \
                    matches_horizontal[matches_horizontal_count++] = previous - offset; \
            }                                                                           \
            streak = 1;                                                                 \
        }                                                                               \
    }                                                                                   \
    if(streak >= streak_max)                                                            \
    {                                                                                   \
        int offset;                                                                     \
        for(offset = 0; offset < streak; offset++)                                      \
            matches_horizontal[matches_horizontal_count++] = current - offset;          \
    }                                                                                   \
    streak = 1;                                                                         \
}                                                               

void match_gem_board_matches_remove(struct MatchGemBoard *board, int rows, int columns)
{
    int size = rows * columns;
    struct MatchGemState *matches_horizontal[size];
    struct MatchGemState *matches_vertical[size];
    int matches_horizontal_count = 0;
    int matches_vertical_count = 0;

    int streak_max = 3;

    struct MatchGemState *state = board->state;

    struct MatchGemState *previous;
    struct MatchGemState *current;
    int i, j, streak;

    match_gem_board_matches_vertical_mark
    match_gem_board_matches_horizontal_mark

    for(i = 0; i < matches_vertical_count; i++)
    {
        matches_vertical[i]->kind = 0;
        matches_vertical[i]->flags = 0;
    }
    for(i = 0; i < matches_horizontal_count; i++)
    {
        matches_horizontal[i]->kind = 0;
        matches_horizontal[i]->flags = 0;
    }
}      






#define match_gem_board_matches_vertical_mark                                               \
streak = 1;                                                                                 \
last = board->state;                                                                        \
current = last + columns;                                                                   \
                                                                                            \
for(i = 0; i < columns; i++)                                                                \
{                                                                                           \
    for(j = 0; j < rows - 1; j++)                                                               \
    {                                                                                       \
        if(last->kind == current->kind)                                                     \
        {                                                                                   \
            streak++;                                                                       \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            if(streak >= streak_max)                                                        \
            {                                                                               \
                int offset;                                                                 \
                for(offset = 0; offset < streak; offset++)                                  \
                    matches_vertical[matches_vertical_count++] = last - offset * columns;   \
            }                                                                               \
            streak = 1;                                                                     \
        }                                                                                   \
        last = current;                                                                     \
        current += columns;                                                                 \
    }                                                                                       \
                                                                                            \
    if(streak >= streak_max)                                                                \
    {                                                                                       \
        int offset;                                                                         \
        for(offset = 0; offset < streak; offset++)                                          \
            matches_vertical[matches_vertical_count++] = current - offset * columns;           \
    }                                                                                       \
    last = ++column_offset;\
    current = last + columns;                                                               \
    streak = 1;                                                                             \
}                                                                       

#define match_gem_board_matches_horizontal_mark                                     \
streak = 1;                                                                         \
last = board->state;                                                                \
current = last + 1;                                                                 \
                                                                                    \
for(j = 0; j < rows; j++)                                                           \
{                                                                                   \
    for(i = 0; i < columns - 1; i++)                                                    \
    {                                                                               \
        if(last->kind == current->kind)                                             \
        {                                                                           \
            streak++;                                                               \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            if(streak >= streak_max)                                                \
            {                                                                       \
                int offset;                                                         \
                for(offset = 0; offset < streak; offset++)                          \
                    matches_horizontal[matches_horizontal_count++] = last - offset; \
            }                                                                       \
            streak = 1;                                                             \
        }                                                                           \
        last = current++;                                                           \
    }                                                                               \
    if(streak >= streak_max)                                                        \
    {                                                                               \
        int offset;                                                                 \
        for(offset = 0; offset < streak; offset++)                                  \
            matches_horizontal[matches_horizontal_count++] = current - offset;         \
    }                                                                               \
    streak = 1;                                                                     \
    last = board->state + j * columns;\
    current = last + 1;\
}                                                               

void match_gem_board_matches_remove(struct MatchGemBoard *board, int rows, int columns)
{
    int size = rows * columns;
    struct MatchGemState *matches_horizontal[size];
    struct MatchGemState *matches_vertical[size];
    int matches_horizontal_count = 0;
    int matches_vertical_count = 0;


    int streak_max = 3;

    struct MatchGemState *column_offset = board->state;

    struct MatchGemState *last;
    struct MatchGemState *current;
    int i, j, streak;

    match_gem_board_matches_vertical_mark
    match_gem_board_matches_horizontal_mark

    for(i = 0; i < matches_horizontal_count; i++)
    {
        matches_horizontal[i]->kind = 0;
        matches_horizontal[i]->flags = 0;
    }
    for(i = 0; i < matches_vertical_count; i++)
    {
        matches_vertical[i]->kind = 0;
        matches_vertical[i]->flags = 0;
    }
}