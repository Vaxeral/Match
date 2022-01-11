#include <memory.h>
#include <stdio.h>


/*
    gap stack.  fifo.  modify the last gap on the stack.  gap has streak count and kind.
*/
void TTT_GetHeatMap(const char *grid, short len, char width, char height, char *X, char *O)
{
    memset(X, 0, len);
    memset(O, 0, len);
    char *P;
    const char *gr;
    char xCnt, yCnt;
    char occupation;
    char flow;

    // left to right
    for(gr = grid, yCnt = height; yCnt--;)
    {
        occupation = 0;
        flow = 0;
        P = NULL;
        for(xCnt = width; xCnt--; gr++)
        {
            if(*gr)
            {
                if(!occupation)
                {
                    occupation = *gr;
                    P = occupation == 'X' ? X : O;
                }
                else if(occupation != *gr)
                {
                    P = NULL;
                    break;
                }
                flow++;
            }
        }
        if(P)
        {
            P += (height - yCnt - 1) * width;
            for(xCnt = width; xCnt--; P++)
                *P += flow;
        }
    }
    // top to bottom
    for(gr = grid, xCnt = width; xCnt--; gr = grid + (width - xCnt))
    {
        occupation = 0;
        flow = 0;
        P = NULL;
        for(yCnt = height; yCnt--; gr += width)
        {
            if(*gr)
            {
                if(!occupation)
                {
                    occupation = *gr;
                    P = occupation == 'X' ? X : O;
                }
                else if(occupation != *gr)
                {
                    P = NULL;
                    break;
                }
                flow++;
            }
        }
        if(P)
        {
            P += width - xCnt - 1;
            for(yCnt = height; yCnt--; P += width)
                *P += flow;
        }
    }
}

#include <time.h>
#include <stdlib.h>

int main()
{
    srand(time(NULL));
    int const columns = 3;
    int const rows = 3;
    int const size = rows * columns;
    char grid[size];
    int i, j;
    for(i = 0; i < size; i++)
        grid[i] = 0;

    for(i = 0; i < size; i++)
    {
        grid[i] = (rand() % 10 > 5) ? 'X': ' ';
    }
    // grid[0 + 0 * columns] = 'X';
    for(j = 0; j < rows; j++)
    {
        for(i = 0; i < columns; i++)
        {
            printf("%c, ", grid[i + j * columns]);
        }
        printf("\n");
    }
    char buffer[size];
    char eat[size];
    TTT_GetHeatMap(grid, size, columns, rows, buffer, eat);
    for(j = 0; j < rows; j++)
    {
        for(i = 0; i < columns; i++)
        {
            printf("%d, ", buffer[i + j * columns]);
        }
        printf("\n");
    }
    return 0;
}