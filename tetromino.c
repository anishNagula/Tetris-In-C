#include "tetromino.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>

GridCell grid[ROWS][COLS] = {0};
Tetromino currentTetromino;

SDL_Color tetrominoColors[7] = {
    {0, 255, 255, 255}, // Cyan (I)
    {255, 255, 0, 255}, // Yellow (O)
    {128, 0, 128, 255}, // Purple (T)
    {0, 0, 255, 255},   // Blue (J)
    {255, 165, 0, 255}, // Orange (L)
    {0, 255, 0, 255},   // Green (S)
    {255, 0, 0, 255}    // Red (Z)
};

void initTetromino() {
    int tBlock[4][4] = {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    
    memcpy(currentTetromino.shape, tBlock, sizeof(tBlock));
    currentTetromino.x = 3;
    currentTetromino.y = 0;
    currentTetromino.colorIndex = 2; // Default to T-tetromino color
}

int isRotationValid(int temp[4][4], int newX, int newY) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (temp[i][j] == 1) {
                int gridX = newX + j;
                int gridY = newY + i;

                // Check if out of bounds
                if (gridX < 0 || gridX >= COLS || gridY >= ROWS) return 0;

                // Check if colliding with an existing block
                if (gridY >= 0 && grid[gridY][gridX].filled) return 0;
            }
        }
    }
    return 1; // Rotation is valid
}

void rotateTetromino() {
    int temp[4][4];

    // Create rotated shape
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j][3 - i] = currentTetromino.shape[i][j];
        }
    }

    // Check if rotation is valid
    if (isRotationValid(temp, currentTetromino.x, currentTetromino.y)) {
        memcpy(currentTetromino.shape, temp, sizeof(temp)); // Apply rotation
    }
}


void moveTetromino(int dx, int dy) {
    int newX = currentTetromino.x + dx;
    int newY = currentTetromino.y + dy;
    
    if (!checkTetrominoCollision(newX, newY)) {
        currentTetromino.x = newX;
        currentTetromino.y = newY;
    }
}

void drawTetromino(SDL_Renderer *renderer) {
    SDL_Color color = tetrominoColors[currentTetromino.colorIndex];
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (currentTetromino.shape[i][j] == 1) {
                SDL_Rect rect = {
                    (currentTetromino.x + j) * BLOCK_SIZE,
                    (currentTetromino.y + i) * BLOCK_SIZE,
                    BLOCK_SIZE, BLOCK_SIZE
                };
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            }
        }
    }
}

int checkTetrominoCollision(int newX, int newY) {
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (currentTetromino.shape[i][j] == 1) {
                int gridX = newX + j;
                int gridY = newY + i;
                
                if (gridX < 0 || gridX >= COLS || gridY >= ROWS) return 1;
                if (gridY >= 0 && grid[gridY][gridX].filled) return 1;
            }
        }
    }
    return 0;
}

void placeCurrentTetrominoInGrid() {
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (currentTetromino.shape[i][j] == 1) {
                int gridX = currentTetromino.x + j;
                int gridY = currentTetromino.y + i;

                // **Game Over Condition**
                if (gridY < 0) {
                    gameOver = 1;
                    return; // Stop placing blocks
                }

                if (gridY >= 0 && gridY < ROWS && gridX >= 0 && gridX < COLS) {
                    grid[gridY][gridX].filled = 1;
                    grid[gridY][gridX].colorIndex = currentTetromino.colorIndex;
                }
            }
        }
    }
}

void spawnNewCurrentTetromino() {
    int shapeIndex = rand() % 7;
    int tetrominoShapes[7][4][4] = {
        {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
    };
    memcpy(currentTetromino.shape, tetrominoShapes[shapeIndex], sizeof(tetrominoShapes[shapeIndex]));
    
    currentTetromino.x = COLS / 2 - 2;
    currentTetromino.y = 0;
    currentTetromino.colorIndex = shapeIndex;
}