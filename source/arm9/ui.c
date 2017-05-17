#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "types.h"
#include "util.h"
#include "hid.h"
#include "arm9/timer.h"
#include "arm9/main.h"
#include "arm9/ui.h"
#include "arm9/console.h"
#include "banner_ppm_bin.h"


static u8 randomColor;
static bool verbose = false;

static const void *bannerData = banner_ppm_bin;


static void uiGetPPMInfo(const u8 *data, unsigned *width, unsigned *height);
static void uiDrawPPM(unsigned start_x, unsigned start_y, const u8 *data);

static void consoleMainInit()
{
	/* Initialize console for both screens using the two different PrintConsole we have defined */
	consoleInit(SCREEN_TOP, &con_top);
	consoleSetWindow(&con_top, 1, 1, con_top.windowWidth - 2, con_top.windowHeight - 2);
	
	// randomize color
	randomColor = (rng_get_byte() % 6) + 1;
	
	consoleInit(SCREEN_LOW, &con_bottom);
	
	consoleSelect(&con_top);
}

void uiDrawSplashScreen()
{
	unsigned width, height;
	
	uiGetPPMInfo(bannerData, &width, &height);
	// centered draw
	uiDrawPPM((SCREEN_WIDTH_TOP - width) / 2, (SCREEN_HEIGHT_TOP - height) / 2, bannerData);
}

void uiInit()
{
	consoleMainInit();
}

void uiDrawConsoleWindow()
{
	drawConsoleWindow(&con_top, 2, randomColor);
}

static void clearConsoles()
{
	consoleSelect(&con_bottom);
	consoleClear();
	consoleSelect(&con_top);
	consoleClear();
}

void uiClearConsoles()
{
	clearConsoles();
}

void clearConsole(int which)
{
	if(which)
		consoleSelect(&con_top);
	else
		consoleSelect(&con_bottom);
		
	consoleClear();
}

void uiSetVerboseMode(bool verb)
{
	verbose = verb;
}

bool uiGetVerboseMode()
{
	return verbose;
}

/* TODO: Should look similar to uiShowMessageWindow */
bool uiDialogYesNo(const char *textYes, const char *textNo, const char *const format, ...)
{
	char tmp[256];
	u32 keys;

	va_list args;
	va_start(args, format);
	vsnprintf(tmp, 256, format, args);
	va_end(args);
	
	/* Print dialog */
	uiPrintInfo("\n%s\n", tmp);
	uiPrintInfo("\t(A): %s\n\t(B): %s\n", textYes, textNo);
	
	/* Get user input */
	do {
		hidScanInput();
		keys = hidKeysDown() & HID_KEY_MASK_ALL;
		if(keys == KEY_A)
			return true;
		if(keys & KEY_B)
			return false;
	} while(1);
}

void uiPrintIfVerbose(const char *const format, ...)
{
	if(verbose)
	{
		char tmp[256];

		va_list args;
		va_start(args, format);
		vsnprintf(tmp, 256, format, args);
		va_end(args);
		
		printf(tmp);
	}
}

/* Prints a given text at the current position */
void uiPrint(const char *const format, unsigned int color, bool centered, ...)
{
	const unsigned int width = (unsigned int)consoleGet()->consoleWidth;
	char tmp[width + 1];

	va_list args;
	va_start(args, centered);
	vsnprintf(tmp, width + 1, format, args);
	va_end(args);

	if(centered)
	{
		size_t len = strlen(tmp);
		printf("\x1B[%um%*s%s\x1B[0m", color, (width - len) / 2, "", tmp);
	}
	else
	{
		printf("\x1B[%um%s\x1B[0m", color, tmp);
	}
}

void uiPrintCenteredInLine(unsigned int y, const char *const format, ...)
{
	const unsigned int width = (unsigned int)consoleGet()->consoleWidth;
	char tmp[width + 1];

	va_list args;
	va_start(args, format);
	vsnprintf(tmp, width + 1, format, args);
	va_end(args);

	size_t len = strlen(tmp);
	printf("\x1b[%u;%uH", y, consoleGet()->cursorX);
	printf("%*s%s\n", (width - len) / 2, "", tmp);
}

/* Prints a given text at a certain position in the current window */
void uiPrintTextAt(unsigned int x, unsigned int y, const char *const format, ...)
{
	char tmp[256];

	va_list args;
	va_start(args, format);
	vsnprintf(tmp, 256, format, args);
	va_end(args);

	printf("\x1b[%u;%uH%s", y, x, tmp);
}

