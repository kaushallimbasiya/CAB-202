#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <math.h>
#include <cpu_speed.h>
//#include <usb_serial.h>
#include <lcd.h>
#include <stdlib.h>
#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"

bool paused = false;
bool p = false;

#define FREQ 8000000.0
#define PRESCALE 1024.0
#define TIMER_SCALE 256.0

int sx; // X position of Starfighter
int sy; // Y position of Starfighter
int starFighterVelocity = 0; // Initially velocity of Starfighter

char buffer[20];

void draw_double(uint8_t x, uint8_t y, double value, colour_t colour) {
	snprintf(buffer, sizeof(buffer), "%f", value);
	draw_string(x, y, buffer, colour);
}

void draw_int(uint8_t x, uint8_t y, int value, colour_t colour) {
	snprintf(buffer, sizeof(buffer), "%d", value);
	draw_string(x, y, buffer, colour);
}

volatile int overflow_counter = 0;
ISR(TIMER0_OVF_vect) {
	overflow_counter++;
}

// Globally setting up Height and Width of Starfighter
#define STARFIGHTER_HEIGHT 6
#define STARFIGHTER_WIDTH 8

// Globally setting up Height and Width of Astroids
#define ASTROID_HEIGHT 7
#define ASTROID_WIDTH 7

#define ASTROID_MAX 3
#define BOULDER_MAX 3
#define FRAGMENTS_MAX 3


int astroid_x[ASTROID_MAX];
int astroid_y[ASTROID_MAX];
int bould_x[BOULDER_MAX];
int bould_y[BOULDER_MAX];
int frag_x[FRAGMENTS_MAX];
int frag_y[FRAGMENTS_MAX];


int lives = 5; // Deflector Shield Lives
int score = 0; // Score



// Draw Deflector Shield using draw_line function
void drawDeflectorShield() {
	for (int i = 0; i <= LCD_X; i++){
		draw_line(i, 39, i, 39, BG_COLOUR);
		i += 1;
	}
}

// Draw Starfighter using draw_line and draw_pixel functions from graphics.h library
void drawStarfighter(void) {
	draw_line(sx, sy, sx+1, sy, FG_COLOUR);
	draw_line(sx, sy+1, sx+1, sy+1, FG_COLOUR);
	draw_line(sx - 1, sy + 2, sx + 2, sy + 2, FG_COLOUR);
	draw_line(sx - 3, sy + 3, sx + 4, sy + 3, FG_COLOUR);
	draw_line(sx - 2, sy + 4, sx + 3, sy + 4, FG_COLOUR);
	draw_line(sx - 3, sy + 5, sx + 4, sy + 5, FG_COLOUR);
}
// Setup the position of the starfighter
void setupStarFighter(){
	sx = LCD_X / 2;
	sy = 42;

}

 
// Initial setup of the game
void setup( void ){
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	clear_screen();

	char *welcome = "n10032029";
	char *welcome1 = "Star Voyager!!";
	draw_string(0, 0, welcome, FG_COLOUR);
	draw_string(0, 10, welcome1, FG_COLOUR);
	show_screen();

	CLEAR_BIT( DDRD, 0 ); // right
	CLEAR_BIT( DDRD, 1 ); // up
	CLEAR_BIT( DDRB, 0 ); // centre
	CLEAR_BIT( DDRB, 1 ); // left
	CLEAR_BIT( DDRB, 7 ); // down
	CLEAR_BIT( DDRF, 5 ); // Right Button
	CLEAR_BIT( DDRF, 6 ); // Left Button
}




// Draw Astroids using draw_line and draw_pixel functions from graphics.h library
void drawAstroid(void) {
	for (int i = 0; i < ASTROID_MAX; i++){
		draw_pixel((astroid_x[i]), (astroid_y[i]), FG_COLOUR);
		draw_line((astroid_x[i] - 1), astroid_y[i] + 1, (astroid_x[i] + 1), astroid_y[i] + 1, FG_COLOUR);
		draw_line(astroid_x[i] - 2, astroid_y[i] + 2, astroid_x[i] + 2, astroid_y[i] + 2, FG_COLOUR);
		draw_line(astroid_x[i] - 3, astroid_y[i] + 3, astroid_x[i] + 3, astroid_y[i] + 3, FG_COLOUR);
		draw_line(astroid_x[i] - 2, astroid_y[i] + 4, astroid_x[i] + 2, astroid_y[i] + 4, FG_COLOUR);
		draw_line((astroid_x[i] - 1), astroid_y[i] + 5, (astroid_x[i] + 1), astroid_y[i] + 5, FG_COLOUR);
		draw_pixel((astroid_x[i]), (astroid_y[i]) + 6, FG_COLOUR);
	}
}


