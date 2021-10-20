/*
 Name:		TFTLCD.ino
 Created:	1/16/2021 2:59:46 PM
 Author:	Brandon Van Pelt
*/

/*
* TFT display framework
* Tested on Arduino Mega2560 and Due using LI9486 or LI 9488
*/

/*=========================================================
    Settings
===========================================================*/
// Select display
#define LI9486
//#define LI9488

#if defined LI9486
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <UTFTGLUE.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000

int16_t BOXSIZE;
int16_t PENRADIUS = 1;
uint16_t ID, oldcolor, currentcolor;
uint8_t Orientation = 0;    //PORTRAIT

// Initialize display
//(byte model, int RS, int WR, int CS, int RST, int SER)
UTFTGLUE myGLCD(0, A2, A1, A3, A4, A0); //all dummy args

char* name = "Please Calibrate.";  //edit name of shield
const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341
const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;
#elif defined LI9488
#include <UTFT.h>
#include <memorysaver.h>
#include <UTouchCD.h>
#include <UTouch.h>
UTFT myGLCD(ILI9488_16, 7, 38, 9, 10);
//RTP: byte tclk, byte tcs, byte din, byte dout, byte irq
UTouch  myTouch(2, 6, 3, 4, 5);
// External import for fonts
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
#endif // 

// Select debug Due
//#define DEBUG(x)  SerialUSB.print(x);
#define DEBUG(x)  Serial.print(x);
//#define DEBUG(x)


/*=========================================================
    Definitions / Globals
===========================================================*/
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
#define BLUEISH  0x0066CC

// Blue-ish Grey 0x3AF1
// Grey 0x4228
// Orange 0xFC00

// For the draw shape functions
#define LEFT 1
#define CENTER 2
#define RIGHT 3

#define VERSION "TFTLCD Ver. 1.0.0"

// For touch controls
int x, y;

// Keeps track of current page
uint8_t graphicLoaderState = 0;
uint8_t page = 1;
uint8_t nextPage = 1;
bool hasDrawn = false;


/*=========================================================
    Framework
===========================================================*/

/***************************************************
*  Draw Round/Square Button                        *
*                                                  *
*  Description:   Draws shapes with/without text   *
*                                                  *
*  Parameters: x start, y start, x stop, y stop    *
*              String: Button text                 *
*              Hex value: Background Color         *
*              Hex value: Border of shape          *
*              Hex value: Color of text            *
*              int: Alignment of text #defined as  *
*                   LEFT, CENTER, RIGHT            *
*                                                  *
***************************************************/
void drawRoundBtn(int x_start, int y_start, int x_stop, int y_stop, String button, int backgroundColor, int btnBorderColor, int btnTxtColor, int align) {
    int size, temp, offset;

    myGLCD.setColor(backgroundColor);
    myGLCD.fillRoundRect(x_start, y_start, x_stop, y_stop); // H_Start, V_Start, H_Stop, V_Stop
    myGLCD.setColor(btnBorderColor);
    myGLCD.drawRoundRect(x_start, y_start, x_stop, y_stop);
    myGLCD.setColor(btnTxtColor); // text color
    myGLCD.setBackColor(backgroundColor); // text background
    switch (align)
    {
    case 1:
        myGLCD.print(button, x_start + 5, y_start + ((y_stop - y_start) / 2) - 8); // hor, ver
        break;
    case 2:
        size = button.length();
        temp = ((x_stop - x_start) / 2);
        offset = x_start + (temp - (8 * size));
        myGLCD.print(button, offset, y_start + ((y_stop - y_start) / 2) - 8); // hor, ver
        break;
    case 3:
        // Right align was situationally programed, needs fixed
        myGLCD.print(button, x_start + 55, y_start + ((y_stop - y_start) / 2) - 8); // hor, ver
        break;
    default:
        break;
    }

}
void drawSquareBtn(int x_start, int y_start, int x_stop, int y_stop, String button, int backgroundColor, int btnBorderColor, int btnTxtColor, int align) {
    int size, temp, offset;
    myGLCD.setColor(backgroundColor);
    myGLCD.fillRect(x_start, y_start, x_stop, y_stop); // H_Start, V_Start, H_Stop, V_Stop
    myGLCD.setColor(btnBorderColor);
    myGLCD.drawRect(x_start, y_start, x_stop, y_stop);
    myGLCD.setColor(btnTxtColor); // text color
    myGLCD.setBackColor(backgroundColor); // text background
    switch (align)
    {
    case 1:
        myGLCD.print(button, x_start + 5, y_start + ((y_stop - y_start) / 2) - 8); // hor, ver
        break;
    case 2:
        size = button.length();
        temp = ((x_stop - x_start) / 2);
        offset = x_start + (temp - (8 * size));
        myGLCD.print(button, offset, y_start + ((y_stop - y_start) / 2) - 8); // hor, ver
        break;
    case 3:
        // Right align was situationally programed, needs fixed
        myGLCD.print(button, x_start + 55, y_start + ((y_stop - y_start) / 2) - 8); // hor, ver
        break;
    default:
        break;
    }
}

