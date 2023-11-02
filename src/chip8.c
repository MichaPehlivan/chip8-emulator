#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "chip8.h"

//macros to avoid arrow operator
#define memory chip8->memory
#define V chip8->V
#define I chip8->I
#define pc chip8->pc
#define stack chip8->stack
#define sp chip8->sp
#define delay_timer chip8->delay_timer
#define keys chip8->keys
#define display chip8->display
#define draw_flag chip8->draw_flag

unsigned char fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

//initialise variables
void init(Chip8 *chip8) {
    //clear memory
    for(int i = 0; i < sizeof(memory); i++) {
        memory[i] = 0;
    }

    //load fontset
    for(int i = 0; i < sizeof(fontset); i++) {
        memory[i] = fontset[i];
    }

    //clear registers and key map
    for(int i = 0; i < sizeof(keys); i++) {
        V[i] = 0;
        keys[i] = 0;
    }
    I = 0;

    //reset pc
    pc = 512; //the emulator itself was usually stored in the first 512 bytes, so pc starts from there

    //clear stack
    for(int i = 0; i < sizeof(stack); i++) {
        stack[i] = 0;
    }
    sp = 0;

    //reset delay timer
    delay_timer = 0;

    //clear display
    for(int i = 0; i < sizeof(display); i++) {
        display[i] = 0;
    }

    //reset draw flag
    draw_flag = 0;
}

