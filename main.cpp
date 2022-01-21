
#include "libs/surface.h"
#include <stdlib.h>

#if _WIN32
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(time) usleep(time * 1000)
#endif

#include <time.h>

typedef struct _ConsoleSize
{
    int width;
    int height;
} CONSOLESIZE;

#if _WIN32
void GetConsoleSize(CONSOLESIZE *pConsize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    pConsize->width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    pConsize->height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
#else
#include <sys/ioctl.h>
void GetConsoleSize(CONSOLESIZE *pConsize)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    pConsize->width  = w.ws_col;
    pConsize->height = w.ws_row;
}
#endif

int main (int argc, char* argv[])
{
    CONSOLESIZE consize;
    GetConsoleSize(&consize);

    int SCREENHEIGHT = consize.height;
    int SCREENWIDTH  = consize.width;

    if (argc >= 3)
    {
        SCREENHEIGHT = atoi(argv[2]);
    }
    if (argc >= 2)
    {
        SCREENWIDTH = atoi(argv[1]);
    }

#if _WIN32
    system("cls");
#else
    system("clear");
#endif

    srand((unsigned int)time(NULL));

    Surface screen(SCREENWIDTH, SCREENHEIGHT);

    int white_char_positions[SCREENWIDTH][2];
    int ticks_from_white[SCREENWIDTH][SCREENHEIGHT];
    int line_heights[SCREENWIDTH];

    for (int i = 0; i < SCREENWIDTH; i++)
    {
        white_char_positions[i][0] = i;
        white_char_positions[i][1] = -1;
        line_heights[i] = rand() % 20 + 5;
    }


    unsigned long tick = 0;
    while (1)
    {
        for (int i = 0; i < SCREENWIDTH; i++)
        {
            if (white_char_positions[i][1] == -1)
            {
                if (rand() % 10 == 0)
                {
                    white_char_positions[i][1] = 0;
                    line_heights[i] = rand() % 20 + 5;
                }
            }
            else
            {
                white_char_positions[i][1]++;

                char character = rand() % 0x5e + 0x21;
                screen.SetCharAt(white_char_positions[i][0], white_char_positions[i][1], character);
                screen.SetForegroundAt(white_char_positions[i][0], white_char_positions[i][1], 255, 255, 255);
                ticks_from_white[i][white_char_positions[i][1]] = 0;

                if (white_char_positions[i][1] == SCREENHEIGHT)
                {
                    white_char_positions[i][1] = -1;
                }
            }

            for (int j = 0; j < SCREENHEIGHT; j++)
            {
                if (ticks_from_white[i][j] == 1)
                {
                    screen.SetForegroundAt(i, j, 0, 255, 0);
                }
                else if (ticks_from_white[i][j] >= line_heights[i])
                {
                    screen.SetForegroundAt(i, j, 0, 0, 0);
                }
                ticks_from_white[i][j]++;
            }
        }
        if (tick >= (SCREENHEIGHT * 10))
        {
            screen.Print();
            Sleep(50);
        }
        tick++;
    }
}

