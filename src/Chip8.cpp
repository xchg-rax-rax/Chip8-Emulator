#include "Chip8.h"
#include "Chip8Screen.h"

// Definition of the Chip8 font-set
static unsigned char font[16][5] = { 
                             {0xF0,0x90,0x90,0x90,0xF0}, // 0
                             {0x20,0x60,0x20,0x20,0x70}, // 1
                             {0xF0,0x10,0xF0,0x80,0xF0}, // 2
                             {0xF0,0x10,0xF0,0x10,0xF0}, // 3
                             {0x90,0x90,0xF0,0x10,0x10}, // 4
                             {0xF0,0x80,0xF0,0x10,0xF0}, // 5
                             {0xF0,0x80,0xF0,0x90,0xF0}, // 6
                             {0xF0,0x10,0x20,0x40,0x40}, // 7
                             {0xF0,0x90,0xF0,0x90,0xF0}, // 8
                             {0xF0,0x90,0xF0,0x10,0xF0}, // 9
                             {0xF0,0x90,0xF0,0x90,0x90}, // A
                             {0xE0,0x90,0xE0,0x90,0xE0}, // B
                             {0xF0,0x80,0x80,0x80,0xF0}, // C
                             {0xE0,0x90,0x90,0x90,0xE0}, // D
                             {0xF0,0x80,0xF0,0x80,0xF0}, // E
                             {0xF0,0x80,0xF0,0x80,0x80}  // F
                            };

// CPU instructions
// each function has an name that describes it's action followed by it's associated opcode

void Chip8::execute_machine_language_subroutine_0NNN(unsigned short opcode) {
    unsigned short NNN = opcode & 0xFFF;
    // This instruction is only used to run machine specific binary for old hardware and
    // need not be implemented in modern emulators .
}

void Chip8::clear_screen_00E0(unsigned short opcode) {
    memset(screen_state,0,sizeof(screen_state));
    screen.clear_screen();
    // boy, was past me stupid or what? :')
    // keeping this as a comment in order to keep me humble
    /*
    int i,j;
    for (i = 0; i < sizeof(screen_state)/sizeof(screen_state[0]); i++){ 
        memset(screen_state[i],0,sizeof(screen_state[0]));
        for (j = 0; j < sizeof(screen_state[0]); j++)
            mvprintw(3+i,22+j," ");
    }
    */
}

void Chip8::return_00EE(unsigned short opcode) {
    IP = stack[SP-1];
    SP--;
}

void Chip8::jump_to_1NNN(unsigned short opcode) {
    unsigned short NNN = opcode & 0xFFF;
    IP = NNN - 2;
}

void Chip8::execute_subroutine_2NNN(unsigned short opcode) {
    unsigned short NNN = opcode & 0xFFF;
    stack[SP] = IP;
    SP++;
    IP = NNN - 2;
}

void Chip8::skip_if_VX_equals_NN_3XNN(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned char NN = opcode & 0xFF;
    if (registers[X] == NN)
        IP += 2;
}

void Chip8::skip_if_not_VX_equal_NN_4XNN(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned char NN = opcode & 0xFF;
    if (registers[X] != NN)
        IP += 2;
}

void Chip8::skip_if_VX_equal_VY_5XY0(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    if (registers[X] == registers[Y])
        IP += 2;
}

void Chip8::load_NN_into_VX_6XNN(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned char NN = opcode & 0xFF;
    registers[X] = NN;
}

void Chip8::add_NN_to_VX_7XNN(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned char NN = opcode & 0xFF;
    registers[X] += NN;
}

void Chip8::mov_VX_to_VY_8XY0(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    registers[X] = registers[Y];
}

void Chip8::set_VX_to_VX_OR_VY_8XY1(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    registers[X] = registers[X] | registers[Y];
}

void Chip8::set_VX_to_VX_AND_VY_8XY2(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    registers[X] = registers[X] & registers[Y];
}

void Chip8::set_VX_to_VX_XOR_VY_8XY3(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    registers[X] = registers[X] ^ registers[Y];
}

void Chip8::add_VY_to_VX_8XY4(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    if (registers[X] < USHRT_MAX - registers[Y])
        registers[0xF] = 1;
    else
        registers[0xF] = 0;
    registers[X] += registers[Y];
}

void Chip8::sub_VY_from_VX_8XY5(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    if (registers[X] > registers[Y])
        registers[0xF] = 1;
    else
        registers[0xF] = 0;
    registers[X] -= registers[Y];
}

