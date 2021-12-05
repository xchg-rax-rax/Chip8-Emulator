#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <locale.h>
#include <limits.h>

#include "Chip8Screen.h"

class Chip8 {
    private:
        // CPU state 
        unsigned short IP; // The instruction pointer points to the instruction to be executed  
        unsigned char memory[4096]; // Chip8 systems have 4kB of addressable, the first 512 bytes are reserved
        unsigned char registers[16]; // Chip8 has 16 8 bit registers named V0 to VF, VF doubles as a flag
        unsigned short I; // The addresses register, used for memory rw
        unsigned short stack[24]; // The call stack only stores return addresses
        unsigned short SP; // The stack pointer, must be between 0 and 23, inicates the location of the last object placed onto the stack
        unsigned char timers[2]; // The sound and delay timers, count down and 60Hz when non zero
        struct timeval last_timer_update;
       
        // Graphics
        Chip8Screen screen;
        unsigned char screen_state[32][64]; // y,x 1F by 3F screen

        // chip8 instruction set
        void execute_machine_language_subroutine_0NNN(unsigned short opcode);
        void clear_screen_00E0(unsigned short opcode);
        void return_00EE(unsigned short opcode);
        void jump_to_1NNN(unsigned short opcode);
        void execute_subroutine_2NNN(unsigned short opcode);
        void skip_if_VX_equals_NN_3XNN(unsigned short opcode);
        void skip_if_not_VX_equal_NN_4XNN(unsigned short opcode);
        void skip_if_VX_equal_VY_5XY0(unsigned short opcode);
        void load_NN_into_VX_6XNN(unsigned short opcode);
        void add_NN_to_VX_7XNN(unsigned short opcode);
        void mov_VX_to_VY_8XY0(unsigned short opcode);
        void set_VX_to_VX_OR_VY_8XY1(unsigned short opcode);
        void set_VX_to_VX_AND_VY_8XY2(unsigned short opcode);
        void set_VX_to_VX_XOR_VY_8XY3(unsigned short opcode);
        void add_VY_to_VX_8XY4(unsigned short opcode);
        void sub_VY_from_VX_8XY5(unsigned short opcode);
        void right_shift_VY_store_in_VX_8XY6(unsigned short opcode);
        void set_VX_to_VY_minus_VX_8XY7(unsigned short opcode);
        void left_shift_VY_store_in_VX_8XYE(unsigned short opcode);
        void skip_if_VX_not_equal_VY_9XY0(unsigned short opcode);
        void store_NNN_in_I_ANNN(unsigned short opcode);
        void jump_to_NNN_plus_V0_BNNN(unsigned short opcode);
        void set_VX_to_random_with_mask_NN_CXNN(unsigned short opcode);
        void draw_N_sprite_at_VX_VY_DXYN(unsigned short opcode);
        void skip_if_VX_key_pressed_EX9E(unsigned short opcode);
        void skip_if_VX_not_pressed_EXA1(unsigned short opcode);
        void store_delay_timer_in_VX_FX07(unsigned short opcode);
        void store_keypress_in_VX_FX0A(unsigned short opcode);
        void set_delay_timer_to_VX_FX15(unsigned short opcode);
        void set_sound_timer_to_VX_FX18(unsigned short opcode);
        void add_VX_to_I_FX1E(unsigned short opcode);
        void set_I_to_addr_of_VX_sprite_FX29(unsigned short opcode);
        void store_VX_as_BCD_at_I_FX33(unsigned short opcode);
        void store_V0_to_VX_in_I_FX55(unsigned short opcode);
        void set_V0_to_VX_to_I_FX65(unsigned short opcode);

        // loop methods
        void increment_timers();

        // screen methods
        void redraw_screen();
        void load_font_into_memory();

    public:
        Chip8();
        ~Chip8();
        void load_process_image(char* rom_path);
        void dump_memory();
        void execute_cycle();
};