//emulate a single opcode
void emulate(Chip8 *chip8) {
    int opcode = memory[pc] << 8 | memory[pc + 1];

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: //00E0 clear screen
                    for(int i = 0; i < sizeof(display); i++) {
                        display[i] = 0;
                    }

                    draw_flag = 1;
                    pc += 2;
                    break;

                case 0x000E: //00EE //return from subroutine
                    sp--;
                    pc = stack[sp];

                    pc += 2;
                    break;
                
                default:
                    printf("unknown opcode: %x\n", opcode);
                    pc += 2;
                    break;
            }
            break;

        case 0x1000: //1NNN jump to adress NNN
            pc = (opcode & 0x0FFF);
            break;

        case 0x2000: //2NNN call subroutine at NNN
            stack[sp] = pc;
            sp++;

            pc = (opcode & 0x0FFF);
            break;

        case 0x3000: //3XNN skip next instruction if Vx == NN
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;

        case 0x4000: //4XNN skip next instruction if Vx != NN
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;

        case 0x5000: //5XY0 skip next instruction if Vx == Vy
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;

        case 0x6000: //6XNN set Vx to NN
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);

            pc += 2;
            break;

        case 0x7000: //7XNN add NN to Vx (carry flag not changed)
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);

            pc += 2;
            break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: // 8XY0 set Vx to the value of Vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];

                    pc += 2;
                    break;
                
                case 0x0001: //8XY1 set Vx to Vx bitwise or Vy
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0002: //8XY2 set Vx to Vx bitwise and Vy
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0003: //8XY3 set Vx to Vx xor Vy
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0004: //8XY4 add Vy to Vx, VF is set to 1 if there is a carry, 0 if not
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];

                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1; //carry
                    else
                        V[0xF] = 0;
                    
                    pc += 2;
                    break;

                case 0x0005: //8XY5 subtract Vy from Vx, VF is set to 0 if there is a borrow, 1 if not
                    if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                        V[0xF] = 0; //borrow
                    else
                        V[0xF] = 1;

                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    
                    pc += 2;
                    break;

                case 0x0006: //8XY6 stores the least significant bit of Vx in VF and shifts Vx to the right by one
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;

                    pc += 2;
                    break;

                case 0x0007: //8XY6 set Vx to Vy - Vx, VF is set to 0 if there is a borrow, 1 if not
                    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 0; //borrow
                    }
                    else {
                        V[0xF] = 1;
                    }

                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];

                    pc += 2;
                    break;

                case 0x000E: //8XYE stores the most significant bit of Vx in VF and shifts Vx to the left by one
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;

                    pc += 2;
                    break;

                default:
                    printf("unknown opcode: %x\n", opcode);
                    pc += 2;
                    break;
            } 
            break;

        case 0x9000: //9XY0 skip next instruction if Vx != Vy
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            }
            else {
                pc += 2;
            }

            break;

        case 0xA000: //ANNN set I to address NNN
            I = (opcode & 0x0FFF);

            pc += 2;
            break;

        case 0xB000: //BNNN jump to address NNN + V0
            pc = (opcode & 0x0FFF) + V[0];
            
            break;

        case 0xC000: //CXNN set Vx to the bitwise and of a random number and NN
            int random_number = rand() % 256;
            V[(opcode & 0x0F00) >> 8] = (random_number & (opcode & 0x00FF));

            pc += 2;
            break;

        case 0xD000: 
            //DXYN draw a sprite at coordinates (Vx, Vy) with a width of 8 and a height of N. 
            //each row is read bit coded starting at memory address I
            //VF is set to 1 if any pixels are flipped from 1 to 0, and to 0 if this does not happen

            ///*
            int x = V[(opcode & 0x0F00) >> 8];
            int y = V[(opcode & 0x00F0) >> 4];
            int n = (opcode & 0x000F);
            int data_start = I;

            V[0xF] = 0;
            for(int i = 0; i < n; i++) { //row
                unsigned char row = memory[data_start + i];

                for(int j = 0; j < 8; j++) { //column
                    unsigned char pixel = row & (0x80 >> j);
                    int index = BOARD_WIDHT*(y+i)+(x+j);
                    
                    if((pixel > 0)) {
                        if(display[index] == 1) {
                            V[0xF] = 1;
                        }
                        display[index] ^= 1;
                    }
                }
            }

            draw_flag = 1;
            pc += 2;
            break;

        case 0xE000:
            switch (opcode & 0x000F) {
                case 0x000E: //EX9E skip next instruction if the key stored in Vx is pressed
                    if(keys[V[(opcode & 0x0F00) >> 8]] != 0) {
                        pc += 4;
                    }
                    else {
                        pc += 2;
                    }

                    break;

                case 0x0001: //EXA1 //skip next instruction if the key stored in Vx is not pressed
                    if(keys[V[(opcode & 0x0F00) >> 8]] == 0) {
                        pc += 4;
                    }
                    else {
                        pc += 2;
                    }

                    break;
            
                default:
                    printf("unknown opcode: %x\n", opcode);
                    pc += 2;
                    break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: //FX07 set Vx to the value of the delay timer
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;

                    break;

                case 0x000A: //FX0A wait for a key press, and store it in Vx
                    {
                        bool key_pressed = false;

                        for(int i = 0; i < sizeof(keys); i++) {
                            if(keys[i] != 0)
                            {
                                V[(opcode & 0x0F00) >> 8] = i;
                                key_pressed = true;
                            }
                        }

                        if(!key_pressed) {
                            return;
                        }

                        pc += 2;
                    }

                    break;

                case 0x0015: //FX15 set the delay timer to Vx
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;

                    break;

                case 0x0018: //FX18 set the sound timer to Vx
                    pc += 2; //sound not implemented
                    break;

                case 0x001E: //FX1E add Vx to I, VF is not affected
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x0029: //FX29 set I to the location of the sprite for the characted in Vx
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;

                case 0x0033: 
                    //FX33 store the binary-coded decimal representation of VX 
                    //with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
                    pc += 2; 
                    break;

                case 0x0055: //FX55 store V0 to Vx (including Vx) in memory starting at memory location I (I is not affected)
                    {
                        int start = I;
                        int x = ((opcode & 0x0F00) >> 8);

                        for(int i = 0; i <= x; i++) {
                            memory[start + i] = V[i];
                        }
                        pc += 2;
                    }

                    break;

                case 0x0065: //FX65 fill V0 to Vx (including Vx) with values from memory starting at memory location I (I is not affected)
                    {
                        int start = I;
                        int x = ((opcode & 0x0F00) >> 8);

                        for(int i = 0; i <= x; i++) {
                            V[i] = memory[start + i];
                        }
                        pc += 2;
                    }

                    break;
                
                default:
                    printf("unknown opcode: %x\n", opcode);
                    pc += 2;
                    break;
            }
            break;

        default:
            printf("unknown opcode: %x\n", opcode);
            pc += 2;
            break;
    }

    if(delay_timer > 0) {
        --delay_timer;
    } 
}

//load a rom into memory
void load_rom(Chip8 *chip8, const char *path) {
    FILE *rom = fopen(path, "rb");
    
    bool EOF_reached = false;
    int memory_index = 512;
    while(!EOF_reached) {
        int c = getc(rom);
        if(c == EOF) {
            EOF_reached = true;
        }
        else {
            memory[memory_index] = c;
        }
        memory_index++;
    }

    fclose(rom);
}