void Chip8::right_shift_VY_store_in_VX_8XY6(unsigned short opcode) {
    // This opcode was originally undocumented and has had two separate implementations
    // in one VX is right shifted and stored in itself in the other VY is right shifted
    // and stored in VX. Just gotta hope you picked the right one or that it doesn't
    // get used by anyone!!
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    if (registers[X] & 1 == 1)
        registers[0xF] = 1;
    else
        registers[0xF] = 0;
    registers[X] = (registers[X] >> 1);
    //registers[X] = (registers[Y] >> 1);
}

void Chip8::set_VX_to_VY_minus_VX_8XY7(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    if (registers[X] < registers[Y])
        registers[0xF] = 1;
    else
        registers[0xF] = 0;
    registers[X] = registers[Y] - registers[X];
}

void Chip8::left_shift_VY_store_in_VX_8XYE(unsigned short opcode) {
    // This opcode was originally undocumented and has had two separate implementations
    // in one VX is left shifted and stored in itself in the other VY is left shifted
    // and stored in VX. Just gotta hope you picked the left one or that it doesn't
    // get used by anyone!!
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    if (registers[X] & 1 == 1)
        registers[0xF] = 1;
    else
        registers[0xF] = 0;
    registers[X] = (registers[X] << 1);
    //registers[X] = (registers[Y] >> 1);
}

void Chip8::skip_if_VX_not_equal_VY_9XY0(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    if (registers[X] != registers[Y])
        IP += 2;
}

void Chip8::store_NNN_in_I_ANNN(unsigned short opcode) {
    unsigned short NNN = opcode & 0xFFF;
    I = NNN;
}

void Chip8::jump_to_NNN_plus_V0_BNNN(unsigned short opcode) {
   unsigned short NNN = opcode & 0xFFF;
    // may not be *2
    IP = NNN + registers[0]*2;
}

void Chip8::set_VX_to_random_with_mask_NN_CXNN(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short NN = opcode & 0xFF;
    registers[X] = (unsigned short) (NN & rand());
}

void Chip8::draw_N_sprite_at_VX_VY_DXYN(unsigned short opcode) {
    // error ahoy
    unsigned short i, j;
    unsigned short val;
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    unsigned short N = (opcode & 0xF);
    registers[0xF] = 0;
    for (i = 0; i < N; i++){
        for (j = 0; j < 8; j++) {
            // update internal state
            val = screen_state[(registers[Y] + i) % 32][(registers[X] - 1 + j) % 64];
            screen_state[(registers[Y] + i) % 32][(registers[X] + j) % 64] ^= (unsigned char) (memory[I+i]&(1 << (7-j))) >> (7-j);

            //set flag if previous set pixel unset
            if (screen_state[(registers[Y]+i)%32][(registers[X]-1+j)%64] == 0 && val == 1)
                registers[0xF] = 1;

            // for the moment I'll use the naive aproach and just redraw the screen on every iteration
            // this is awfully in efficient but for now I simply aim to get it working before optimizing
            // it's easier to speed up a moving car than one that won't even start
            //redraw_screen();
            if (screen_state[(registers[Y]+i)%32][(registers[X]-1+j)%64]) {
                screen.draw_pixel((registers[X]-1+j)%64, (registers[Y]+i)%32, 1);
            }
            else { 
                screen.draw_pixel((registers[X]-1+j)%64, (registers[Y]+i)%32, 0);
            }
     //       mvprintw(40,10,"i:%02x j:%02x x;%02x y:%02x %02x %02x %02x ",i,j,(registers[Y]+i)%32,(registers[X]-1+j)%64,val,screen_state[(registers[Y]+i)%32][(registers[X]-1+j)%64], (unsigned char) (memory[I+i]&(1 << (7-j))) >> (7-j));
      //      getch();
        }
    }
}

void Chip8::skip_if_VX_key_pressed_EX9E(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    // check for key
    unsigned char key = screen.check_for_input();
    if (key != 0xFF){
        printf("Key Pressed : %x\n", key);
        if (registers[X] == key) 
            IP += 2;
    }
}

void Chip8::skip_if_VX_not_pressed_EXA1(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned short Y = (opcode & 0xF0) >> 4;
    // check for key
    unsigned char key = screen.check_for_input();
    if (registers[X] != key) 
        IP += 2;
}

void Chip8::store_delay_timer_in_VX_FX07(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    registers[X] = timers[0];
}

void Chip8::store_keypress_in_VX_FX0A(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    unsigned char key = 0xFF; 
    while (key == 0xFF) {
        key = screen.check_for_input();
    }
    printf("Key Pressed : %x\n", key);
    registers[X] = key;
}

