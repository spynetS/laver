#include "./ball.h"
#include "../lib/printer/src/printer.h"
#include <stdlib.h>
#include <stdio.h>



void update_ball(Ball *ball, int width, int height) {

	if (ball->x >= width){
		ball->vx = -ball->vx;
	}
	if (ball->x < 0){
		ball->vx = -ball->vx;
	}
	if (ball->y >= height){
		ball->vy = -ball->vy;
	}
	if (ball->y < 0){
		ball->vy = -ball->vy;
	}
	
	ball->x += ball->vx;
	ball->y += ball->vy;
		
}


void draw_ball(Canvas* canvas, Ball *ball) {
	int r = ball->r/2;
	int x = r;
	int y = 0;
	int decision = 1 - r;

	while (x >= y) {
		setPixel(canvas,ball->x + x, ball->y + y, "@", RED, RED);
		setPixel(canvas,ball->x - x, ball->y + y, "@", RED, RED);
		setPixel(canvas,ball->x + x, ball->y - y, "@", RED, RED);
		setPixel(canvas,ball->x - x, ball->y - y, "@", RED, RED);
		setPixel(canvas,ball->x + y, ball->y + x, "@", RED, RED);
		setPixel(canvas,ball->x - y, ball->y + x, "@", RED, RED);
		setPixel(canvas,ball->x + y, ball->y - x, "@", RED, RED);
		setPixel(canvas,ball->x - y, ball->y - x, "@", RED, RED);
		
		y++;

		if (decision <= 0) {
			decision += 2*y + 1;
		} else {
			x--;
			decision += 2*(y - x) + 1;
		}
	}
	
}


Ball* new_ball(int x, int y, int r){
	Ball *ball = malloc(sizeof(Ball));
	ball->x = x;
	ball->y = y;


	float a = 1;
	ball->vx = ((float)rand()/(float)(RAND_MAX)) * a;
	ball->vy = ((float)rand()/(float)(RAND_MAX)) * a;

	ball->r = r;
	return ball;
}
