#include <stdio.h>
#include <stdlib.h>
#include "Chip8.h"

int main(int argc, char* argv[]) {
    int i;
    Chip8 vm;
    char* rom_path;
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc < 2){
        printf("Error: No ROM specified.");
        printf("Usage: chip8_emulator <location of ROM>\n");
        return 1;
    }
    rom_path = argv[1];
    vm.load_process_image(rom_path);
    vm.dump_memory();
    getchar(); // pause till keypress 
    // start execution
    while (1)
        vm.execute_cycle();
    return 0;
}
