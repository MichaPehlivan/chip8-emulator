cmake_minimum_required(VERSION 3.0)
project(chip8-emulator)

add_executable(chip8-emulator src/main.c src/chip8.c src/chip8.h)

target_link_libraries(chip8-emulator SDL2)