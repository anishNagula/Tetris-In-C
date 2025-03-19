#ifndef TETROMINO_H
#include <SDL2/SDL.h>
extern int gameOver;

#define TETROMINO_H
#define TETROMINO_SIZE 4
#define BLOCK_SIZE 50
#define currentTetromino_SIZE 50
#define ROWS 20
#define COLS 10 

typedef struct {
    int filled;
    int colorIndex;
} GridCell;

extern GridCell grid[ROWS][COLS];

extern SDL_Color tetrominoColors[];

typedef struct {
    int shape[TETROMINO_SIZE][TETROMINO_SIZE];
    SDL_Rect rect;
    int x, y;
    int colorIndex;
} Tetromino;

extern Tetromino currentTetromino;

void initTetromino();
void rotateTetromino();
void moveTetromino(int dx, int dy);
void drawTetromino(SDL_Renderer *renderer);
int checkTetrominoCollision(int newX, int newY);
void placeCurrentTetrominoInGrid();
void spawnNewCurrentTetromino();

#endif