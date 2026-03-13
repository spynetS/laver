#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "./lib/printer/src/printer.h"
#include "./ball.h"
#include <math.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define THRESHOLD 0.7f
#define NUM_THREADS 1

int WIDTH = 60;
int HEIGHT = 60;
int NUM_BALLS = 6;

int running = 1;
Ball** balls = NULL;

float gx = 0.0f;
float gy = 0.0f;

typedef struct {
	int start;
	int end;
	Canvas* canvas;
	Ball** balls;
	int num_balls;
} Argument ;



int brightness(int red, int green, int blue) {
  return (0.299 * red + green * 0.587 + blue * 0.114);
}

// TODO skip pixels that are really far from any ball
void update(Canvas* canvas, Ball** balls, int balls_size, int start, int end){
	for(int y = 0; y < HEIGHT; y++){
		for(int x = start; x < end; x++){
			float F = 0;
			float maxF = 0.0f;
			for(int i = 0; i < balls_size; i ++) {

				float dx = x - balls[i]->x;
				float dy = y - balls[i]->y;
				float d2 = dx*dx + dy*dy;

				float w = 1;
				
				if(y >= HEIGHT-1){
					w += 0.05f;
				}
				if(y < 1){
					w += 0.05f;
				}
				if(x >= WIDTH-1){
					w += 0.05f;
				}
				if(x < 1){
					w += 0.05f;
				}

				
				F += (balls[i]->r * balls[i]->r)/(d2 + 0.0001f)*w;
				if(F > maxF) maxF = F;  // track the strongest contribution
			}


			if (F > THRESHOLD/4){

				float t = fminf(F / THRESHOLD, 1.0f);

				int r = 180 + (int)(75 * t);
				int g = 50  + (int)(60 * t);
				int b = 200 + (int)(55 * t);
				set_pixel(canvas,x,y,r,g,b);
			}
		}
	}
}

void size(Ball** balls, int num_balls, float strength) {
	for(int i = 0; i < num_balls; i ++){
		balls[i]->r *= strength;
	}
}


void speed(Ball** balls, int num_balls, float strength) {
	for(int i = 0; i < num_balls; i ++){
		balls[i]->vx *= strength;
		balls[i]->vy *= strength;
		gx *= strength;
		gy *= strength;
	}
}

int radius = 1.0f;
double angle = 0;
void rotate_pivot() {
	gx = radius * cos(angle);
	gy = radius * sin(angle);

	angle++;
}


void input() {
	if(kbhit()) {
		int ch = getchar();
		if (ch == 27) {  // ESC detected
			int ch2 = getchar();
			int ch3 = getchar();
			if (ch2 == '[') {  // arrow key sequence
				switch(ch3){
				case 'C': gx += 0.01f; break; // Right
				case 'D': gx -= 0.01f; break; // Left
				case 'A': gy -= 0.01f; break; // Up
				case 'B': gy += 0.01f; break; // Down
				}
			}
		} else {
			switch(ch){
			case 'q': running = 0; break;
			case 'r': rotate_pivot(); break;
			case 'm':
				if(NUM_BALLS < NUM_BALLS*4){
					balls[NUM_BALLS++] = new_ball(rand() % WIDTH,
																				rand() % HEIGHT,
																				rand() % HEIGHT/4);
				}
				break;
			case 'l':
				if(NUM_BALLS > 0 && balls[NUM_BALLS-1]){
					free(balls[NUM_BALLS-1]);
					NUM_BALLS--;
				}
				break;
			case 'f': speed(balls, NUM_BALLS, 1.2); break;
			case 's': speed(balls, NUM_BALLS, 0.8); break;
			case '+':
			case '=': size(balls, NUM_BALLS, 1.2); break;
			case '-': size(balls, NUM_BALLS, 0.8); break;
			}
		}
	}
}

void* bla(void* arg) {
	Argument *argument = (Argument*)arg;
	update(argument->canvas,
				 argument->balls,
				 argument->num_balls,
				 argument->start,
				 argument->end);
	return NULL;
}


int main(){
	srand(time(NULL));

	WIDTH = termWidth()-1;
	HEIGHT = termHeight()-1;
		
	Canvas *canvas = new_canvas(WIDTH, HEIGHT);
	balls = malloc(sizeof(Ball*)*NUM_BALLS*4);

	disableEcho();
	printf(HIDE_CURSOR);
	
	for(int i = 0; i < NUM_BALLS; i ++){
		balls[i] = new_ball(rand() % WIDTH,rand() % HEIGHT, rand() % HEIGHT/4);
	}
	
	while(running){

		input();

		pthread_t threads[NUM_THREADS];
		Argument arguments[NUM_THREADS];
		for(int i = 0; i < NUM_THREADS; i ++) {
			arguments[i].canvas = canvas;
			arguments[i].balls = balls;
			arguments[i].balls = balls;
			arguments[i].num_balls = NUM_BALLS;
			arguments[i].start = i * (WIDTH/NUM_THREADS);
			arguments[i].end = (i+1) * (WIDTH/NUM_THREADS);
			
			pthread_create(&threads[i], NULL, bla, &arguments[i]);
		}
		for(int i = 0; i < NUM_THREADS; i ++)
			pthread_join(threads[i], NULL);
		
		for(int i = 0; i < NUM_BALLS; i ++)
			update_ball(balls[i], WIDTH,HEIGHT, 0.1f * gx, 0.1f * gy);

		double x = 5*gx+WIDTH/2;
		double y = 5*gy+HEIGHT/2;
		
		set_pixel(canvas, x, y, 255, 255, 255);
		draw(canvas);
		msleep(20);
	}

	for(int i = 0; i < NUM_BALLS; i ++){
		free(balls[i]);
	}
	free(balls);
	free_canvas(canvas);
	return 0;
}
