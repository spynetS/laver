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
	for(int y = 0; y < HEIGHT/4; y++){
		for(int x = start; x < end; x++){
			float F = 0;
			float maxF = 0.0f;
			for(int i = 0; i < balls_size; i ++) {

				float dx = x - balls[i]->x;
				float dy = y - balls[i]->y;
				float d2 = dx*dx + dy*dy;
					
				F += (balls[i]->r * balls[i]->r)/(d2 + 0.0001f);
				if(F > maxF) maxF = F;  // track the strongest contribution
			}

			if(y >= HEIGHT-2){
				F+= 0.02f;
			}
			if(y < 2){
				F+= 0.02f;
			}

			if(x >= WIDTH-2){
				F+= 0.02f;
			}
			if(x < 2){
				F+= 0.02f;
			}

			if (F > THRESHOLD/4){

				float t = fminf(F / THRESHOLD, 1.0f);
				float c = 0.5f;

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
	}
}



void* bla(void* arg) {
	Argument *argument = (Argument*)arg;
	update(argument->canvas,
				 argument->balls,
				 argument->num_balls,
				 argument->start,
				 argument->end);
}


int main(){
	srand(time(NULL));

	WIDTH = termWidth()-1;
	HEIGHT = termHeight()-1;
		
	Canvas *canvas = new_canvas(WIDTH, HEIGHT);
	Ball* balls[NUM_BALLS*10];

	disableEcho();
	printf(HIDE_CURSOR);
	
	for(int i = 0; i < NUM_BALLS; i ++){
		balls[i] = new_ball(rand() % WIDTH,rand() % HEIGHT, rand() % HEIGHT/4);
	}
	
	int running = 1;
	while(running){


		if(kbhit() == 1){
			switch((char)getchar()){
			case 'q':
				running = 0;
				break;
			case 'm':
				balls[NUM_BALLS++] = new_ball(rand() % WIDTH,rand() % HEIGHT, rand() % HEIGHT/4);
				break;
			case 'l':
				if(NUM_BALLS > 0 ){
					free(balls[NUM_BALLS-1]);
					NUM_BALLS--;
				}
				break;
			case 'f':
				speed(balls, NUM_BALLS, 1.2);
				break;
			case 's':
				speed(balls, NUM_BALLS, 0.8);
				break;
			case '+':
			case '=':
				size(balls, NUM_BALLS, 1.2);
				break;
			case '-':
				size(balls, NUM_BALLS, 0.8);
				break;
			}
		}

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
			update_ball(balls[i], WIDTH,HEIGHT, 0, 0.0f);
		
		draw(canvas);
	 	msleep(20);
	}

	for(int i = 0; i < NUM_BALLS; i ++){
		free(balls[i]);
	}
	free_canvas(canvas);
	return 0;
}
