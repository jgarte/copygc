#pragma once
#include <SDL3/SDL.h>

typedef struct {
	int x;
	int y;
	int color;
} Cursor;
        
typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	int canvas_size;
	int window_size;
	int *canvas;
	Cursor cursor;
} Canvas;

void canvas_init(Canvas *canvas, int canvas_size, int window_size);
void canvas_quit(Canvas *canvas);

int canvas_setPosition(Canvas *canvas, int x, int y);
int canvas_up(Canvas *canvas);
int canvas_down(Canvas *canvas);
int canvas_left(Canvas *canvas);
int canvas_right(Canvas *canvas);
int canvas_setColor(Canvas *canvas, int color);
void canvas_plot(Canvas *canvas);
void canvas_render(Canvas *canvas);
