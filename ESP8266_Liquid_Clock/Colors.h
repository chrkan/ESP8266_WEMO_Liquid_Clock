/******************************************************************************
Colors.h
******************************************************************************/

#ifndef COLORS_H
#define COLORS_H
#include <Arduino.h>

enum eColorChange : uint8_t
{
	COLORCHANGE_NO,
	COLORCHANGE_MOOD,
	COLORCHANGE_FIVE,
	COLORCHANGE_HOUR,
	COLORCHANGE_DAY,
	COLORCHANGE_COUNT = COLORCHANGE_DAY
};

enum eColor : uint8_t
{
	COLOR_WHITE,
	COLOR_RED,
	COLOR_RED_25,
	COLOR_RED_50,
	COLOR_ORANGE,
	COLOR_YELLOW,
	COLOR_YELLOW_25,
	COLOR_YELLOW_50,
	COLOR_GREENYELLOW,
	COLOR_GREEN,
	COLOR_GREEN_25,
	COLOR_GREEN_50,
	COLOR_MINTGREEN,
	COLOR_CYAN,
	COLOR_CYAN_25,
	COLOR_CYAN_50,
	COLOR_LIGHTBLUE,
	COLOR_BLUE,
	COLOR_BLUE_25,
	COLOR_BLUE_50,
	COLOR_VIOLET,
	COLOR_MAGENTA,
	COLOR_MAGENTA_25,
	COLOR_MAGENTA_50,
	COLOR_PINK,
  COLOR_DARK_Yellow,
	COLOR_COUNT = COLOR_DARK_Yellow
};

const char sColorStr[][15] PROGMEM =
{
	"White",
	"Red",
	"Red 75%",
	"Red 50%",
	"Orange",
	"Yellow",
	"Yellow 75%",
	"Yellow 50%",
	"Green-Yellow",
	"Green",
	"Green 75%",
	"Green 50%",
	"Mintgreen",
	"Cyan",
	"Cyan 75%",
	"Cyan 50%",
	"Light Blue",
	"Blue",
	"Blue 75%",
	"Blue 50%",
	"Purple",
	"Magenta",
	"Magenta 75%",
	"Magenta 50%",
	"Pink",
  "Dark Yellow",
};

struct color_s
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

const color_s defaultColors[] =
{
	{ 0xFF, 0xFF, 0xFF }, // 00 WHITE

	{ 0xFF, 0x00, 0x00 }, // 01 RED
	{ 0xAF, 0x08, 0x08 }, // 02 RED_25
	{ 0x8F, 0x10, 0x10 }, // 03 RED_50

	{ 0xDF, 0x20, 0x00 }, // 04 ORANGE

	{ 0xFF, 0xBF, 0x00 }, // 05 YELLOW
	{ 0xFF, 0xFF, 0x40 }, // 06 YELLOW_25
	{ 0xFF, 0xFF, 0x80 }, // 07 YELLOW_50

	{ 0x7F, 0xFF, 0x00 }, // 08 GREENYELLOW

	{ 0x00, 0xFF, 0x00 }, // 09 GREEN
	{ 0x40, 0xFF, 0x40 }, // 10 GREEN_25
	{ 0x80, 0xFF, 0x80 }, // 11 GREEN_50

	{ 0x00, 0xFF, 0x7F }, // 12 MINTGREEN

	{ 0x00, 0xFF, 0xFF }, // 13 CYAN
	{ 0x00, 0x9F, 0x9F }, // 14 CYAN_25
	{ 0x80, 0xFF, 0xFF }, // 15 CYAN_50

	{ 0x00, 0x7F, 0xFF }, // 16 LIGHTBLUE

	{ 0x00, 0x00, 0xFF }, // 17 BLUE
	{ 0x00, 0x10, 0xAF }, // 18 BLUE_25
	{ 0x10, 0x10, 0x8F }, // 19 BLUE_50

	{ 0x7F, 0x00, 0xFF }, // 20 VIOLET

	{ 0xFF, 0x00, 0xFF }, // 21 MAGENTA
	{ 0x7F, 0x00, 0xAF }, // 22 MAGENTA_25
	{ 0xCF, 0x00, 0xCF }, // 23 MAGENTA_50

	{ 0xFF, 0x00, 0x7F },  // 24 PINK

  { 0x12, 0x12, 0x00 }   //25 Dark Yellow
} ;

static uint32_t getDefaultColor(uint8_t colorNum){
	return ((defaultColors[colorNum].red << 16) , (defaultColors[colorNum].green << 8) , defaultColors[colorNum].blue);
};

#endif