// Setup Astroids at random x position from 0 t0 LCD_X
void setup_astroid()
{
	for (int i = 0; i < ASTROID_MAX; i++){
		astroid_x[i] = rand() % (LCD_X - 7) - 0;
    	astroid_y[i] = -10 ;
	}
}

// Updates movement of astroids in vertical positive y direction
void update_astroid(){
	
	for(int i = 0; i < 3; i++){
		
		astroid_y[i] ++;

		if (astroid_y[i] >= LCD_Y - 15){
			astroid_x[i] = rand() % (LCD_X - 7);
			astroid_y[i] = -7;
			lives --;
		}
	}
}

// generates velocity to starfighter by updating x position of starfighter

void velocity(void) {
	if (starFighterVelocity == -1) {
		sx--;
	}
	else if(starFighterVelocity == 1) {
		sx ++;
	}
	if ( sx == LCD_X - 5 ) {
		starFighterVelocity = 0;
	}
	else if ( sx == 3 ) {
		starFighterVelocity = 0;
	}
}

// Setting BITS to move starfighter in left and right direction respectively

void updateStarFighter(void) {
	if ( BIT_IS_SET ( PINB, 1 ) ) {

		if ( starFighterVelocity == 0 ) {
			starFighterVelocity = -1;
		}
		if ( starFighterVelocity == 1 ) {
			starFighterVelocity = 0;
		}
	}
	if ( BIT_IS_SET ( PIND, 0 ) ) {
		if ( starFighterVelocity == 0 ) {
			starFighterVelocity = 1;
		}
		if ( starFighterVelocity == -1 ) {
			starFighterVelocity = 0;
		}
	}
} 

// This function is used to display game status
void gameStatus( void ) {
	double time = ( overflow_counter * 256.0 + TCNT0 ) * PRESCALE / FREQ;
	clear_screen();
	draw_string(3, 5, "Time: ", FG_COLOUR);
	draw_int(35, 5, round(time), FG_COLOUR);
	draw_string(3, 15, "Lives:   ", FG_COLOUR);
	draw_int(35, 15, lives, FG_COLOUR);
	draw_string(3, 25, "Score: ", FG_COLOUR);
	draw_int(35, 25, score, FG_COLOUR);
	show_screen();
}


// initial setup of the timer
void setup_timer( void ) {
	TCCR0A = 0;
	TCCR0B = 5; 
	TIMSK0 = 1;
	sei();
}


// When the game is start it setups the following:

void startGame ( void ) {
	setup();
	setup_timer();
	clear_screen();
	setupStarFighter();
	setup_astroid();
	show_screen();	
}



// When Left Button is pressed game is reset
void reset(){
	while ((BIT_IS_SET(PINF, 6))){
		paused = true;
		lives = 5;
		score = 0;
		double time = ( overflow_counter * 256.0 + TCNT0 ) * PRESCALE / FREQ;
		draw_double(35, 5, time, FG_COLOUR);

	startGame();
	}
}

// In the loop function there are different conditions according to the specifications how the game should work.

void loop() {
	reset();
	if((BIT_IS_SET(PINB, 0))){
		paused = !paused;
	}
	if ((BIT_IS_SET(PINB, 7))){
		p = !p;
	}
	if(p){
		gameStatus();
	}
	if(paused){
		clear_screen();
		if(p){
		gameStatus();
		}
		else {
		drawDeflectorShield();
		drawStarfighter();
		drawAstroid();
		show_screen();
		}	
	}
	else {
		clear_screen();
		drawDeflectorShield();
		drawStarfighter();
		drawAstroid();
		show_screen();
		velocity();
		updateStarFighter();
		update_astroid();
	}
}


int main( void ){
	startGame();
	setup();
	while (!(BIT_IS_SET(PINF, 6)));
	for ( ;; ){
		loop();
		_delay_ms(100);
	}
}
