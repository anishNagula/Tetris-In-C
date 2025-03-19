#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 20
#define COLS 10
#define BLOCK_SIZE 40
#define TETROMINO_SIZE 4

/* 
    Compile: gcc -o tetris main.c $(pkg-config --cflags --libs sdl2)
    Run: ./tetris
*/

// initialize grid
int grid[ROWS][COLS] = {0};

typedef struct {
    int shape[TETROMINO_SIZE][TETROMINO_SIZE];
    int x, y;
} Tetromino;

Tetromino tetrominoes[] = {
    {   // T-Block
        .shape = {
            {0, 0, 0, 0},
            {0, 1, 1, 1},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        .x = 3, .y = 0
    },
    {   // I-Block
        .shape = {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        .x = 3, .y = 0
    },
    {   // O-Block
        .shape = {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        .x = 4, .y = 0
    },
    {   // L-Block
        .shape = {
            {0, 0, 0, 0},
            {0, 1, 1, 1},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        .x = 3, .y = 0
    },
    {   // J-Block
        .shape = {
            {0, 0, 0, 0},
            {0, 1, 1, 1},
            {0, 0, 0, 1},
            {0, 0, 0, 0}
        },
        .x = 3, .y = 0
    },
    {   // S-Block
        .shape = {
            {0, 0, 0, 0},
            {0, 0, 1, 1},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        .x = 3, .y = 0
    },
    {   // Z-Block
        .shape = {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 1},
            {0, 0, 0, 0}
        },
        .x = 3, .y = 0
    }
};

Tetromino t_block;

const int SCREEN_WIDTH = COLS * BLOCK_SIZE;
const int SCREEN_HEIGHT = ROWS * BLOCK_SIZE;

void placeBlockInGrid();
int checkCollision(int newX, int newY);
void renderTetromino(SDL_Renderer *renderer, Tetromino *tetromino);
void spawnNewBlock();
void rotateBlock(Tetromino *tetromino);

int main(int argc, char *args[]) {
    srand(time(NULL));
    
    atexit(SDL_Quit);
    SDL_Window* screen = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Could not init SDL: %s\n", SDL_GetError());
        return 1;
    } 

    screen = SDL_CreateWindow("Tetris",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        0);

    if(!screen) {
        fprintf(stderr, "Could not create window\n");
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);

    if(!renderer) {
        fprintf(stderr, "Could not create renderer\n");
        return 1;
    }

    spawnNewBlock();

    SDL_Event event;
    int running = 1;
    Uint32 lastDropTime = SDL_GetTicks();
    Uint32 dropInterval = 500;

    while (running) {

        // clearing screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // drawing stored blocks in grid
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                if (grid[row][col] == 1) {
                    SDL_Rect rect = {col * BLOCK_SIZE, row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        // drawing moving t_block
        renderTetromino(renderer, &t_block);

        // moving t_block down (gravity)
        if (SDL_GetTicks() - lastDropTime > dropInterval) {
            if (!checkCollision(t_block.x, t_block.y + 1)) {
                t_block.y += 1;
            } else {
                placeBlockInGrid();
                spawnNewBlock();
            }
            lastDropTime = SDL_GetTicks();
        }

        SDL_RenderPresent(renderer);

        // movements (keys)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if(event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (!checkCollision(t_block.x - 1, t_block.y)) {
                            t_block.x -= 1;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (!checkCollision(t_block.x + 1, t_block.y)) {
                            t_block.x += 1;
                        }
                        break;
                    case SDLK_DOWN:
                        if (!checkCollision(t_block.x, t_block.y + 1)) {
                            t_block.y += 1;
                        }
                        break;
                    case SDLK_UP:
                        rotateBlock(&t_block);
                        break;
                }
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();
    
    return 0;
}

// store t_block upon landing
void placeBlockInGrid() {

    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (t_block.shape[i][j] == 1) {
                int gridX = t_block.x + j;
                int gridY = t_block.y + i;

                if (gridY >= 0 && gridY < ROWS && gridX >= 0 && gridX < COLS) {
                    grid[gridY][gridX] = 1;
                }
            }
        }
    }

    spawnNewBlock();
}

// render the t_block
void renderTetromino(SDL_Renderer *renderer, Tetromino *tetromino) {
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);

    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (tetromino->shape[i][j] == 1) {
                SDL_Rect rect = {
                    (tetromino->x + j) * BLOCK_SIZE,
                    (tetromino->y + i) * BLOCK_SIZE,  
                    BLOCK_SIZE,
                    BLOCK_SIZE,
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    
}

// spawn new t_block
void spawnNewBlock() {
    
    int randomIndex = rand() % 7;
    t_block = tetrominoes[randomIndex];

    // Check if the new block collides immediately (game over scenario)
    if (checkCollision(t_block.x, t_block.y)) {
        printf("Game Over!\n");
        exit(0);
    }

}

// collision logic
int checkCollision(int newX, int newY) {
    
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (t_block.shape[i][j] == 1) {
                int gridX = newX + j;
                int gridY = newY + i;

                if (gridX < 0 || gridX >= COLS || gridY >= ROWS) {
                    return 1;
                }

                if (gridY >= 0 && grid[gridY][gridX] == 1) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

void rotateBlock(Tetromino *tetromino) {
    int temp[TETROMINO_SIZE][TETROMINO_SIZE] = {0};

    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            temp[j][TETROMINO_SIZE - 1 - i] = tetromino->shape[i][j];
        }
    }

    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            tetromino->shape[i][j] = temp[i][j];
        }
    }
}