void Chip8::set_delay_timer_to_VX_FX15(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    timers[0] = registers[X];
}

void Chip8::set_sound_timer_to_VX_FX18(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    timers[1] = registers[X];
}

void Chip8::add_VX_to_I_FX1E(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    I += registers[X];
}

void Chip8::set_I_to_addr_of_VX_sprite_FX29(unsigned short opcode) {
    unsigned short X = (opcode & 0xF00) >> 8;
    I = 0x50 + 5*registers[X];
}

void Chip8::store_VX_as_BCD_at_I_FX33(unsigned short opcode) {
    unsigned short val;
    unsigned short X = (opcode & 0xF00) >> 8;
    val = registers[X];
    memory[I] = (char) (val / 100);
    val -= memory[I]*100;
    memory[I+1] = (char) (val / 10);
    val -= memory[I+1]*10;
    memory[I+2] = (char) (val);
}

void Chip8::store_V0_to_VX_in_I_FX55(unsigned short opcode) {
    // could and should make I the iterator for this
    int i;
    unsigned short X = (opcode & 0xF00) >> 8;
    for (i = 0; i<X+1; i++)
        memory[I + i] = registers[i];
    I += X + 1;

}

void Chip8::set_V0_to_VX_to_I_FX65(unsigned short opcode) {
    // could and should make I the iterator for this
    int i;
    unsigned short X = (opcode & 0xF00) >> 8;
    for (i = 0; i<X+1; i++)
        registers[i] = memory[I + i]; 
    I += X + 1;
}

// Initialization Functions

Chip8::Chip8() {
    // set up the initial state  
    IP = 0x200;
    memset(memory,0,sizeof(memory));
    memset(registers,0,sizeof(registers));
    memset(&(I),0,sizeof(I));
    memset(stack,0,sizeof(stack));
    SP = 0;
    memset(timers,0,sizeof(timers));
    gettimeofday(&(last_timer_update),NULL);
    memset(screen_state,0,sizeof(screen_state));
    load_font_into_memory();
}

Chip8::~Chip8() {
     // destructor
     // gracefully destroy window
}

void Chip8::load_process_image(char* rom_path) {
    FILE* rom_fd;
    unsigned int rom_size;
    unsigned int bytes_read;
    if (access(rom_path, R_OK) != 0) {
        printf("Error: Cannot read ROM at path %s.\n", rom_path);
        exit(1);
    }
    // load rom into chip8 memory
    rom_fd = fopen(rom_path, "r");
    if (!rom_fd) {
        printf("Error: Could not open ROM at path %s.\n", rom_path);
        exit(1);
    }
    fseek(rom_fd, 0L, SEEK_END); // go to end of file
    rom_size  = ftell(rom_fd);
    rewind(rom_fd);
    if (rom_size > 4096-512) {
        printf("Error: ROM is of size %d bytes and exceeds capacity of 4KB memory.\n",rom_size);
        fclose(rom_fd);
        exit(1);
    }
    bytes_read = fread(memory+0x200,rom_size,1,rom_fd)*rom_size;
    if (bytes_read != rom_size) {
        printf("Error: Failed to read complete ROM into memory. Read %d, expected %d\n", bytes_read);
        fclose(rom_fd);
        exit(1);
    }
    fclose(rom_fd);
}

void Chip8::load_font_into_memory() {
    int i,j,k;
    k = 80;
    for (i = 0; i < sizeof(font)/sizeof(font[0]); i++){
        for (j = 0; j < sizeof(font[0]); j++){
            memory[k] = font[i][j];
            k++;
        }
    }
}


// Cycle Functions
void Chip8::increment_timers() {
    int i;
    unsigned short update;
    struct timeval now;
    gettimeofday(&now,NULL);
    update = (unsigned short) (now.tv_usec - (last_timer_update).tv_usec);
    if (update > 16666) {
        for (i = 0; i<2; i++) {
            if (timers[i] > 0){
                timers[i] -= 1;
            }
        }
        last_timer_update = now;
    }
}

void Chip8::redraw_screen() {
    int i, j;
    for (i = 0; i < 64; i++){
        for (j = 0; j < 32; j++) {
            screen.draw_pixel(i, j, screen_state[j][i]);
        }
    }
}


