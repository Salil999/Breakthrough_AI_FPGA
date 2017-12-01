#ifndef MINIMAX_H
#define MINIMAX_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define max(a, b) ( ((a) > (b)) ? (a) : (b) )
#define min(a, b) ( ((a) < (b)) ? (a) : (b) )

#define WHITE 0
#define BLACK 1
#define MAX_DEPTH 3
#define TRUE 1
#define FALSE 0

// struct that acts as a position
struct pos
{
    int x;
    int y;
};
typedef struct pos pos;

// struct that will hold our maze object
struct maze_struct
{
    char** array;
    int turn;
    pos white_pieces[16];
    pos black_pieces[16];
    int white_pieces_length;
    int black_pieces_length;
};
typedef struct maze_struct maze_struct;

// struct that acts as the return value from our alpha-beta functions
struct alpha_beta
{
    float heuristic;
    pos current;
    pos move;
};
typedef struct alpha_beta alpha_beta;

// Maze manipulating functions
void create_maze(maze_struct* maze);
void destroy_maze(char** maze);
void print_maze(maze_struct* maze);
void run_game(maze_struct* maze);
alpha_beta alpha_beta_max(int depth, pos piece, int alpha, int beta, maze_struct* maze);
alpha_beta alpha_beta_min(int depth, pos piece, int alpha, int beta, maze_struct* maze);
float defensive_heuristic(int own_pieces);
float offensive_heuristic(int opponent_pieces);
int utility(maze_struct* maze);
int count_alive_pieces(pos pieces[16]);
int in_bounds(pos position);
void possible_actions(pos position, pos moves[3], maze_struct* maze);
void save_board(char** backup, maze_struct* maze);
void restore_board(char** backup, maze_struct* maze);
float get_random_number();
void print_pos_array(pos* arr);
void find_and_remove(pos pieces[16], pos item, int len);

#endif
