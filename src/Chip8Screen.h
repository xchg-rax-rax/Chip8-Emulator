/*
 *  The canvas class encapsulates the chip8 screen and provides 
 *  a simple interface while maintaining the X11 UI components 
 *  and managing user input.
 */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

class Chip8Screen {
    private:
        // private variables
        int screen;
        unsigned long black, white;
        Display* dis;
        Window win;
        GC gc;
    public:
        //public methods
        Chip8Screen();
        ~Chip8Screen();
        void draw_pixel(int chip8_x, int chip8_y, int mode);
        void clear_screen();
        unsigned char check_for_input();
};
