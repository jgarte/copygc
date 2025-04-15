#pragma once
#include <SDL3/SDL.h>

typedef struct {
    // The coordinates of the virtual pixel to paint.
	int x;
	int y;
    // The color to paint the virtual pixel.
	int color;
} Cursor;
        
typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	int canvas_size;
	int window_size;
    // The array of virtual pixels. It has length `canvas_size * canvas_size`.
	int *canvas;
	Cursor cursor;
} Canvas;



/* Creates a window that is `window_size` pixels wide and tall and has a canvas
 * that is `canvas_size` virtual pixels wide and tall. Each virtual pixel should
 * be made of multiple real pixels. */
void canvas_init(Canvas *canvas, int canvas_size, int window_size);
/* Call before exit if a `Canvas` object exists. */
void canvas_quit(Canvas *canvas);

/* Set the position of the cursor. Returns 0 on success, 1 if x is out of range,
 * 2 if y is out of range, and 3 if both x and y are out of range. If x is valid
 * but y is invalid, then the x position is set. If y is valid but x is invalid,
 * then the y position is set. */
int canvas_setPosition(Canvas *canvas, int x, int y);
/* Move the cursor up if possible. Returns 0 if the movement succeeded. */
int canvas_up(Canvas *canvas);
/* Move the cursor down if possible. Returns 0 if the movement succeeded. */
int canvas_down(Canvas *canvas);
/* Move the cursor left if possible. Returns 0 if the movement succeeded. */
int canvas_left(Canvas *canvas);
/* Move the cursor right if possible. Returns 0 if the movement succeeded. */
int canvas_right(Canvas *canvas);
/* Set the brush color. `color` is a 24-bit integer. The leftmost 8 bits of that
 * integer is the red value, the middle 8 bits is the green value, and the
 * rightmost 8 bits is the blue value. If the number is greater than 0xFFFFFF or
 * less than 0, the color is not set and 1 is returned. The function returns 0
 * on success. */
int canvas_setColor(Canvas *canvas, int color);
/* Once the color and position has been set, call `canvas_plot` to paint the
 * virtual pixel. */
void canvas_plot(Canvas *canvas);
/* Render the current canvas. This should be called in a loop so that the canvas
 * is redrawn even when moving the window. */
void canvas_render(Canvas *canvas);
