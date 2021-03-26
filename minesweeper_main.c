/*----------------------------------------------------------------------------
 *	CHAN TIAN WEN		25298			JANUARY 2021 SEMESTER
 *	
 *	This is a working minesweeper game with the following functionalities:
 *	1. Real-Time Operating System
 *	2. Keypad input for left, right, select, open and flag
 *	3. ADC input for vertical movement across mine field
 *	4. 7 SEG to display number of flags available
 *	5. Red LEDs flash upon losing
 *	6. RGB LEDs go into party mode upon winning
 *	7. LCD as game display
 *
 *	GAMEPLAY INSTRUCTIONS:
 *	1. Use keypad to select(keypad 5) levels by moving up(keypad 2) and down(keypad 8).
 *	2. Level EASY has 5 mines, MEDIUM has 10 mines, and HARD has 20 mines
 *	2. Select mines by moving up (ADC anti-clockwise), down (ADC clockwise), left(keypad 4) and right(keypad 6).
 *	3. Open(keypad 1) mines or flag(keypad 3) mines.
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"  
#include "sys.c"                                 
#include "clk.c"                              
#include "gpio.c"                              
#include "spi.c"
#include "ADC.c"

//custom RTOS library codes produced by 2019 students
#include "Seven_Segment_Display.c"               
#include "Keypad.c"                            
#include "lcd.c"						//this has been slightly modified. The modified parts are marked.
#include "Variable_Resistor.c" 

extern int Init_Thread(void);

#define FIELD_HEIGHT	8			// max height of our minefield in field squares
#define FIELD_WIDTH		16			// max width of our minefield in field squares
#define FIELD_SQUARE	8		// pixel size of our field squares
#define CUBE_SIZE		4				// pixel size of the itty-bitty cube occupying the field square

int field[FIELD_HEIGHT][FIELD_WIDTH];						//holds the actual contents of our minefield
int field_state[FIELD_HEIGHT][FIELD_WIDTH];			//holds the state of the minefield squares (e.g, opened(1), flagged(2) or hidden(0))

volatile uint8_t keypad, prev_keypad, timer_state=0;
volatile float VR, rand_seed;
int mines, mine_count=0;
int cursor_x = 4;
int cursor_y = 2;

//takes a number and returns the number in char datatype. We need this for printing purposes
char grab_numchar(int number) {
	
	switch(number){
		case 1:
			return '1';		
		
		case 2:
			return '2';
	
		case 3:
			return '3';
	
		case 4:
			return '4';
	
		case 5:
			return '5';
	
		case 6:
			return '6';
	
		case 7:
			return '7';
	
		case 8:
			return '8';
		
		case '*':
			return '*';
		
		default:
			return ' ';	
	}
}

//self-written round function as the native C functions hangs the program all the time :( much sad
int rounding(float number) {
	return (int)(number + 0.5);
}

//pseudo-timer function (wait for loop to finish then change timer_state)
void pseudo_timer(int seconds) {
	int i, j;
	
	for (i=0; i<seconds*1000; i++) {
		for (j=0; j<seconds*500; j++) {
		}
	}	
	
	timer_state^=1;						//XOR operator is a handy way of alternating between 1 and 0 states
}

//grab x and y coordinates for drawing
//these x and y coordinates ALWAYS refer to the upper left starting point of the thing-to-draw
//we use two functions because c programming doesn't allow single return of multiple values =.=
int grab_x(int field_x, unsigned char symbol) {
	
	int x;
	
	switch(symbol) {		
		case 'c':		//cursor
			x = field_x*FIELD_SQUARE+1;
			break;
		
		default:
			x = field_x*FIELD_SQUARE+2;
			break;		
	}
	return x;	
}

int grab_y(int field_y, unsigned char symbol) {
	
	int y;	
	
	switch(symbol) {		
		case 'c':		//cursor
			y = field_y*FIELD_SQUARE+1;
			break;
		
		default:
			y = field_y*FIELD_SQUARE+2;
			break;
	}
	return y;
}

//looping over the welcome screen until a choice of level is chosen
int welcome_screen() {
	
	int init_row = 0;
	int init_col = 15;
	int i, j, square_x, square_y = 0;
	
	int init_y = cursor_y;
	
	static unsigned char welcome_message[4][13] = {
	'*','M','I','N','E','S','W','E','E','P','E','R','*',
	' ',' ',' ',' ','E','A','S','Y',' ',' ',' ',' ',' ',
	' ',' ',' ','M','E','D','I','U','M',' ',' ',' ',' ',
	' ',' ',' ',' ','H','A','R','D',' ',' ',' ',' ',' ',
	};
	
	for (i=0; i<4; i++) {
		for (j=0; j<13; j++) {
			printC_5x7(init_col+(j*8), init_row+(i*2*8), welcome_message[i][j]);
		}
	}
	
	while (Keypad_Return() != 5) {			//while the user has not selected a level
		keypad = Keypad_Return();
		rand_seed += 7;
		square_x = grab_x(cursor_x, 'c');
		square_y = grab_y(cursor_y, 'c');	
		RectangleDraw(square_x, square_y-4, square_x + 56, square_y + 8, FG_COLOR, BG_COLOR);
		
		if (keypad == 2 && cursor_y>init_y && prev_keypad != 2) {
			RectangleDraw(square_x, square_y-4, square_x + 56, square_y + 8, BG_COLOR, BG_COLOR);
			cursor_y -=2;
		} else if (keypad == 8 && cursor_y<init_y + 3 && prev_keypad != 8) {
			RectangleDraw(square_x, square_y-4, square_x + 56, square_y + 8, BG_COLOR, BG_COLOR);
			cursor_y += 2;
		} else {
			cursor_y = cursor_y;
		}
		prev_keypad = keypad;				//required to debounce the keypad		
	}	
	
	//Return number of mines depending on what level of difficulty was chosen
	if (cursor_y == init_y) {
		return 5;
	} else if (cursor_y == init_y + 2) {
		return 10;
	} else {
		return 20;
	}
}

//seed our minefield with randomly located mines
//the first square the user clicks on is always a safe square
void rand_mines(int number, int safe_x, int safe_y) {
	 
	int i, row, col = 0;
	
	while (i<number) {
		row = rand()%FIELD_HEIGHT;			//number of rows is dependent on field height
		col = rand()%FIELD_WIDTH;				//number of rows is dependent on field width
		
		//mines are not allowed to be placed if a)that square already has a mine, b)that square is the safe square
		if (field[row][col] == '*') continue;						
		if (row == safe_y && col == safe_x) continue;
		
		field[row][col] = '*';
		i+=1;
	}	
}

//function to clear specified field square for redrawing. Needs to be called when using 2DGraphics function
void clear_cursor (int row, int col) {

	int square_x, square_y;

	square_x = grab_x(col, 'c');
	square_y = grab_y(row, 'c');
	
	RectangleDraw(square_x, square_y, square_x+CUBE_SIZE+2, square_y + CUBE_SIZE+2, BG_COLOR, BG_COLOR);
}

//calculate the clues for each mine.
//if the field square has a mine, we fill all surrounding cells with clues, PROVIDED THAT the cell-to-fill does not have a mine.
//This function is the core of minesweeper and the most complex. The maximum amt of squares surrounding a cell is 8.
//However, it can have less than that when it is at row=0, row=max, col=0 and col=max (borders).
//Therefore, we must always check if the cell exists
void calculate_clues() {
	int row, col, i, j, clue;
	
	for (row=0; row<FIELD_HEIGHT; row++) {
		clue = 0;				//reset clue for new square to be filled
		for (col=0; col<FIELD_WIDTH; col++) {
			clue = 0;
			if (field[row][col] != '*') {						//if it is an empty cell we can smack a clue in
				for (i=-1; i<2; i++) {
					for (j=-1; j<2; j++) {
						if (row+i < 0 || row+i >= FIELD_HEIGHT) continue;			//skip if not an existing row
						if (col+j < 0 || col+j >= FIELD_WIDTH) continue;			//skip if not an existing column
						if (field[row+i][col+j] != '*') continue;							//skip if it does not have a mine
						clue += 1;			//clue increases if a)the cell-to-be-checked exists and b)there is a mine
					}
				}
				
				field[row][col] = clue;							//update our field to have the clues				
			}				
		}
	}	
}

//draw opened minefield (GAME OVER)
void draw_opened() {
	int row, col;
	
	clear_LCD();
	for (row=0; row<FIELD_HEIGHT; row++) {
		for (col=0; col<FIELD_WIDTH; col++) {
			if (field[row][col] == '*') {			//draw the mines
				printC_5x7(grab_x(col, '*'), grab_y(row, '*'), '*');				
			} else {													//draw the clues
				printC_5x7(grab_x(col, 'n'), grab_y(row, 'n'), grab_numchar(field[row][col]));	
			}
		}
	}
}

//the second most complex function of minesweeper.
//when a zero-clue cell is clicked, all surrounding(8) cells are opened (unless it's a mine). 
//Not only that, this effect triggers a chain reaction of openings if these 8 cells also contain zeroes. 
void open_surround(int row, int col) {
	
	int i, j;
	
	for (i=-1; i<2; i++) {
		for (j=-1; j<2; j++) {
			if (row+i < 0 || row+i >= FIELD_HEIGHT) continue;			//skip if not an existing row
			if (col+j < 0 || col+j >= FIELD_WIDTH) continue;			//skip if not an existing column
			if (field_state[row+i][col+j] != 0) continue;					//skip if the square is already revealed
			if (field[row+i][col+j] == '*') continue;							//skip if the square is a mine
			
			field_state[row+i][col+j] = 1;												//mark to be opened in next LCD redraw
		}
	}
	return;	
}

//check if victory has been attained (all non-mine squares are opened)
int check_victory() {
	int row, col;
	
	for (row=0; row<FIELD_HEIGHT; row++) {
		for (col=0; col<FIELD_WIDTH; col++) {
			//if the square is not a mine but not opened, game is still in progress
			if (field[row][col] != '*' && field_state[row][col] != 1) {
				return 0;
			}
		}
	} 
	return 1;	
}

//function to draw our cursor. Our cursor is represented by a rectangle surrounding the field cube
void draw_cursor() {
	int square_x, square_y;	
	
	//cursors should not exceed boundaries of the minefield
	if (keypad == 6 && cursor_x+1<FIELD_WIDTH) {				//mid-right keypress to move cursor right
		cursor_x +=1;
	} else if (keypad == 4 && cursor_x-1>= 0)	{					//mid-left keypress to move cursor left
		cursor_x -=1;
	} else {
		cursor_x = cursor_x;
	}
	
	cursor_y = rounding(VR*(FIELD_HEIGHT-1)/255);

	square_x = grab_x(cursor_x, 'c');
	square_y = grab_y(cursor_y, 'c');
	
	RectangleDraw(square_x, square_y, square_x + CUBE_SIZE+2, square_y + CUBE_SIZE+2, FG_COLOR, BG_COLOR);
}

//get user input for flagging and opening
void get_user_input() {
	if (keypad == 1 && field_state[cursor_y][cursor_x] != 2 && prev_keypad != 1) {
			field_state[cursor_y][cursor_x] = 1;					//open up the square
	} else if (keypad == 3 && prev_keypad != 3 && field_state[cursor_y][cursor_x] == 0) {												
		if (field_state[cursor_y][cursor_x] == 2) {
			field_state[cursor_y][cursor_x] = 0;				//unflag if previously flagged
			mine_count += 1;
		} else {
			field_state[cursor_y][cursor_x] = 2;				//flag if unflagged
			mine_count -= 1;
		}
	}
	prev_keypad = keypad;
}

//explosion animation
void explosion(int row, int col) {	
	
	int beep;
	
	PB11=1;
	for (beep=0;beep<8;beep++) {
		pseudo_timer(1);
		PB11 = !PB11;
	}
	
	PB11=0;
	pseudo_timer(2);
	clear_LCD();
	PB11=1;
	draw_LCD(explode);
	pseudo_timer(2);	
}

//draw minefield (GAME IN PROGRESS)
int draw_field() {
	int row, col, square_x, square_y;
		
	for (row=0; row<FIELD_HEIGHT; row++) {
		for (col=0; col<FIELD_WIDTH; col++) {
				square_x = grab_x(col, 's');
				square_y = grab_y(row, 's');
			
			if (field_state[row][col] == 0) {						//state 0 = hidden
				RectangleFill(square_x, square_y, square_x + CUBE_SIZE, square_y + CUBE_SIZE, FG_COLOR, BG_COLOR);
			} else if (field_state[row][col] == 1) {		//state 1 = open up
				if (field[row][col] == '*') {					//opened up a mine - lost the game!
					clear_cursor(cursor_y, cursor_x);
					printC_5x7(square_x, square_y, grab_numchar(field[row][col]));
					explosion(row, col);
					return 0;														//victory becomes 0
				} else if (field[row][col] == 0) {		//open surrounding cells if the opened cell is 0
					printC_5x7(square_x, square_y, grab_numchar(field[row][col]));
					open_surround(row, col);
				} else {															//open the clue cell
					printC_5x7(square_x, square_y, grab_numchar(field[row][col]));	
				}
			} else if (field_state[row][col] == 2) {		//state 2 = flagged
				printC_5x7(square_x, square_y, 'x');			
			}
		}
	}
	clear_cursor(cursor_y, cursor_x);					//delete the cursor
	draw_cursor();		//redraw cursor
	if (check_victory()) {
		return 1;			//we return 1 if the game is won
	} else {
		return 3;			//we return 3 if the game is neither won or lost (in progress)
	}
}

//initialize our minefield and minefield state with zeros
//the first user input is always a safe square, so we need to lock that in.
void init_field() {
	
	int row, col;
	for (row = 0; row < FIELD_HEIGHT; row++) {
		for (col = 0; col < FIELD_WIDTH; col++) {
			field[row][col] = 0;
			field_state[row][col] = 0;
		}
	}
	clear_LCD();				//delete the welcome screen
	
	while (keypad != 1) {
		keypad = Keypad_Return();
		VR = Variable_Resistor_Value();
		draw_field();
	}
	field_state[cursor_y][cursor_x] = 1;		//marked it as an opened square
}

//game over animations
void game_over(int victory) {
	int beep, i, j;
	
	int init_col = 32;
	int init_row = 0;
	
	static unsigned char lost_message[4][9] = {
	'G','A','M','E',' ','O','V','E','R',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ','T','O','O',' ','B','A','D',' ',
	' ','Y','O','U',' ','D','E','D',' ',
	};
	
	static unsigned char win_message[4][9] = {
	'G','A','M','E',' ','O','V','E','R',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ','Y','O','U',' ','W','I','N',' ',
	' ',' ','G','G',' ','E','Z',' ',' ',
	};
	
	clear_LCD();
	for (beep=0; beep<8; beep++) {
		pseudo_timer(1);
		if (timer_state) {
			if (victory) {
				for (i=0; i<4; i++) {
					for (j=0; j<9; j++) {
						printC_5x7(init_col+(j*8), init_row+(i*2*8), win_message[i][j]);
					}
				}
			} else {
				for (i=0; i<4; i++) {
					for (j=0; j<9; j++) {
						printC_5x7(init_col+(j*8), init_row+(i*2*8), lost_message[i][j]);
					}
				}
			}
		} else {
			clear_LCD();
		}
	}		
}

//ending screen
void ending_screen(int victory) {
		pseudo_timer(1);
		if (victory) {
			clear_LCD();
			PA12 = !PA12;
			PA13 = !PA13;
			PA14 = !PA14;
			
			if (timer_state) {
				draw_LCD(confetti);
			} else {
				clear_LCD();
			}
			
		} else {
			PC12 = !PC12;
			PC13 = !PC13;
			PC14 = !PC14;
			PC15 = !PC15;
		}	
}

/* -------- THREAD DECLARATIONS -------- */
//Thread 1 is in charge of getting user input from variable resistor and keypad
void task_thread1() {
	while(1) {
		VR = Variable_Resistor_Value();
		keypad = Keypad_Return();
	}
}