// Highlights round buttons when selected
void waitForIt(int x1, int y1, int x2, int y2)
{
    myGLCD.setColor(themeBackground);
    myGLCD.drawRoundRect(x1, y1, x2, y2);

    unsigned long timer = millis();
    while (Touch_getXY() || millis() - timer < 20)
    {
        if (Touch_getXY())
        {
            timer = millis();
        }
        backgroundProcess();
    }
    myGLCD.setColor(BLACK);
    myGLCD.drawRoundRect(x1, y1, x2, y2);
}

// Highlights square buttons when selected
void waitForItRect(int x1, int y1, int x2, int y2)
{
    myGLCD.setColor(themeBackground);
    myGLCD.drawRect(x1, y1, x2, y2);

    unsigned long timer = millis();
    while (Touch_getXY() || millis() - timer < 20)
    {
        if (Touch_getXY())
        {
            timer = millis();
        }
        backgroundProcess();
    }
    myGLCD.setColor(menuBtnBorder);
    myGLCD.drawRect(x1, y1, x2, y2);
}

// Page control framework
void pageControl()
{
    // Check if button on menu is pushed
    menuButtons();

    // Switch which page to load
    switch (page)
    {
    case 1:
        // Draw Page
        if (!hasDrawn)
        {
            if (!drawPage1())
            {
                graphicLoaderState++;
            }
            else
            {
                hasDrawn = true;
            }
        }

        // Call buttons if any


        // Call anything before page changes
        if (nextPage != page)
        {
            hasDrawn = false;
            page = nextPage;
        }
        break;
    case 2:
        // Draw Page
        if (!hasDrawn)
        {
            if (!drawPage2())
            {
                graphicLoaderState++;
            }
            else
            {
                hasDrawn = true;
            }
        }

        // Call buttons if any


        // Call anything before page changes
        if (nextPage != page)
        {
            hasDrawn = false;
            page = nextPage;
        }
        break;
    case 3:
        // Draw Page
        if (!hasDrawn)
        {
            hasDrawn = true;
        }

        // Call buttons if any


        // Call anything before page changes
        if (nextPage != page)
        {
            hasDrawn = false;
            page = nextPage;
        }
        break;
    case 4:
        // Draw Page
        if (!hasDrawn)
        {
            hasDrawn = true;
        }

        // Call buttons if any


        // Call anything before page changes
        if (nextPage != page)
        {
            hasDrawn = false;
            page = nextPage;
        }
        break;
    case 5:
        // Draw Page
        if (!hasDrawn)
        {
            hasDrawn = true;
        }

        // Call buttons if any


        // Call anything before page changes
        if (nextPage != page)
        {
            hasDrawn = false;
            page = nextPage;
        }
        break;
    }
}

// Setup TFT 
void setup()
{
    Serial.begin(115200);
    Serial3.begin(115200);
    myGLCD.InitLCD();
    myGLCD.clrScr();

#if defined LI9486
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
#elif defined LI9488
    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);
#endif
    myGLCD.setFont(BigFont);
    myGLCD.setBackColor(0, 0, 255);

    drawMenu();
}

