void remove_matches_in_board(char *board, int rows, int columns)
{
    int captured[rows * columns];
    int captured_count = 0;
    int n, i, j;
    int j_streak, i_streak;
    for(j_streak = 1, n = 0; n < rows * columns; n++)
    {
        i = n / rows;
        j = n % rows;
        int current_index = i + j * columns;
        int previous_index = i + (j - 1) * columns;
        if(board[previous_index] == board[current_index])
            j_streak++;
        else
        {
            if(j_streak >= 3)
            {
                int back;
                for(back = 0; back < j_streak + 0; back++) 
                {
                    captured[captured_count++] = i + ((j - 1) - back) * columns;
                }
            }
            j_streak = 1;
        }
        if(j_streak >= 3)
        {
            int back;
            for(back = 0; back < j_streak + 0; back++) 
            {
                captured[captured_count++] = i + ((rows - 1) - back) * columns;
            }
        }
        if(j == 0) j_streak = 1;

        j = n / columns;
        i = n % columns;
        current_index = i + j * columns;
        previous_index = (i - 1) + j * columns;
        if(board[previous_index] == board[current_index])
            i_streak++;
        else
        {
            if(i_streak >= 3)
            {
                int back;
                for(back = 0; back < i_streak + 0; back++) 
                {
                    captured[captured_count++] = ((i - 1) - back) + j * columns;
                }
            }
            i_streak = 1;
        }
        if(i_streak >= 3)
        {
            int back;
            for(back = 0; back < i_streak + 0; back++) 
            {
                captured[captured_count++] = ((columns - 1) - back) + j * columns;
            }
        }
        if(i == 0) i_streak = 1;
    }
    for(i = 0; i < captured_count; i++) board[captured[i]] = ' ';
}

void check_for_matches_in_board(char *board, int rows, int columns)
{
    int captured[rows * columns];
    int captured_count = 0;
    int n, i, j;
    int j_streak, i_streak;
    for(i_streak = 1, j_streak = 1, n = 0; n < rows * columns; n++)
    {
        i = n / rows;
        j = n % rows;
        int current_index = i + j * columns;
        int previous_index = i + (j - 1) * columns;
        int is_streak = board[previous_index] == board[current_index];
        if(j != 0 && is_streak)
            j_streak++;
        if(!is_streak && j != 0)
        {
            if(j_streak >= 3)
            {
                int back;
                for(back = 0; back < j_streak; back++) 
                {
                    captured[captured_count++] = i + ((j - 1) - back) * columns;
                }
            }
            j_streak = 1;
        }
        if(j_streak >= 3)
        {
            int back;
            for(back = 0; back < j_streak; back++) 
            {
                captured[captured_count++] = (i - 1) + ((rows - 1) - back) * columns;
            }
        }
        if(j == 0) j_streak = 1;

        j = n / columns;
        i = n % columns;
        current_index = i + j * columns;
        previous_index = (i - 1) + j * columns;
        i_streak = board[previous_index] == board[current_index];
        if(i != 0 && is_streak)
            i_streak++;
        if(!is_streak && i != 0)
        {
            if(i_streak >= 3)
            {
                int back;
                for(back = 0; back < i_streak; back++) 
                {
                    captured[captured_count++] = ((i - 1) - back) + j * columns;
                }
            }
            i_streak = 1;
        }
        if(i_streak >= 3)
        {
            int back;
            for(back = 0; back < i_streak; back++) 
            {
                captured[captured_count++] = ((columns - 1) - back) + j * columns;
            }
        }
        if(i == 0) i_streak = 1;
    }
    for(i = 0; i < captured_count; i++) board[captured[i]] = ' ';
}

void window(int *data, int size, int max_elements, int stride, void (*callback)(int *[], int size, struct WindowData *), struct WindowData *window_data)
{
    int i, j;
    int *window[size];
    if(window_data)
    {
        window_data->stride = &stride;
        window_data->index = &i;
    }
    for(i = 0; i < max_elements / stride - (size - 1); i++)
    {
        for(j = 0; j < size; j++)
        {
            window[j] = data + ((i + j) / stride + (i % stride + j) * stride) % (stride * stride);
        }
        callback(window, size, window_data);
    }
}