/* Prints a given text surrounded by a graphical window */
/* centered in the middle of the screen. */
/* Waits for the user to press any button, after that the */
/* original framebuffer gets restored */
/*void uiShowMessageWindow(const char *const format, int screen, unsigned int x,
                         unsigned int y, bool centered, ...)
{
	char tmp[256];

	va_list args;
	va_start(args, centered);
	vsnprintf(tmp, 256, format, args);
	va_end(args);

	char *ptr = tmp;
	unsigned int lines = 3, longestLine = 1, curLen = 1;
	while(*ptr)
	{
		switch(*ptr)
		{
			case '\n':
				lines++;
				curLen = 1;
				break;
			default:
				curLen++;
		}
		if(curLen > longestLine) longestLine = curLen;

		ptr++;
	}

	PrintConsole *prevCon = consoleGet();
	PrintConsole windowCon;
	consoleInit(screen, &windowCon);

	if(centered)
	{
		x = (windowCon.windowWidth / 2) - (longestLine / 2);
		y = (windowCon.windowHeight / 2) - (lines / 2);
	}
	consoleSetWindow(&windowCon, x, y, longestLine, lines);

	u8 *fbBackup = (u8*)malloc(screen ? SCREEN_HEIGHT_TOP * SCREEN_WIDTH_TOP * 2 :
	                                    SCREEN_HEIGHT_SUB * SCREEN_WIDTH_SUB * 2);
	if(fbBackup)
	{
		// TODO: Optimize to only backup what will be overwritten. I'm lazy.
		memcpy(fbBackup, windowCon.frameBuffer, screen ? SCREEN_HEIGHT_TOP * SCREEN_WIDTH_TOP * 2 :
		                                                 SCREEN_HEIGHT_SUB * SCREEN_WIDTH_SUB * 2);

		printf("\x1B[30m\x1B[47m\x1B[2J");
		printf("%s\x1b[%u;%uHOK", tmp, lines - 1, (longestLine - 2) / 2);
		do
		{
			hidScanInput();
		} while(!hidKeysDown());

		memcpy(windowCon.frameBuffer, fbBackup, screen ? SCREEN_HEIGHT_TOP * SCREEN_WIDTH_TOP * 2 :
		                                                 SCREEN_HEIGHT_SUB * SCREEN_WIDTH_SUB * 2);
	}
	free(fbBackup); // free() checks for NULL

	consoleSelect(prevCon);
}*/


void uiPrintProgressBar(unsigned int x, unsigned int y, unsigned int w,
                        unsigned int h, unsigned int cur, unsigned int max)
{
	u16 *fb = consoleGet()->frameBuffer;
	const u16 color = consoleGetFgColor();


	for(u32 xx = x + 1; xx < x + w - 1; xx++)
	{
		fb[xx * SCREEN_HEIGHT_TOP + y] = color;
		fb[xx * SCREEN_HEIGHT_TOP + y + h - 1] = color;
	}

	for(u32 yy = y; yy < y + h; yy++)
	{
		fb[x * SCREEN_HEIGHT_TOP + yy] = color;
		fb[(x + w - 1) * SCREEN_HEIGHT_TOP + yy] = color;
	}

	for(u32 xx = x + 2; xx < (u32)(((float)(x + w - 2) / max) * cur); xx++)
	{
		for(u32 yy = y + 2; yy < y + h - 2; yy++)
		{
			fb[xx * SCREEN_HEIGHT_TOP + yy] = color;
		}
	}
}

static inline void drawPixel(unsigned x, unsigned y, u16 color)
{
	u16 *framebuf = (u16 *) FRAMEBUF_TOP_A_1;
	framebuf[x * SCREEN_HEIGHT_TOP + y] = color;
}

static void uiGetPPMInfo(const u8 *data, unsigned *width, unsigned *height)
{
	/* get image dimensions */
	const char *ptr = (const char *) data + 3;
		while(*ptr != 0x0A) ptr++;
	ptr++;
	
	sscanf(ptr, "%i %i", width, height);
}

static void uiDrawPPM(unsigned start_x, unsigned start_y, const u8 *data)
{
	unsigned width, height;
	
	/* get image dimensions */
	const char *ptr = (const char *) data + 3;
		while(*ptr != 0x0A) ptr++;
	ptr++;
	
	sscanf(ptr, "%i %i", &width, &height);
	
	const u8 *imagedata = data + 0x26;	// skip ppm header
	
	u16 color;
	
	for(unsigned x = 0; x < width; x++)
	{
		for(unsigned y = height; y > 0; y--)
		{
			const u8 *pixeldata = &imagedata[(x + (y-1) * width)*3];
			color = RGB8_to_565(pixeldata[2], pixeldata[0], pixeldata[1]);
			drawPixel(start_x + x, start_y + height - y, color);
		}
	}
}
