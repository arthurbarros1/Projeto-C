#ifndef SCREEN_H
#define SCREEN_H

#define CYAN 36
#define YELLOW 33
#define DARKGRAY 90
#define GREEN 32
#define RED 31

#define MAXX 80
#define MAXY 25
#define MINX 0
#define MINY 0

void screenInit(int enableColor);
void screenSetColor(int fg, int bg);
void screenGotoxy(int x, int y);
void screenClear();
void screenUpdate();
void screenDestroy();

#endif // SCREEN_H
