#pragma once

// Global LCD theme color variables
#define themeBackground 0xFFFF 
#define menuBtnText 0xFFFF 
#define menuBtnBorder 0x0000 
#define menuBtnColor 0xFC00 
#define menuBackground 0x3AF1 

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Blue-ish Grey 0x3AF1
// Grey 0x4228
// Orange 0xFC00

// For the draw shape functions
#define LEFT 1
#define CENTER 2
#define RIGHT 3


//extern UTFTGLUE myGLCD;
extern bool Touch_getXY();
extern int x;
extern int y;
extern char keyboardInput[10];
extern uint8_t keypadInput[3];
extern void waitForIt(int, int, int, int);
extern void waitForItRect(int, int, int, int);
extern void drawRoundBtn(int, int, int, int, String, int, int , int, int );
extern void drawSquareBtn(int, int, int, int, String, int, int, int, int );