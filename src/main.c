#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "chip8.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 640;

// Keypad keymap
uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main() {
    Chip8 chip8;
    init(&chip8);
    load_rom(&chip8, "../roms/PONG");

    srand(time(NULL));
    
    SDL_Window* window = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create window
        window = SDL_CreateWindow( "Conway's Game Of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    int quit = 0;
    SDL_Event e;

    while (!quit)
    {
        //Handle events on queue
        while( SDL_PollEvent(&e) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = 1;
            }

            if(e.type == SDL_KEYDOWN) {
                if(e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 1;
                }

                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.keys[i] = 1;
                    }
                }
            }

            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.keys[i] = 0;
                    }
                }
            }
        }

        emulate(&chip8);
        //render display
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        for(int i = 0; i < 64; i++) {
            for(int j = 0; j < 32; j++) {
                if(chip8.display[i + 64*j]) {
                    SDL_Rect rect;
                    rect.w = 20;
                    rect.h = 20;
                    rect.x = i*20;
                    rect.y = j*20;
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        //present and sleep
        SDL_RenderPresent(renderer);
        usleep(1200);
    }

    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;   
}