// Draw the side menu
void drawMenu()
{
    // Draw Layout
    drawSquareBtn(0, 0, 479, 319, "", themeBackground, themeBackground, themeBackground, CENTER);
    drawSquareBtn(0, 0, 130, 319, "", menuBackground, menuBackground, menuBackground, CENTER);

    // Draw Menu Buttons
    drawRoundBtn(5, 32, 125, 83, F("Home"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(5, 88, 125, 140, F("Lights"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(5, 145, 125, 197, F("Sensors"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(5, 202, 125, 254, F("Power"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(5, 259, 125, 312, F("Config"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
}

// Buttons for the main menu
void menuButtons()
{
    // Touch screen controls
    if (Touch_getXY())
    {
        if ((x >= 5) && (x <= 125))
        {
            if ((y >= 32) && (y <= 83))
            {
                // CANBUS
                waitForIt(5, 32, 125, 83);
                nextPage = 1;
                graphicLoaderState = 0;
            }
            if ((y >= 88) && (y <= 140))
            {
                // VEHTOOL
                waitForIt(5, 88, 125, 140);
                nextPage = 2;
                graphicLoaderState = 0;
            }
            if ((y >= 145) && (y <= 197))
            {
                // RZRTOOL
                waitForIt(5, 145, 125, 197);
                nextPage = 3;
                graphicLoaderState = 0;
            }
            if ((y >= 202) && (y <= 254))
            {
                // EXTRAFN
                waitForIt(5, 202, 125, 254);
                nextPage = 4;
                graphicLoaderState = 0;
            }
            if ((y >= 259) && (y <= 312))
            {
                // SETTING
                waitForIt(5, 259, 125, 312);
                nextPage = 5;
                graphicLoaderState = 0;
            }
        }
    }
}

bool Touch_getXY(void)
{
#if defined LI9486
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) 
    {
        y = 320 - map(p.x, TS_LEFT, TS_RT, 0, 320); //.kbv makes sense to me
        x = map(p.y, TS_TOP, TS_BOT, 0, 480);
    }
    return pressed;
#elif defined LI9488
    if (myTouch.dataAvailable())
    {
        myTouch.read();
        x = myTouch.getX();
        y = myTouch.getY();
        return true;
    }
    return false;
#endif
}

void backgroundProcess()
{
    uint8_t temp[9];

    if (Serial3.available() > 9)
    {
        uint8_t test = Serial3.read();
        if (test == 0xFF)
        {
            temp[0] = Serial3.read();
            temp[1] = Serial3.read();
            temp[2] = Serial3.read();
            temp[3] = Serial3.read();
            temp[4] = Serial3.read();
            temp[5] = Serial3.read();
            temp[6] = Serial3.read();
            temp[7] = Serial3.read();
            temp[8] = Serial3.read();
        }
        Serial.println(temp[1]);
        Serial.println(temp[2]);
    }
}

// the loop function runs over and over again until power down or reset
void loop() 
{
    // GUI
    pageControl();

    // Background processes
    backgroundProcess();
}


/*=========================================================
    Page1
===========================================================*/
// Draws the menu 1 page
bool drawPage1()
{
    switch (graphicLoaderState)
    {
    case 0:
        break;
    case 1:
        drawSquareBtn(131, 55, 479, 319, "", themeBackground, themeBackground, themeBackground, CENTER);
        break;
    case 2:
        drawRoundBtn(140, 80, 305, 130, F("Option 1"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 3:
        drawRoundBtn(310, 80, 475, 130, F("Option 2"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 4:
        drawRoundBtn(140, 135, 305, 185, F("Option 3"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 5:
        drawRoundBtn(310, 135, 475, 185, F("Option 4"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 6:
        drawRoundBtn(140, 190, 305, 240, F(""), menuBackground, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 7:
        drawRoundBtn(310, 190, 475, 240, F(""), menuBackground, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 8:
        drawRoundBtn(140, 245, 305, 295, F(""), menuBackground, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 9:
        drawRoundBtn(310, 245, 475, 295, F(""), menuBackground, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 10:
        drawSquareBtn(150, 301, 479, 319, VERSION, themeBackground, themeBackground, menuBtnColor, CENTER);
        break;
    case 11:
        return true;
        break;
    }
    return false;
}


/*=========================================================
    Page2
===========================================================*/
// Draws page 2 one button at a time to avoid blocking background processes
bool drawPage2()
{
    switch (graphicLoaderState)
    {
    case 0:
        break;
    case 1:
        drawSquareBtn(131, 55, 479, 319, "", themeBackground, themeBackground, themeBackground, CENTER);
        break;
    case 2:
        drawRoundBtn(140, 80, 305, 130, F("Option 1"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 3:
        drawRoundBtn(310, 80, 475, 130, F("Option 2"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 4:
        drawRoundBtn(140, 135, 305, 185, F("Option 3"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 5:
        drawRoundBtn(310, 135, 475, 185, F("Option 4"), menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 6:
        drawRoundBtn(140, 190, 305, 240, F(""), menuBackground, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 7:
        drawRoundBtn(310, 190, 475, 240, F(""), menuBackground, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 8:
        drawRoundBtn(140, 245, 305, 295, F(""), menuBackground, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 9:
        drawRoundBtn(310, 245, 475, 295, F(""), menuBackground, menuBtnBorder, menuBtnText, CENTER);
        break;
    case 10:
        drawSquareBtn(150, 301, 479, 319, VERSION, themeBackground, themeBackground, menuBtnColor, CENTER);
        break;
    case 11:
        return true;
        break;
    }
    return false;
}

// Button functions for config page
void page2Buttons()
{
    // Touch screen controls
    if (Touch_getXY())
    {
        if ((y >= 60) && (y <= 100))
        {
            if ((x >= 150) && (x <= 300))
            {
                waitForIt(150, 60, 300, 100);
                // Function here
            }
            if ((x >= 310) && (x <= 460))
            {
                waitForIt(310, 60, 460, 100);
                // Function here
            }
        }
        if ((y >= 110) && (y <= 150))
        {
            if ((x >= 150) && (x <= 300))
            {
                waitForIt(150, 110, 300, 150);
                // Function here
            }
            if ((x >= 310) && (x <= 460))
            {
                waitForIt(310, 110, 460, 150);
                // Function here
            }
        }
    }
}
