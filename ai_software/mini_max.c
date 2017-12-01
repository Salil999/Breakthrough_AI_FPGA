#include <unistd.h>
#include "mini_max.h"
#include "vec.h"

float get_random_number()
{
    int random_number = arc4random();
    return fabs((random_number % 1000) / 1000.0);
    // return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}


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

    x = position.x; y = position.y;
    arr_index = 0;
    color = maze->array[x][y];

    if (color == 'W')
    {
        for (k = -1; k < 2; ++k)
        {
            currX = x + 1;
            currY = y + k;
            temp.x = currX; temp.y = currY;
            if (!in_bounds(temp))
            {
                // printf("NOT IN BOUNDS: (%d, %d)\n", temp.x, temp.y);
                continue;
            }
            if (maze->array[currX][currY] == '_' || maze->array[currX][currY] == 'B')
            {
                if (k == 0 && maze->array[currX][currY] == 'B')
                {
                    // do nothing
                }
                else
                {
                    // printf("Adding: (%d, %d)\n", temp.x, temp.y);
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
            if (!in_bounds(temp))
            {
                // printf("NOT IN BOUNDS: (%d, %d)\n", temp.x, temp.y);
                continue;
            }
            if (maze->array[currX][currY] == '_' || maze->array[currX][currY] == 'W')
            {
                if (k == 0 && maze->array[currX][currY] == 'W')
                {
                    // do nothing
                }
                else
                {
                    // printf("Adding: (%d, %d)\n", temp.x, temp.y);
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
        // own_pieces = count_alive_pieces(maze->white_pieces);
        // opponent_pieces = count_alive_pieces(maze->black_pieces);
        own_pieces = maze->white_pieces_length;
        opponent_pieces = maze->black_pieces_length;
    }
    else
    {
        opponent_pieces = maze->white_pieces_length;
        own_pieces = maze->black_pieces_length;
        // opponent_pieces = count_alive_pieces(maze->white_pieces);
        // own_pieces = count_alive_pieces(maze->black_pieces);
    }

    // based on the heuristic chosen, return the value for THAT heuristic
    // return defensive_heuristic(own_pieces);
    return offensive_heuristic(opponent_pieces);
}


// an offensive heuristic that AI will use
float offensive_heuristic(int opponent_pieces)
{
    return 2 * (30 - opponent_pieces) + get_random_number();
}


void print_pos_array(pos* arr)
{
    int i;
    for (i = 0; i < 3; ++i)
    {
        printf("\t(%d, %d)\n", arr[i].x, arr[i].y);
    }
}


// a defensive heuristic that AI will use
float defensive_heuristic(int own_pieces)
{
    return 2 * own_pieces + get_random_number();
}


alpha_beta alpha_beta_max(int depth, int alpha, int beta, maze_struct* maze)
{
    alpha_beta ret_val;
    pos base_position;
    base_position.x = 0; base_position.y = 0;
    ret_val.current = base_position;
    ret_val.move = base_position;


    if (depth > MAX_DEPTH)
    {
        ret_val.heuristic = utility(maze);
        return ret_val;
    }

    int i, j, k, l;

    int pieces_length;
    pos* pieces = NULL;

    pos possible_moves[3];
    pos move;
    move.x = -999;
    move.y = -999;

    for (i = 0; i < 3; ++i)
    {
        possible_moves[i] = move;
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


    // char** backup;
    char backup[8][8];
    for (i = 0; i < pieces_length; ++i)
    {
        possible_actions(pieces[i], possible_moves, maze);
        // printf("current piece: (%d, %d), %c\n", pieces[i].x, pieces[i].y, maze->array[pieces[i].x][pieces[i].y]);
        // printf("possible moves:\n");
        // print_pos_array(possible_moves);
        for (j = 0; j < 3; ++j)
        {
            alpha_beta temp;
            move = possible_moves[j];

            if (move.x < 0)
            {
                continue;
            }
            // backup the current game state
            for (k = 0; k < 8; ++k)
            {
                for (l = 0; l < 8; ++l)
                {
                    backup[k][l] = maze->array[k][l];
                }
            }
            // save_board(backup, maze);

            // recurse
            // printf("depth + 1: %d\n", depth + 1);
            // print_pos_array(possible_moves);
            // printf("alpha: %d\n", alpha);
            // printf("beta: %d\n", beta);
            temp = alpha_beta_min(depth + 1, alpha, beta, maze);

            if (temp.heuristic > ret_val.heuristic)
            {
                ret_val.heuristic = temp.heuristic;
                ret_val.current = pieces[i];
                ret_val.move = possible_moves[j];
            }

            for (k = 0; k < 8; ++k)
            {
                for (l = 0; l < 8; ++l)
                {
                    maze->array[k][l] = backup[k][l];
                }
            }

            if (ret_val.heuristic >= beta)
            {
                return ret_val;
            }
            alpha = max(alpha, ret_val.heuristic);
        }
    }

    return ret_val;
}


alpha_beta alpha_beta_min(int depth, int alpha, int beta, maze_struct* maze)
{
    // char** backup;

    alpha_beta ret_val;
    pos base_position;
    base_position.x = 0; base_position.y = 0;
    ret_val.current = base_position;
    ret_val.move = base_position;

    if (depth > MAX_DEPTH)
    {
        ret_val.heuristic = utility(maze);
        return ret_val;
    }

    int i, j, k, l;

    int pieces_length;
    pos* pieces = NULL;

    pos possible_moves[3];
    pos move;
    move.x = -999;
    move.y = -999;

    for (i = 0; i < 3; ++i)
    {
        possible_moves[i] = move;
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


    char backup[8][8];
    for (i = 0; i < pieces_length; ++i)
    {
        possible_actions(pieces[i], possible_moves, maze);
        // printf("current piece: (%d, %d), %c\n", pieces[i].x, pieces[i].y, maze->array[pieces[i].x][pieces[i].y]);
        // printf("possible moves:\n");
        // print_pos_array(possible_moves);
        for (j = 0; j < 3; ++j)
        {
            alpha_beta temp;
            move = possible_moves[j];

            if (move.x < 0)
            {
                continue;
            }
            // save the current game state
            for (k = 0; k < 8; ++k)
            {
                for (l = 0; l < 8; ++l)
                {
                    backup[k][l] = maze->array[k][l];
                }
            }
            // save_board(backup, maze);

            // recurse
            // printf("depth + 1: %d\n", depth + 1);
            // printf("alpha: %d\n", alpha);
            // printf("beta: %d\n", beta);

            temp = alpha_beta_max(depth + 1, alpha, beta, maze);

            if (temp.heuristic < ret_val.heuristic)
            {
                ret_val.heuristic = temp.heuristic;
                ret_val.current = pieces[i];
                ret_val.move = possible_moves[j];
            }

            // restore the original game state
            for (k = 0; k < 8; ++k)
            {
                for (l = 0; l < 8; ++l)
                {
                    backup[k][l] = maze->array[k][l];
                }
            }
            // restore_board(backup, maze);

            if (ret_val.heuristic <= alpha)
            {
                return ret_val;
            }
            beta = min(beta, ret_val.heuristic);
        }
    }

    return ret_val;
}


void find_and_remove(pos pieces[16], pos item, int len)
{
    int i;
    for (i = 0; i < len; ++i)
    {
        if (pieces[i].x == item.x && pieces[i].y == item.y)
        {
            break;
        }
    }
    pieces[i] = pieces[len - 1];
}


// infinite loop that will handle all game logic
void run_game(maze_struct* maze)
{
    while (1)
    {
        alpha_beta value;
        pos position;
        if (maze->turn % 2 == WHITE)
        {
            value = alpha_beta_max(0, -INFINITY, INFINITY, maze);
        }
        else
        {
            value = alpha_beta_min(0, INFINITY, -INFINITY, maze);
        }
        printf("Current: (%d, %d)\nMove: (%d, %d)\nCurrent-Character: %c\nMove-Character: %c\n",
               value.current.x, value.current.y,
               value.move.x, value.move.y,
               maze->array[value.current.x][value.current.y],
               maze->array[value.move.x][value.move.y]
              );

        // curr -> current
        // nxt -> move

        if (maze->array[value.move.x][value.move.y] == 'B' || maze->array[value.move.x][value.move.y] == 'W')
        {
            if (maze->turn % 2 == WHITE)
            {
                find_and_remove(maze->black_pieces, value.move, maze->black_pieces_length);
                maze->black_pieces_length--;
                // remove the "value" position piece from the black pieces
            }
            else
            {
                find_and_remove(maze->white_pieces, value.move, maze->white_pieces_length);
                maze->white_pieces_length--;
                // remove the "value" position piece from the white pieces
            }
        }
        if (maze->turn % 2 == WHITE)
        {
            find_and_remove(maze->white_pieces, value.current, maze->white_pieces_length);
            maze->white_pieces[maze->white_pieces_length - 1] = value.move;
            maze->white_pieces_length++;
            // remove value.current from white
            // add value.next to white
        }
        else
        {
            find_and_remove(maze->black_pieces, value.current, maze->black_pieces_length);
            maze->black_pieces[maze->black_pieces_length - 1] = value.move;
            maze->black_pieces_length++;
            // remove value.current from black
            // add value.next to black
        }
        maze->array[value.move.x][value.move.y] = maze->array[value.current.x][value.current.y];
        maze->array[value.current.x][value.current.y] = '_';
        maze->turn++;
        printf("%d\n\n", maze->turn);
        print_maze(maze);

        sleep(1);
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
            for (j = 0; j < 8; ++j)
            {
                maze->array[i][j] = '_';
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

    maze->turn = 0;
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
    printf("\n");
}


// main function to run
int main()
{
    // Generates the random seed
    maze_struct maze;
    maze.turn = WHITE;
    create_maze(&maze);
    run_game(&maze);
    print_maze(&maze);
    destroy_maze(maze.array);

    return 0;
}
