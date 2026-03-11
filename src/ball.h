#ifndef _BALL_
#define _BALL_
#include "../lib/printer/src/printer.h"


typedef struct ball{
	float x;
	float y;

	float vx;
	float vy;

	float r;
} Ball;

float get_field_value(Ball *ball);
void update_ball(Ball *ball, int width, int height, float gx, float gy);
void draw_ball(Canvas *canvas, Ball *ball);
Ball* new_ball(int x, int y, int r);

#endif
