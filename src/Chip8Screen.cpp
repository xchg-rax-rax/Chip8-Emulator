/*
 *  The canvas class encapsulates the chip8 screen and provides 
 *  a simple interface while maintaining the X11 UI components 
 *  and managing user input.
 *  I don't think that the way in which I have implemented this 
 *  is very canonical wrt. X11 and will doubtless have to refactor 
 *  it in the future when I understand the library better
 *  It is likely architectural and hence I may have to refactor 
 *  the other class in addition to this one...
 */

#include "Chip8Screen.h"

#define HEIGHT 320 
#define WIDTH 640 

#define RGB(r, g, b) (unsigned long) ((r<<16)+ (g<<8) + b)

Chip8Screen::Chip8Screen() {
    // Open display and get default screen
    dis = XOpenDisplay(NULL);
    screen = DefaultScreen(dis);

    // Get Black, White and Red values
    black = BlackPixel(dis, screen);
    white = RGB(0,255,0); //WhitePixel(dis, screen);
    // Is this stupid? Why ^^ if this? What's the canonical way of generating colours

    // Create window and configure
    win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, WIDTH, HEIGHT, 5, black, black);
    XSetStandardProperties(dis, win, "Chip8Screen", "Chip8Screen", None, NULL, 0, NULL);

    // Select the kind of events we wish X11 to inform us of 
    XSelectInput(dis, win, ExposureMask | KeyPressMask);

    // Create the graphics context
    gc = XCreateGC(dis, win, 0,0);
    
    // Set draw colours
    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);

    // Clear the window and bring to the front
    XClearWindow(dis, win);
    XMapRaised(dis, win);
}

unsigned char Chip8Screen::check_for_input() { 
    char text[32];
    XEvent event;
    KeySym key;

    if (XPending(dis)) {
        // Wait for event and handle if key press or redraw event
        // Get next event (is this blocking?)
        XNextEvent(dis, &event);
        if(event.type == Expose && event.xexpose.count == 0) {
            XClearWindow(dis, win);
        }
        else if(event.type == KeyPress && XLookupString(&event.xkey, text, 255, &key,0)==1) {
            // If user presses q, quit. Otherwise print char pressed to the terminal
            switch(text[0]) {
                case '1':
                    return 1;
                    break;
                case '2':
                    return 2;
                    break;
                case '3':
                    return 3;
                    break;
                case '4':
                    return 0xC;
                    break;
                case 'q':
                    return 4;
                    break;
                case 'w':
                    return 5;
                    break;
                case 'e':
                    return 6;
                    break;
                case 'r':
                    return 0xD;
                    break;
                case 'a':
                    return 7;
                    break;
                case 's':
                    return 8;
                    break;
                case 'd':
                    return 9;
                    break;
                case 'f':
                    return 0xE;
                    break;
                case 'z':
                    return 0xA;
                    break;
                case 'x':
                    return 0;
                    break;
                case 'c':
                    return 0xB;
                    break;
                case 'v':
                    return 0xF;
                    break;
                default:
                    //printf("You pressed the %c key!\n",text[0]);
                    return 0xFF;
            }
        }
    }
    return 0xFF;
}

void Chip8Screen::draw_pixel(int chip8_x, int chip8_y, int mode) {
    int screen_x, screen_y;

    // Convert to the X11 windows coordinate system
    screen_x = (chip8_x*10) % WIDTH;
    screen_y = (chip8_y*10) % HEIGHT;

    // Set colour depending on if we are drawing or erasing
    if (mode != 0) {
        //printf("Drawing pixel at %d, %d\n", screen_x, screen_y);
        XSetForeground(dis, gc, white);
        XSetBackground(dis, gc, white);
    } 
    else {
        XSetForeground(dis, gc, black);
        XSetBackground(dis, gc, black);
    }
    // Draw a 10x10 'pixel' to the screen
    XFillRectangle(dis, win, gc, screen_x, screen_y, 10, 10);  
    XFlush(dis);
}

void Chip8Screen::clear_screen() {
    XClearWindow(dis, win);
}

Chip8Screen::~Chip8Screen() {
    // Free graphics context
    XFreeGC(dis, gc);
    // Destroy the window
    XDestroyWindow(dis, win);
    // Close our display handle
    XCloseDisplay(dis);
}

