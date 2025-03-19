#include <stdio.h>
#include <SDL2/SDL.h>

#define ROWS 20
#define COLS 10
#define BLOCK_SIZE 40

/* 
    Compile: gcc -o tetris main.c $(pkg-config --cflags --libs sdl2)
    Run: ./tetris
*/

// initialize grid
int grid[ROWS][COLS] = {0};

const int SCREEN_WIDTH = COLS * BLOCK_SIZE;
const int SCREEN_HEIGHT = ROWS * BLOCK_SIZE;
const int FPS = 60;
const int frameDelay = 1000 / FPS;

// initialize block
SDL_Rect block = {SCREEN_WIDTH / 2 - BLOCK_SIZE, 0, BLOCK_SIZE, BLOCK_SIZE};

Uint32 frameStart;
int frameTime;

void placeBlockInGrid();
void spawnNewBlock();
int checkCollision(int newX, int newY);

int main(int argc, char *args[]) {
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

    SDL_Event event;
    int running = 1;
    Uint32 lastDropTime = SDL_GetTicks();
    Uint32 dropInterval = 500;

    while (running) {
        frameStart = SDL_GetTicks();

        // clearing screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // drawing stored blocks in grid
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                if (grid[row][col] == 1) {
                    SDL_Rect rect = {col * BLOCK_SIZE, row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        // drawing moving block
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &block);

        // moving block down (gravity)
        if (SDL_GetTicks() - lastDropTime > dropInterval) {
            if (!checkCollision(block.x, block.y + BLOCK_SIZE)) {
                block.y += BLOCK_SIZE;
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
                        if (!checkCollision(block.x - BLOCK_SIZE, block.y)) {
                            block.x -= BLOCK_SIZE;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (!checkCollision(block.x + BLOCK_SIZE, block.y)) {
                            block.x += BLOCK_SIZE;
                        }
                        break;
                    case SDLK_DOWN:
                        if (!checkCollision(block.x, block.y + BLOCK_SIZE)) {
                            block.y += BLOCK_SIZE;
                        }
                        break;
                }
            }
        }

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();
    
    return 0;
}

// store block upon landing
void placeBlockInGrid() {
    int gridX = block.x / BLOCK_SIZE;
    int gridY = block.y / BLOCK_SIZE;

    grid[gridY][gridX] = 1;
}

// spawn new block
void spawnNewBlock() {
    block.x = SCREEN_WIDTH / 2 - BLOCK_SIZE;
    block.y = 0;
}

// collision logic
int checkCollision(int newX, int newY) {
    int gridX = newX / BLOCK_SIZE;
    int gridY = newY / BLOCK_SIZE;

    if (gridY >= ROWS) return 1;
    if (grid[gridY][gridX] == 1) return 1;

    return 0;
}
