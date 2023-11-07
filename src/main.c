#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "chip8.h"

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

int main(int argc, char* argv[]) {
    
    char *rom_path;
    if(argc == 1) {
        printf("specify rom path\n");
        return EXIT_FAILURE;
    }
    else if(argc == 2) {
        rom_path = argv[1];
    }
    else if(argc > 2) {
        printf("too many arguments!\n");
        return EXIT_FAILURE;
    }

    Chip8 chip8;
    init(&chip8);
    load_rom(&chip8, rom_path);

    srand(time(NULL));
    
    SDL_Window* window;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        //Create window
        window = SDL_CreateWindow("Chip8 emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( window == NULL )
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
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

                for (int i = 0; i < sizeof(keymap); i++) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.keys[i] = 1;
                    }
                }
            }

            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < sizeof(keymap); i++) {
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

        if(chip8.draw_flag) {
            chip8.draw_flag = 0;
            
            for(int i = 0; i < 64; i++) {
                for(int j = 0; j < 32; j++) {
                    if(chip8.display[i + 64*j]) {
                        SDL_Rect rect;
                        rect.w = PIXEL_SIZE;
                        rect.h = PIXEL_SIZE;
                        rect.x = i*PIXEL_SIZE;
                        rect.y = j*PIXEL_SIZE;
                        SDL_RenderFillRect(renderer, &rect);
                    }
                }
            }

            SDL_RenderPresent(renderer);
        }

        usleep(1200);
    }

    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;   
}
