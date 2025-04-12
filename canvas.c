#include "canvas.h"
#include <stdio.h>
#include <stdlib.h>

bool initialized = 0;

void canvas_init(Canvas *canvas, int canvas_size, int window_size) {
	if (initialized) return;

    window_size = canvas_size * (window_size / canvas_size);

	{
		bool e = SDL_Init(SDL_INIT_VIDEO);
		if (!e) {
			puts("SDL_Init failed");
		}
	}
	{
		bool e = SDL_CreateWindowAndRenderer("canvas", window_size, window_size, 0, &canvas->window, &canvas->renderer);
		if (!e) {
			puts("SDL_CreateWindowAndRenderer failed");
		}
	}

	canvas->canvas = malloc(canvas_size * canvas_size * sizeof(int));
	for (int i = 0; i < canvas_size * canvas_size; i++) {
		canvas->canvas[i] = (((rand() % 256) << 8*2)
		                     | ((rand() % 256) << 8*1)
		                     | ((rand() % 256) << 8*0));
	}
	canvas->canvas_size = canvas_size;
	canvas->window_size = window_size;
    canvas->cursor.color = 0xFF00FF;
    canvas->cursor.x = 0;
    canvas->cursor.y = 0;

	initialized = 1;
}

void canvas_quit(Canvas *canvas) {
	if (!initialized) return;
	free(canvas->canvas); canvas->canvas = NULL;
	SDL_DestroyRenderer(canvas->renderer);
	SDL_DestroyWindow(canvas->window);
	SDL_Quit();
	initialized = 0;
}

/* Returns 0 on success, 1 if x is out of range, 2 if y is out of range, and 3 if both x and y are out of range. */
int canvas_setPosition(Canvas *canvas, int x, int y) {
	int e = 0;
	if (0 <= x && x < canvas->canvas_size) {
		canvas->cursor.x = x;
	}
	else {
		e |= 1;
	}
	if (0 <= y && y < canvas->canvas_size) {
		canvas->cursor.y = y;
	}
	else {
		e |= 2;
	}
	return e;
}

int canvas_up(Canvas *canvas) {
	return canvas_setPosition(canvas, canvas->cursor.x, canvas->cursor.y - 1);
}

int canvas_down(Canvas *canvas) {
	return canvas_setPosition(canvas, canvas->cursor.x, canvas->cursor.y + 1);
}

int canvas_left(Canvas *canvas) {
	return canvas_setPosition(canvas, canvas->cursor.x - 1, canvas->cursor.y);
}

int canvas_right(Canvas *canvas) {
	return canvas_setPosition(canvas, canvas->cursor.x + 1, canvas->cursor.y);
}

int canvas_setColor(Canvas *canvas, int color) {
	if (color > 0xFFFFFF || color < 0) return 1;
	canvas->cursor.color = color;
	return 0;
}

void canvas_plot(Canvas *canvas) {
	canvas->canvas[canvas->cursor.y * canvas->canvas_size + canvas->cursor.x] = canvas->cursor.color;
}

void canvas_render(Canvas *canvas) {
	int canvas_size = canvas->canvas_size;
	for (int y = 0; y < canvas_size; y++) {
		for (int x = 0; x < canvas_size; x++) {
			int color = canvas->canvas[canvas_size * y + x];
			int r = (color & 0xFF0000) >> 8*2;
			int g = (color & 0x00FF00) >> 8*1;
			int b = (color & 0x0000FF) >> 8*0;
			SDL_SetRenderDrawColor(canvas->renderer, r, g, b, 1);
			int rect_size = canvas->window_size / canvas->canvas_size;
			SDL_FRect rect = {.x = x * rect_size, .y = y * rect_size, .w = rect_size, .h = rect_size};
			SDL_RenderFillRect(canvas->renderer, &rect);
		}
	}
	SDL_RenderPresent(canvas->renderer);
}

#ifdef CANVAS_STANDALONE
int main() {
	Canvas canvas;
	canvas_init(&canvas, 16, 600);

	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 16; x++) {
			canvas_setPosition(&canvas, x, y);
			canvas_setColor(&canvas, 10 * (x ^ y));
			canvas_plot(&canvas);
		}
	}

	canvas_setPosition(&canvas, 8, 8);
	canvas_setColor(&canvas, 0x00FF00);
	canvas_plot(&canvas);
        
	canvas_up(&canvas);
	canvas_setColor(&canvas, 0xFF0000);
	canvas_plot(&canvas);
	canvas_down(&canvas);
        
	canvas_down(&canvas);
	canvas_setColor(&canvas, 0xFF0000);
	canvas_plot(&canvas);
	canvas_up(&canvas);

	canvas_left(&canvas);
	canvas_setColor(&canvas, 0xFF0000);
	canvas_plot(&canvas);
	canvas_right(&canvas);
        
	canvas_right(&canvas);
	canvas_setColor(&canvas, 0xFF0000);
	canvas_plot(&canvas);
	canvas_left(&canvas);

	while (1) {
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT) break;
		canvas_render(&canvas);
	}

	canvas_quit(&canvas);
}
#endif