//Thread 2 is in charge of the main gameplay logic you see on LCD screen.
void task_thread2() {
	int victory = 3;
	
	while (1) {
		if (victory==3) {
			get_user_input();
			victory = draw_field();
		} else {
			game_over(victory);
			clear_LCD();
			draw_opened();
			PC12=PC13=PC14=PC15=1;		//red LEDs are off
			PA14=0;										//RGB LEDs at diff states
			PA13=PA12=1;
			pseudo_timer(1);
			
			while(1) {
				ending_screen(victory);
			}
		}
	}
}

//Thread 3 is in charge of 7-segment display
void task_thread3() {
	seven_segment_begin();
}

//Thread 4 updates the seven_segment mine values
void task_thread4() {
	while (1) {
		seven_segment_value_input(mine_count);
	}
}


//Next, we define the priority level of each thread.
osThreadDef(task_thread1,osPriorityNormal,1,0);              //     Thread Priority define.
osThreadDef(task_thread2,osPriorityNormal,1,0);
osThreadDef(task_thread3,osPriorityNormal,1,0);
osThreadDef(task_thread4,osPriorityNormal,1,0);

/*
 * main: initialize and start the system
 */
int32_t main (void) {
	
	SYS_UnlockReg();
	CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);           //     Enable clock source -> XTL12M. 12 MHz.
	CLK->CLKSEL0|=CLK_CLKSEL0_HCLK_S_Msk;                 //     Select 12MHz clock as Host Clock.
	SYS_LockReg();
	
  osKernelInitialize ();
	
	InitVRADC();                                          // Initialize Variable Resistor ADC, LCD.
	init_LCD();
	clear_LCD();
	
	mines = welcome_screen();
	mine_count = mines;				//get mine count depending on chosen level of difficulty
	srand(rand_seed);					//seed random generator

	//get random starting positions for cursor
	cursor_y = rand()%FIELD_HEIGHT;			
	cursor_x = rand()%FIELD_WIDTH;
	
	init_field();
	rand_mines(mines, cursor_x, cursor_y);
	calculate_clues();
	
	Init_Thread();                                        // Initialize thread.
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	osThreadCreate(osThread(task_thread1),NULL);               // Allocating thread locations for specific thread function.	
	osThreadCreate(osThread(task_thread2),NULL);
	osThreadCreate(osThread(task_thread3),NULL);	
	osThreadCreate(osThread(task_thread4),NULL);
  osKernelStart (); 
}
