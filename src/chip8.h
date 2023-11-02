#include <stdint.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 640
#define PIXEL_SIZE 20
#define BOARD_WIDHT 64
#define BOARD_HEIGHT 32

typedef struct {
    uint8_t memory[4096]; //memory
    uint8_t V[16]; //registers V0 to VF
    uint16_t I; //Index register
    
    uint16_t pc; //program counter

    uint16_t stack[48]; //stack
    uint16_t sp; //stack pointer
    
    uint16_t delay_timer; //delay timer
    
    uint8_t keys[16]; //key map
    uint8_t display[2048]; //graphics display

    uint8_t draw_flag; //draw flag
} Chip8;

//initialise variables
void init(Chip8 *chip8);

//emulate a single opcode
void emulate(Chip8 *chip8);

//load a rom into memory
void load_rom(Chip8 *chip8, const char *path);
