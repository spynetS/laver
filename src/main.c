#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "./lib/printer/src/printer.h"
#include "./ball.h"
#include <math.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define THRESHOLD 0.8f

#define NUM_BALLS 6
#define NUM_THREADS 8

char *unicode1[] = {"░", "▒", "▓", "█"};

int WIDTH = 60;
int HEIGHT = 60;




int brightness(int red, int green, int blue) {
  // 0.299 r + 0.587 g + 0.114 b
  // return red;
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
				float c = fminf(maxF / balls[0]->r, 1.0f);  // local center intensity

				int r = 100 + (int)(155 * c);  // emphasize center
				int g = 100 + (int)(155 * t);  // overall blending
				int b = 255;                    // keep bluish

				char *blaa = malloc(sizeof(char)*100);
				
				sprintf(blaa,"\033[38;2;%d;%d;%dm%s%s",r,g,b,
								unicode1[(brightness(r,g,b) * (4 - 1) / 255)],
								unicode1[(brightness(r,g,b) * (4 - 1) / 255)]);


				setPixel(canvas,x,y,blaa,TRANS,TRANS);
				//				setCharAt(x,y,blaa);

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
	//srand(100);

	WIDTH = termWidth()/2-1;
	HEIGHT = termHeight()-1;
		
	Canvas *canvas = newCanvas(WIDTH, HEIGHT,"  ",RED, WHITE);


	Ball* balls[NUM_BALLS];
	for(int i = 0; i < NUM_BALLS; i ++){
		
		balls[i] = new_ball(rand() % WIDTH,rand() % HEIGHT, rand() % HEIGHT/4);
	}
	int running = 1;
	while(running){
		clearPixels(canvas);

		if(kbhit() == 1){
			switch((char)getchar()){
			case 'q':
				running = 0;
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
		
		
		//update(canvas,balls,NUM_BALLS);
		for(int i = 0; i < NUM_BALLS; i ++){
			//draw_ball(canvas, balls[i]);
			update_ball(balls[i], WIDTH,HEIGHT);
		}
		
		draw(canvas);
		msleep(20);
	}
	// TODO free the BALLS


	return 0;
}