void Chip8::execute_cycle() {
    unsigned short opcode;
    unsigned short lsn; // least significant nibble
    opcode = memory[IP] << 8;
    opcode += memory[IP+1];
    usleep(1000);
    // looking up instruction via case statement
    switch((opcode&0xF000) >> 12) {
        case 0x0:
            if (opcode == 0x00E0)
                clear_screen_00E0(opcode);
            else if (opcode == 0x00EE)
                return_00EE(opcode);
            else
                // 0NNN
                printf("Instruction is not implemented : %04x\n",opcode);
            break;
        case 0x1:
            jump_to_1NNN(opcode);
            break;
        case 0x2:
            execute_subroutine_2NNN(opcode);
            break;
        case 0x3:
            skip_if_VX_equals_NN_3XNN(opcode);
            break;
        case 0x4:
            skip_if_not_VX_equal_NN_4XNN(opcode);
            break;
        case 0x5:
            skip_if_VX_equal_VY_5XY0(opcode);
            break;
        case 0x6:
            load_NN_into_VX_6XNN(opcode);
            break;
        case 0x7:
            add_NN_to_VX_7XNN(opcode);
            break;
        case 0x8:
            switch(opcode & 0xF) {
                case 0x0: 
                    mov_VX_to_VY_8XY0(opcode);
                    break;
                case 0x1: 
                    set_VX_to_VX_OR_VY_8XY1(opcode);
                    break;
                case 0x2: 
                    set_VX_to_VX_AND_VY_8XY2(opcode);
                    break;
                case 0x3: 
                    set_VX_to_VX_XOR_VY_8XY3(opcode);
                    break;
                case 0x4: 
                    add_VY_to_VX_8XY4(opcode);
                    break;
                case 0x5: 
                    sub_VY_from_VX_8XY5(opcode);
                    break;
                case 0x6: 
                    right_shift_VY_store_in_VX_8XY6(opcode);
                    break;
                case 0x7: 
                    set_VX_to_VY_minus_VX_8XY7(opcode); 
                    break;
                case 0xE: 
                    left_shift_VY_store_in_VX_8XYE(opcode);
                    break;
                default:
                    printf("Instruction is not implemented (undocumented) : %04x\n",opcode);
            }
            break;
        case 0x9:
            if ((opcode & 0xF) == 0x0)
                skip_if_VX_not_equal_VY_9XY0(opcode);
            else
                printf("Instruction is not implemented (undocumented) : %04x\n",opcode);
            break;
        case 0xA:
            store_NNN_in_I_ANNN(opcode);
            break;
        case 0xB:
            jump_to_NNN_plus_V0_BNNN(opcode);
            break;
        case 0xC:
            set_VX_to_random_with_mask_NN_CXNN(opcode);
            break;
        case 0xD:
            draw_N_sprite_at_VX_VY_DXYN(opcode);
            break;
        case 0xE:
            if ((opcode & 0xFF) == 0x9E)
                skip_if_VX_key_pressed_EX9E(opcode);
            else if ((opcode & 0xFF) == 0xA1)
                skip_if_VX_not_pressed_EXA1(opcode);
            else
                printf("Instruction is not implemented (undocumented) : %04x\n",opcode);
            break;
        case 0xF:
            switch(opcode & 0xFF) {
                case 0x07:
                    store_delay_timer_in_VX_FX07(opcode);
                    break;
                case 0x0A:
                    store_keypress_in_VX_FX0A(opcode); 
                    break;
                case 0x15:
                    set_delay_timer_to_VX_FX15(opcode);
                    break;
                case 0x18:
                    set_sound_timer_to_VX_FX18(opcode);
                    break;
                case 0x1E:
                    add_VX_to_I_FX1E(opcode);
                    break;
                case 0x29:
                    set_I_to_addr_of_VX_sprite_FX29(opcode);
                    break;
                case 0x33:
                    store_VX_as_BCD_at_I_FX33(opcode);
                    break;
                case 0x55:
                    store_V0_to_VX_in_I_FX55(opcode);
                    break;
                case 0x65:
                    set_V0_to_VX_to_I_FX65(opcode);
                    break;
                default:
                    printf("Instruction is not implemented (undocumented) : %04x\n",opcode);
            }
            break;
        default:
            printf("An error has been made by the author : %04x\n",opcode);
    }
    // executing instruction simple function call 
    increment_timers();
    //update_display(opcode);
    IP += 2;
 }

void Chip8::dump_memory() {
    for (int i=0; i < 4096; i++){
        if (i % 32 == 0)
            printf("%04x : ",i);
        printf("%02X ",memory[i]);
        if (i % 32 == 31)
            printf("\n");
    }
}

