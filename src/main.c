#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "./lib/printer/src/printer.h"
#include "./ball.h"
#include <math.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define THRESHOLD 0.9f

#define NUM_THREADS 12

int WIDTH = 60;
int HEIGHT = 60;
int NUM_BALLS = 6;

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
					
				F += (balls[i]->r * balls[i]->r)/(d2 + 0.0001f);
				if(F > maxF) maxF = F;  // track the strongest contribution
			}

			if (F > THRESHOLD/4){

				float t = fminf(F / THRESHOLD, 1.0f);     // overall blob intensity
				float c = 0.5f;//fminf(maxF / balls[0]->r, 1.0f);  // local center intensity

				int r = 100 + (int)(155 * t);  // emphasize center
				int g = 100 + (int)(155 * t);  // overall blending
				int b = 255;                    // keep bluish

				set_pixel(canvas,x,y,r,g,b);
			}
		}
	}
}

typedef struct {
	int start;
	int end;
	Canvas* canvas;
	Ball** balls;
	int num_balls;
} Argument ;

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
		//clearPixels(canvas);

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
		for(int i = 0; i < NUM_THREADS; i ++){
			pthread_join(threads[i], NULL);
		}
		
		
		for(int i = 0; i < NUM_BALLS; i ++){
			update_ball(balls[i], WIDTH,HEIGHT);
		}
		
		draw(canvas);
	 	msleep(20);
	}
	// TODO free the BALLS
	for(int i = 0; i < NUM_BALLS; i ++){
		free(balls[i]);
	}
	free_canvas(canvas);

	return 0;
}
