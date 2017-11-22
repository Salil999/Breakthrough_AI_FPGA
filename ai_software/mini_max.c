#include "mini_max.h"
#include "vec.h"


void restore_board(char** backup, maze_struct* maze)
{
    int i, j;
    for (i = 0; i < 8; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            maze->array[i][j] = backup[i][j];
        }
    }
    destroy_maze(backup);
}


void save_board(char** backup, maze_struct* maze)
{
    int i, j;
    backup = (char**)malloc(8 * sizeof(char*));
    for (i = 0; i < 8; ++i)
    {
        backup[i] = (char*)malloc(8 * sizeof(char));
    }

    for (i = 0; i < 8; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            backup[i][j] = maze->array[i][j];
        }
    }
}



void possible_actions(pos position, pos moves[3], maze_struct* maze)
{
    int x, y, k, arr_index;
    int currX, currY;
    char color;
    pos temp;

    x = position.x; y = position.y; arr_index = 0;
    color = maze->array[x][y];

    if (color == 'W')
    {
        for (k = -1; k < 2; ++k)
        {
            currX = x + 1;
            currY = y + k;
            temp.x = currX; temp.y = currY;
            if (in_bounds(temp) == FALSE) continue;
            if (maze->array[currX][currY] == ' ' || maze->array[currX][currY] == 'B')
            {
                if (k == 0 && maze->array[currX][currY] == 'B')
                {
                }
                else
                {
                    moves[arr_index++] = temp;
                }
            }
        }
    }
    else if (color == 'B')
    {
        for (k = -1; k < 2; ++k)
        {
            currX = x - 1;
            currY = y + k;
            temp.x = currX; temp.y = currY;
            if (in_bounds(temp) == FALSE) continue;
            if (maze->array[currX][currY] == ' ' || maze->array[currX][currY] == 'W')
            {
                if (k == 0 && maze->array[currX][currY] == 'W')
                {
                }
                else
                {
                    moves[arr_index++] = temp;
                }
            }
        }
    }

    // This is just here to fill up the moves array with some invalid moves
    temp.x = -1; temp.y = -1;
    while (arr_index < 3)
    {
        moves[arr_index++] = temp;
    }

}


int in_bounds(pos position)
{
    int x, y;
    x = position.x; y = position.y;
    if (x < 8 && x >= 0 && y < 8 && y >= 0)
        return TRUE;
    return FALSE;
}


int count_alive_pieces(pos pieces[16])
{
    int i, ret_val;
    ret_val = 16;
    pos temp;
    for (i = 0; i < 16; ++i)
    {
        temp = pieces[i];
        if (temp.x == -1 || temp.y == -1)
        {
            ret_val--;
        }
    }
    return ret_val;
}


int utility(maze_struct* maze)
{
    int own_pieces, opponent_pieces;
    if (maze->turn % 2 == WHITE)
    {
        own_pieces = count_alive_pieces(maze->white_pieces);
        opponent_pieces = count_alive_pieces(maze->black_pieces);
    }
    else
    {
        opponent_pieces = count_alive_pieces(maze->white_pieces);
        own_pieces = count_alive_pieces(maze->black_pieces);
    }

    // based on the heuristic chosen, return the value for THAT heuristic
    return defensive_heuristic(own_pieces);
}


// an offensive heuristic that AI will use
int offensive_heuristic(int opponent_pieces)
{
    // return 2 * (30 - opponent_pieces) + random();
    return 0;
}


// a defensive heuristic that AI will use
int defensive_heuristic(int own_pieces)
{
    // return 2 * own_pieces + random();
    return 0;
}


alpha_beta alpha_beta_max(int depth, pos piece, int alpha, int beta, maze_struct* maze)
{
    int i, j;
    int pieces_length;
    pos move;
    pos* pieces;
    pos possible_moves[3];
    char** backup;

    alpha_beta ret_val, temp;
    pos base_position; base_position.x = 0; base_position.y = 0;
    ret_val.current = base_position; ret_val.move = base_position;

    if (depth > MAX_DEPTH)
    {
        ret_val.heuristic = utility(maze);
        return ret_val;
    }

    ret_val.heuristic = -INFINITY;
    if (maze->turn % 2 == WHITE)
    {
        if (depth % 2 == 0)
        {
            pieces = (pos *)maze->white_pieces;
            pieces_length = maze->white_pieces_length;
        }
        else
        {
            pieces = (pos *)maze->black_pieces;
            pieces_length = maze->black_pieces_length;
        }
    }
    else
    {
        if (depth % 2 == 0)
        {
            pieces = (pos *)maze->black_pieces;
            pieces_length = maze->black_pieces_length;
        }
        else
        {
            pieces = (pos *)maze->white_pieces;
            pieces_length = maze->white_pieces_length;
        }
    }


    for (i = 0; i < pieces_length; ++i)
    {
        // pieces[i]
        possible_actions(pieces[i], possible_moves, maze);
        for (j = 0; j < 3; ++j)
        {
            // possible_moves[j]
            move = possible_moves[j];
            save_board(backup, maze);

            // recurse
            temp = alpha_beta_min(depth + 1, pieces[i], alpha, beta, maze);

            if (temp.heuristic > ret_val.heuristic)
            {
                ret_val.heuristic = temp.heuristic;
                ret_val.current = pieces[i]; ret_val.move = possible_moves[j];
            }

            // restore the original game state
            restore_board(backup, maze);

            if (ret_val.heuristic >= beta)
            {
                return ret_val;
            }
            alpha = max(alpha, ret_val.heuristic);
        }
    }

    return ret_val;
}


