#ifndef TPE_ARQUI_VIDEODRIVER_H
#define TPE_ARQUI_VIDEODRIVER_H
#include <stdint.h>

typedef struct Color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}Color;

#define WHITE (struct Color){255, 255, 255}
#define BLACK (struct Color){0, 0, 0}
#define RED (struct Color){255, 0, 0}
#define GREEN (struct Color){0, 255, 0}
#define BLUE (struct Color){0, 0, 255}
#define YELLOW (struct Color){255, 255, 0}
#define PINK (struct Color){255, 0, 255}
#define CYAN (struct Color){0, 255, 255}

//DOUBLE BUFFERING OPTIONS
void enableDoubleBuffering();
void disableDoubleBuffering();
void drawBuffer();
void clearBuffer();

//PIXELS
void putPixel(Color c, uint32_t x, uint32_t y);
void putHexPixel(uint32_t hexColor, uint32_t x, uint32_t y);

//SHAPES
void putCharAt(uint32_t x, uint32_t y, char character);
void putColoredCharAt(Color c, uint32_t x, uint32_t y, char character);

void drawLine(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
void drawColoredLine(Color c, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
void drawHexLine(uint32_t hexColor, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

void drawEmptyColoredCircle(Color c, uint32_t x0, uint32_t y0, uint32_t radius);
void drawEmptyHexCircle(uint32_t hexColor, uint32_t x0, uint32_t y0, uint32_t radius);
void drawColoredCircle(Color c, uint32_t x0, uint32_t y0, uint32_t radius);
void drawHexCircle(uint32_t hexColor, uint32_t x0, uint32_t y0, uint32_t radius);

void drawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void drawEmptyColoredRectangle(Color c, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void drawEmptyHexRectangle(uint32_t hexColor, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void drawColoredRectangle(Color c, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void drawHexRectangle(uint32_t hexColor, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

//UTILS
void clearScreen();
void scrollCharArea();

uint32_t getXCharSlots();
uint32_t getYCharSlots();

#endif //TPE_ARQUI_VIDEODRIVER_H
