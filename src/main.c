#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "./lib/printer/src/printer.h"
#include "./ball.h"
#include <math.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define THRESHOLD 0.5f

#define NUM_BALLS 4

char *unicode1[] = {"░", "▒", "▓", "█"};

int WIDTH = 60;
int HEIGHT = 60;




int brightness(int red, int green, int blue) {
  // 0.299 r + 0.587 g + 0.114 b
  // return red;
  return (0.299 * red + green * 0.587 + blue * 0.114);
}

// TODO skip pixels that are really far from any ball
void update(Canvas* canvas, Ball** balls, int balls_size){
	for(int y = 0; y < HEIGHT; y++){
		for(int x = 0; x < WIDTH; x++){
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

			}
		}
	}

}

int main(){
	srand(time(NULL));

	WIDTH = termWidth()/2-1;
	HEIGHT = termHeight()-1;
		
	Canvas *canvas = newCanvas(WIDTH, HEIGHT,"  ",RED, WHITE);


	Ball* balls[NUM_BALLS];
	for(int i = 0; i < NUM_BALLS; i ++){
		
		balls[i] = new_ball(rand() % WIDTH,rand() % HEIGHT, rand() % HEIGHT/5);
	}
	
	while(1){
		clearPixels(canvas);
		
		update(canvas,balls,NUM_BALLS);
		for(int i = 0; i < NUM_BALLS; i ++){
			//draw_ball(canvas, balls[i]);
			update_ball(balls[i], WIDTH,HEIGHT);
		}
		

		//		drawBorder(canvas,1);
		draw(canvas);
		
		msleep(20);
	}
	
	return 0;
}