alpha_beta alpha_beta_min(int depth, pos piece, int alpha, int beta, maze_struct* maze)
{
    int i, j;
    int pieces_length;
    pos move;
    pos* pieces;
    pos possible_moves[3];
    char** backup;

    alpha_beta ret_val, temp;
    pos base_position; base_position.x = 0; base_position.y = 0;
    ret_val.current = base_position; ret_val.move = base_position;

    if (depth > MAX_DEPTH)
    {
        ret_val.heuristic = utility(maze);
        return ret_val;
    }

    ret_val.heuristic = INFINITY;
    if (maze->turn % 2 == WHITE)
    {
        if (depth % 2 == 0)
        {
            pieces = (pos *)maze->white_pieces;
            pieces_length = maze->white_pieces_length;
        }
        else
        {
            pieces = (pos *)maze->black_pieces;
            pieces_length = maze->black_pieces_length;
        }
    }
    else
    {
        if (depth % 2 == 0)
        {
            pieces = (pos *)maze->black_pieces;
            pieces_length = maze->black_pieces_length;
        }
        else
        {
            pieces = (pos *)maze->white_pieces;
            pieces_length = maze->white_pieces_length;
        }
    }


    for (i = 0; i < pieces_length; ++i)
    {
        // pieces[i]
        possible_actions(pieces[i], possible_moves, maze);
        for (j = 0; j < 3; ++j)
        {
            // possible_moves[j]
            move = possible_moves[j];
            save_board(backup, maze);

            // recurse
            temp = alpha_beta_max(depth + 1, pieces[i], alpha, beta, maze);

            if (temp.heuristic < ret_val.heuristic)
            {
                ret_val.heuristic = temp.heuristic;
                ret_val.current = pieces[i]; ret_val.move = possible_moves[j];
            }

            // restore the original game state
            restore_board(backup, maze);

            if (ret_val.heuristic <= alpha)
            {
                return ret_val;
            }
            beta = min(beta, ret_val.heuristic);
        }
    }

    return ret_val;
}


// infinite loop that will handle all game logic
void run_game(maze_struct* maze)
{
    alpha_beta value;
    pos position;
    while (1)
    {
        print_maze(maze);
        if (maze->turn % 2 == WHITE)
        {
            value = alpha_beta_max(0, position, -INFINITY, INFINITY, maze);
        }
        else
        {
            value = alpha_beta_min(0, position, INFINITY, -INFINITY, maze);
        }
        if (maze->array[value.move.x][value.move.y] == 'B' || maze->array[value.move.x][value.move.y] == 'W')
        {
            if (maze->turn % 2 == WHITE)
            {
                // remove the "value" position piece from the black pieces
            }
            else
            {
                // remove the "value" position piece from the white pieces
            }
        }
        if (maze->turn % 2 == WHITE)
        {
            // remove value.current from white
            // add value.next to white
        }
        else
        {
            // remove value.current from black
            // add value.next to black
        }

        maze->array[value.move.x][value.move.y] = maze->array[value.current.x][value.current.y];
        maze->array[value.current.x][value.current.y] = ' ';
        maze->turn++;

    }
}


// Initializes the 8x8 board
void create_maze(maze_struct* maze)
{
    int i, j;
    int white_index, black_index;
    pos current_position;


    // Allocates the space on the heap
    maze->array = (char**)malloc(8 * sizeof(char*));
    for (i = 0; i < 8; ++i)
    {
        maze->array[i] = (char*)malloc(8 * sizeof(char));
    }

    // Initializes the board
    for (i = 0; i < 8; ++i)
    {
        if (i < 2)
        {
            for (j = 0; j < 8; ++j)
            {
                maze->array[i][j] = 'W';
            }
        }
        else if (i > 5)
        {
            for (j = 0; j < 8; ++j)
            {
                maze->array[i][j] = 'B';
            }
        }
        else
        {
            for (int j = 0; j < 8; ++j)
            {
                maze->array[i][j] = ' ';
            }
        }
    }

    // Sets the positions of the white and black pieces
    white_index = 0; black_index = 0;
    for (i = 0; i < 8; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            if (maze->array[i][j] == 'W')
            {
                current_position.x = i; current_position.y = j;
                maze->white_pieces[white_index++] = current_position;
            }
            else if (maze->array[i][j] == 'B')
            {
                current_position.x = i; current_position.y = j;
                maze->black_pieces[black_index++] = current_position;
            }
        }
    }

    // Sets the length of each of the white and black pieces arrays
    maze->white_pieces_length = 16;
    maze->black_pieces_length = 16;
}


// Clears the board so there are no memory leaks
void destroy_maze(char** maze)
{
    int i;
    for (i = 0; i < 8; ++i)
    {
        free(maze[i]);
        maze[i] = NULL;
    }
    free(maze);
    maze = NULL;
}


// Helper function to print the contents of the board
void print_maze(maze_struct* maze)
{
    int i, j;
    for (i = 0; i < 8; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            printf("%c ", maze->array[i][j]);
        }
        printf("\n");
    }
}


// main function to run
int main()
{
    // Generates the random seed
    // srand(time(NULL));
    maze_struct maze;
    maze.turn = WHITE;
    run_game(&maze);
    create_maze(&maze);
    print_maze(&maze);
    destroy_maze(maze.array);

    return 0;
}
