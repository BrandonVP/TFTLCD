/*
 Name:		TFTLCD.ino
 Created:	1/16/2021 2:59:46 PM
 Author:	Brandon Van Pelt
*/

// the setup function runs once when you press reset or power the board
#include <UTFT.h>
#include <memorysaver.h>
#include <UTouchCD.h>
#include <UTouch.h>


// Global LCD theme color variables
#define themeBackground 0xFFFF // White
#define menuBtnText 0xFFFF // White
#define menuBtnBorder 0x0000 // Black
#define menuBtnColor 0xFC00 // Orange
#define menuBackground 0xC618 //Silver

// For the draw shape functions
#define LEFT 1
#define CENTER 2
#define RIGHT 3

// Initialize display
//(byte model, int RS, int WR, int CS, int RST, int SER)
UTFT myGLCD(ILI9488_16, 7, 38, 9, 10);
//RTP: byte tclk, byte tcs, byte din, byte dout, byte irq
UTouch  myTouch(2, 6, 3, 4, 5);

// For touch controls
int x, y;

// External import for fonts
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// Keeps track of current page
uint8_t controlPage = 1;

// Draws the menu 1 page
void drawPage1()
{
    drawSquareBtn(141, 1, 478, 319, "", themeBackground, themeBackground, themeBackground, CENTER);
    drawSquareBtn(180, 10, 400, 45, "Page 1", themeBackground, themeBackground, menuBtnColor, CENTER);
}

// Draws the menu 2 page
void drawPage2()
{
    drawSquareBtn(141, 1, 478, 319, "", themeBackground, themeBackground, themeBackground, CENTER);
    drawSquareBtn(180, 10, 400, 45, "Page 2", themeBackground, themeBackground, menuBtnColor, CENTER);
    drawRoundBtn(150, 60, 300, 100, "Option 1", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(310, 60, 460, 100, "Option 2", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(150, 110, 300, 150, "Option 3", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(310, 110, 460, 150, "Option 4", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
}

// Button functions for config page
void page2Buttons()
{
    // Touch screen controls
    if (myTouch.dataAvailable())
    {
        myTouch.read();
        x = myTouch.getX();
        y = myTouch.getY();

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
    while (myTouch.dataAvailable())
        myTouch.read();
    myGLCD.setColor(menuBtnBorder);
    myGLCD.drawRoundRect(x1, y1, x2, y2);
}

// Highlights square buttons when selected
void waitForItRect(int x1, int y1, int x2, int y2)
{
    myGLCD.setColor(themeBackground);
    myGLCD.drawRect(x1, y1, x2, y2);
    while (myTouch.dataAvailable())
        myTouch.read();
    myGLCD.setColor(menuBtnBorder);
    myGLCD.drawRect(x1, y1, x2, y2);
}

// Page control framework
void pageControl(int page, bool value = false)
{
    // Static bool ensures the page is drawn only once while the loop is running
    static bool hasDrawn;

    // Seperated because compiler produces error with 1 line
    // Call function with true value if you don't want a page to be drawn when called
    hasDrawn = value;

    while (true)
    {
        // Check if button on menu is pushed
        menuButtons();

        // Switch which page to load
        switch (page)
        {
        case 1: 
            if (!hasDrawn)
            {
                // Call draw page here
                drawPage1();
                hasDrawn = true;
                controlPage = page;
            }
            // Call buttons if any here

            break;
        case 2:
            if (!hasDrawn)
            {
                // Call draw page here
                drawPage2();
                hasDrawn = true;
                controlPage = page;
            }
            // Call buttons if any here
            page2Buttons();
            break;
        case 3:
            if (!hasDrawn)
            {
                // Call draw page here

                hasDrawn = true;
                controlPage = page;
            }
            // Call buttons if any here

            break;
        case 4:
            if (!hasDrawn)
            {
                // Call draw page here

                hasDrawn = true;
                controlPage = page;
            }
            // Call buttons if any here

            break;
        case 5:
            if (!hasDrawn)
            {
                // Call draw page here

                hasDrawn = true;
                controlPage = page;
            }
            // Call buttons if any here

            break;
        }
    }
}

// Setup TFT 
void setup()
{
    Serial.begin(115200);
    myGLCD.InitLCD();
    myGLCD.clrScr();

    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);

    myGLCD.setFont(BigFont);
    myGLCD.setBackColor(0, 0, 255);

    drawMenu();
}

// Draw the side menu
void drawMenu()
{
    // Draw Layout
    drawSquareBtn(1, 1, 478, 319, "", themeBackground, themeBackground, themeBackground, CENTER);
    drawSquareBtn(1, 1, 140, 319, "", menuBackground, menuBackground, menuBackground, CENTER);

    // Draw Menu Buttons
    drawRoundBtn(10, 10, 130, 65, "Menu 1", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(10, 70, 130, 125, "Menu 2", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(10, 130, 130, 185, "Menu 3", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(10, 190, 130, 245, "Menu 4", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
    drawRoundBtn(10, 250, 130, 305, "Menu 5", menuBtnColor, menuBtnBorder, menuBtnText, CENTER);
}

// Buttons for the main menu
void menuButtons()
{
    while (true)
    {
        // Touch screen controls
        if (myTouch.dataAvailable())
        {
            myTouch.read();
            x = myTouch.getX();
            y = myTouch.getY();

            // Menu
            if ((x >= 10) && (x <= 130))  // Button: 1
            {
                if ((y >= 10) && (y <= 65))  // Upper row
                {
                    waitForIt(10, 10, 130, 65);
                    pageControl(1);
                }
                if ((y >= 70) && (y <= 125))  // Upper row
                {

                    // X_Start, Y_Start, X_Stop, Y_Stop
                    waitForIt(10, 70, 130, 125);
                    pageControl(2);

                }
                if ((y >= 130) && (y <= 185))  // Upper row
                {
                    // X_Start, Y_Start, X_Stop, Y_Stop
                    waitForIt(10, 130, 130, 185);
                    pageControl(3);
                }
                // Settings touch button
                if ((y >= 190) && (y <= 245))
                {

                    // X_Start, Y_Start, X_Stop, Y_Stop
                    waitForIt(10, 190, 130, 245);
                    pageControl(4);
                }
                if ((y >= 250) && (y <= 305))
                {

                    // X_Start, Y_Start, X_Stop, Y_Stop
                    waitForIt(10, 250, 130, 305);
                    pageControl(5);
                }

            }
        }
        return;
    }
}

// the loop function runs over and over again until power down or reset
void loop() {
    pageControl(controlPage);
}
