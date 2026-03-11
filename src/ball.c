#include "./ball.h"
#include "../lib/printer/src/printer.h"
#include <stdlib.h>
#include <stdio.h>

void update_ball(Ball *ball, int width, int height, float gx, float gy) {
	ball->vx += gx;
	ball->vy += gy;
	if (ball->x >= width){
		ball->vx = -ball->vx+(gx*1.01f);
	}
	if (ball->x < 0){
		ball->vx = -ball->vx+(gx*1.01f);
	}
	if (ball->y >= height){
		ball->vy = -ball->vy+(gy*1.01f);
	}
	if (ball->y < 0){
		ball->vy = -ball->vy+(gy*1.11f);
	}

	ball->x += ball->vx;
	ball->y += ball->vy;
}

Ball* new_ball(int x, int y, int r){
	Ball *ball = malloc(sizeof(Ball));
	ball->x = x;
	ball->y = y;


	float a = 1.0f;
	ball->vx = ((float)rand()/(float)(RAND_MAX)) * a;
	ball->vy = ((float)rand()/(float)(RAND_MAX)) * a;

	ball->r = r;
	return ball;
}
