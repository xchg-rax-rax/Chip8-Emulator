#include <stdio.h>
#include "../src/Chip8Screen.h"


int main() {
    int i, key;
    Chip8Screen scr;
    // draw a psychic cross
    //
    //   *
    // *****
    //   *
    //  ***
    //   *
    // *****
    //   *
    //
    scr.check_for_input();
    getchar(); 

    scr.draw_pixel(2,0,1);
    for (i = 0; i < 5; i++) {
        scr.draw_pixel(i,1,1);
    }
    scr.draw_pixel(2,2,1);
    for (i = 0; i < 5; i++) {
        scr.draw_pixel(i,3,1);
    }
    scr.draw_pixel(2,4,1);
    for (i = 0; i < 5; i++) {
        scr.draw_pixel(i,5,1);
    }
    scr.draw_pixel(2,6,1);
    
    getchar(); 

    scr.clear_screen();

    // Draw a line that exceeds the horizontal bounds
    for (i = 0; i < 128; i++)
        scr.draw_pixel(i,16,1);
    
    // Draw a line that exceeds the vertical bounds
    for (i = 0; i < 128; i++)
        scr.draw_pixel(32,i,1);
    
    getchar(); 
    scr.clear_screen();

    // this is likely wildly inefficient though in the case of our emulator it will be just fine
    while (1) {
        if ((key = scr.check_for_input()) != -1) {
            printf("Key pressed : %x\n", key);
        }
    }
    return 0;
}
