#include "NUC1xx.h"
#include "boardlib.h"
#include "PMWlib.h"
#include "timerlib.h"
#include "NUC1xx.h"
#include "SYS.h"
#include "GPIO.h"
#include "LCD.h"
#include "2DGraphics.h"

#define BOARD_X_START 0
#define BOARD_X_SC_START 4
#define BOARD_X_LAST 11
#define BOARD_Y_START 0
#define BOARD_Y_LAST 14
#define CORD_MAX 4

volatile uint8_t MUSIC_PLAYING = 0;
volatile uint8_t MUSIC_INDEX = 0;
volatile uint8_t MUSIC_REPEAT = 0;
volatile uint8_t MUSIC_NUM = 0;
volatile uint8_t MUSIC_TEMP = 0;
volatile uint8_t NEXT = 1;
volatile uint8_t SP_GOING = 1;
volatile uint8_t RND_SEED = 0;
volatile uint8_t IS_SPEEDUP = 0;
volatile int8_t  cord[5][2];
volatile uint8_t sp_y = 5;
volatile uint8_t clear_buf[8];
volatile uint8_t clear_q = 0;
volatile uint16_t score = 0;

uint8_t CLEAR_ANI = 0;
uint8_t IS_FAILED = 0;
uint8_t sp_type = 0;				//Tetris block type
uint8_t sp_index = 3;
uint8_t cord_q = 0;


uint8_t cubes[][4][4] = {
	0,0,0,0,
	1,0,0,0,
	1,1,1,0,
	0,0,0,0,
	
	0,0,0,0,
	0,0,1,0,
	1,1,1,0,
	0,0,0,0,
	
	0,0,0,0,
	1,1,0,0,
	0,1,1,0,
	0,0,0,0,
	
	0,0,0,0,
	0,1,1,0,
	1,1,0,0,
	0,0,0,0,
	
	0,0,0,0,
	0,1,0,0,
	1,1,1,0,
	0,0,0,0,
	
	0,0,0,0,
	0,1,1,0,
	0,1,1,0,
	0,0,0,0,
	
	0,1,0,0,
	0,1,0,0,
	0,1,0,0,
	0,1,0,0
	
};

//矩陣轉至 把左下角貼給左上角

int8_t board[][BOARD_Y_LAST + 1] = {
	
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
};





const uint8_t sheet[][3] = {
	MUSIC_START,MUSIC_START,MUSIC_START, // 1. 好音效
	1,0,0,
	1,0,0,
	1,0,0,
	1,3,0,
	1,4,0,
	1,5,0,
	1,5,0,
	1,0,0,
	1,0,0,
	1,0,0,
	1,0,0,
	MUSIC_STOP,MUSIC_STOP,MUSIC_STOP,
	MUSIC_START,MUSIC_START,MUSIC_START, // 2. 壞音效
	1,0,0,
	1,0,0,
	1,0,0,
	1,5,0,
	1,4,0,
	1,3,0,
	1,2,0,
	1,1,0,
	1,1,0,
	1,0,0,
	1,0,0,
	1,0,0,
	1,0,0,
	1,0,0,
	
	MUSIC_STOP,MUSIC_STOP,MUSIC_STOP,
	MUSIC_START,MUSIC_START,MUSIC_START, // 3. 主題曲
	2,3,0,
	2,3,0,
	1,7,0,
	2,1,0,
	2,2,0,
	2,2,0,
	2,1,0,
	1,7,0,
	1,6,0,
	1,6,0,
	1,6,0,
	2,1,0,
	2,3,0,
	2,3,0,
	2,2,0,
	2,1,0,
	1,7,0,
	1,7,0,
	1,7,0,
	2,1,0,
	2,2,0,
	2,2,0,
	2,3,0,
	2,3,0,
	2,1,0,
	2,1,0,
	1,6,0,
	1,0,0,
	1,6,0,
	1,0,0,
	1,0,0, // 第二段
	1,0,0,
	1,0,0,
	2,2,0,
	2,2,0,
	2,2,0,
	2,4,0,
	2,6,0,
	2,6,0,
	2,5,0,
	2,4,0,
	2,3,0,
	2,0,0,
	2,1,0,
	2,0,0,
	2,3,0,
	2,0,0,
	2,2,0,
	2,1,0,
	1,7,0,
	1,7,0,
	1,7,0,
	2,1,0,
	2,2,0,
	2,2,0,
	2,3,0,
	2,3,0,
	2,1,0,
	2,0,0,
	1,6,0,
	1,0,0,
	1,6,0, // 第三段
	1,0,0,
	1,0,0,
	1,0,0,
	1,0,0,
	2,3,0,
	2,3,0,
	2,3,0,
	2,3,0,
	2,1,0,
	2,1,0,
	2,1,0,
	2,1,0,
	2,2,0,
	2,2,0,
	2,2,0,
	2,2,0,
	1,7,0,
	1,7,0,
	1,7,0,
	1,7,0,
	2,1,0,
	2,1,0,
	2,1,0,
	2,1,0,
	1,6,0,
	1,6,0,
	1,6,0,
	1,6,0,
	1,5,1,
	1,5,1,
	1,5,1,
	1,5,1,
	1,7,0,
	1,7,0,
	1,7,0,
	1,7,0,
	2,3,0,
	2,3,0,
	2,3,0,
	2,3,0,
	2,1,0,
	2,1,0,
	2,1,0,
	2,1,0,
	2,2,0,
	2,2,0,
	2,2,0,
	2,2,0,
	1,7,0,
	1,7,0,
	1,7,0,
	1,7,0,
	2,1,0,
	2,1,0,
	2,3,0,
	2,3,0,
	2,6,0,
	2,6,0,
	2,6,0,
	2,6,0,
	2,5,1,
	2,5,1,
	2,0,0,
	1,0,0,
	1,0,0,
	1,0,0,
	MUSIC_STOP,MUSIC_STOP,MUSIC_STOP,
	MUSIC_START,MUSIC_START,MUSIC_START,
	1,3,0,
	1,3,0,
	1,5,0,
	1,5,0,
	1,0,0,
	1,0,0,
	1,0,0,
	1,0,0,
	1,0,0,
	1,0,0,
	MUSIC_STOP,MUSIC_STOP,MUSIC_STOP,
};

uint8_t RND(uint8_t range){
	RND_SEED += range;
	
	return RND_SEED % range;
}

uint8_t pointY(uint8_t Y){
	return (4+Y*8)%128;
}

void clear(uint8_t x,uint8_t y,uint8_t len){
	
	unsigned char e = 0x00;
	
	draw_Pixel(x,y,BG_COLOR, BG_COLOR);
	
}

void drawSymbol(uint8_t x,uint8_t y,uint8_t c){

	int8_t c_table[] = {'+','-','x','X','\\','c','h','T','R','I','S',-1};
	//除號用正斜線代替
	int i = 0;
	while(c_table[i] != c && c_table[i] != -1)
		i++;
	
	if(c_table[i] != -1)
		RectangleDraw(x,y, x+4, y-4, FG_COLOR, BG_COLOR);
}

void turn(){
	
	uint8_t i;
	
	int8_t x_dif[CORD_MAX], y_dif[CORD_MAX], new_x[CORD_MAX], new_y[CORD_MAX];
	
	uint8_t rotate_size;

	if( sp_type == 5 )
		return;
	else if( sp_type < 5 )
		rotate_size = 2;
	else if( sp_type == 6 )
		rotate_size = 3;

	
	for(i = 0;i < CORD_MAX;i++){
		x_dif[i] = cord[i][0] - cord[CORD_MAX][0];
		y_dif[i] = cord[i][1] - cord[CORD_MAX][1];
		new_x[i] = cord[CORD_MAX][0] + y_dif[i];
		new_y[i] = cord[CORD_MAX][1] + rotate_size - x_dif[i];
		
		
		if( new_x[i] > BOARD_X_LAST ||
				new_x[i] < BOARD_X_START ||
				new_y[i] > BOARD_Y_LAST ||
				new_y[i] < BOARD_Y_START ||
				board[ new_x[i] ][ new_y[i] ] == 2)
				return;
	}
		
		for(i = 0;i < CORD_MAX;i++){
		
			cord[i][0] = new_x[i];
			cord[i][1] = new_y[i];
			
		}
}

void shift(int8_t dir){
	
	uint8_t i;
	
	for(i = 0;i < CORD_MAX;i++)
		if( (cord[i][1] + dir) < BOARD_Y_START ||
				(cord[i][1] + dir) > BOARD_Y_LAST ||
				board[ cord[i][0] ][ cord[i][1] + dir ] == 2
		) return;
		
	for(i = 0;i < cord_q;i++)
			cord[i][1] += dir;

	cord[4][1] += dir;
	

}

void spawn(){
	
		int i,j;
	
	/*	if( board[4][sp_y] == 2)
			IS_FAILED = 1;*/

		
		cord_q = 0;
		
		for(i = 0;i < 4;i++)
			for(j = 0;j < 4;j++)
				if(cubes[sp_type][i][j]){
					cord[cord_q][0] = i;
					cord[cord_q++][1] = sp_y + j;
				}
	
		cord[4][0] = 0;
		cord[4][1] = sp_y; 
	
				

		sp_type = RND(7); // 重新產生新的type
		SP_GOING = 0;
}


void check(){
	
	uint8_t x,y;
	
	clear_q = 0;
	
	for(x = BOARD_X_LAST;x >= BOARD_X_SC_START;x--){
		
		uint8_t is_cleared = 1;
		
		for(y = BOARD_Y_START;y <= BOARD_Y_LAST;y++)
			if( board[x][y] != 2){
				is_cleared = 0;
				break;
			}
		
		if(is_cleared)
			clear_buf[clear_q++] = x;
		
	}
	
	if(clear_q){
	
		CLEAR_ANI = 1;
		if(MUSIC_PLAYING){
			MUSIC_TEMP = MUSIC_INDEX + 2;
			MUSIC_INDEX = search_music(1,sheet);
		}
		
	}
}

void fall(){
	
	uint8_t i;
	
	uint8_t is_block = 0;
	
	for(i = 0;i < cord_q;i++)
		if( 2 == board[ cord[i][0] + 1][ cord[i][1] ] ){
				is_block = 1;
				break;
		}
	

	if(is_block){
		for(i = 0;i < cord_q;i++){
			
			board[ cord[i][0] ][ cord[i][1] ] = 2;
			if( cord[i][0] < BOARD_X_SC_START )
				IS_FAILED = 1;
			
		}
		
		if(IS_FAILED){
			
			if(MUSIC_PLAYING){
					MUSIC_TEMP = MUSIC_INDEX + 2;
					MUSIC_INDEX = search_music(2,sheet);
				}
			IS_SPEEDUP = 0;
				
			return;
		}
		
		cord_q = 0; // 清空儲存點
		SP_GOING = 1;
		check();
		
	}else{
		for(i = 0;i < cord_q;i++)
			++cord[i][0];
			
		++cord[4][0];
	}
		
	
}

void clear_cubes(){
	
	//掉落演算法: 每一行其下有幾個清空行就掉幾格
	
	uint8_t fall_times[ BOARD_X_LAST ] = {0,0,0,0,0,0,0,0,0,0,0};
	uint8_t i;
	uint8_t x,y;
	
	score += clear_q;
	
	for(i = 0;i < clear_q;i++)
		for(x = clear_buf[i] - 1 ; x >= BOARD_X_SC_START; x--)
			fall_times[x] ++;
	

	
	for(x = BOARD_X_LAST - 1 ;x >= BOARD_X_SC_START; x--)
		for(y = BOARD_Y_START ; y <= BOARD_Y_LAST; y++)
			board[x + fall_times[x] ][ y ] =  board[x][y];
	
	
	
}


void draw_board(){
	
	int x,y,i,j;
	
	if(CLEAR_ANI)
		return;

	for(x = BOARD_X_SC_START;x <= BOARD_X_LAST ;x++)
		for(y = BOARD_Y_START ;y <= BOARD_Y_LAST;y++)
			if(board[x][y])
				drawSymbol(x-4,pointY(y),'c');
			else
				clear(x-4,pointY(y),8);
			
	if(!IS_FAILED){
		for(i = 0;i < 4;i++)
			for(j = 0;j < 4;j++)
				if(cubes[sp_type][i][j]){
					drawSymbol(i,pointY( BOARD_Y_LAST - 2 + j ),'h');
				}
	}
			
			
	for(x = 0;x < CORD_MAX;x++)
			if(cord[x][0] > 3)
				drawSymbol(cord[x][0]-4, pointY( cord[x][1] ) , 'c');

	
	
	
}

void TMR0_IRQHandler() // Timer0 interrupt subroutine 
{

	TIMER0->TCMPR = IS_SPEEDUP ? 5000 : 27500;
	
	NEXT = 1;
	
	RND_SEED++;
	
	TIMER0->TISR.TIF =1;
}

void TMR1_IRQHandler() // Timer0 interrupt subroutine 
{

	if(MUSIC_PLAYING)
			MUSIC_INDEX++;
	
	TIMER1->TISR.TIF =1;
}

//----------------------------------------------------------------------------
//  MAIN function
//----------------------------------------------------------------------------
int32_t main (void)
{
	

	uint16_t count[11];
	
	uint16_t bound = 2500;
	
	uint16_t seg[4];
	
	uint8_t i,j;
	uint8_t x,y;
	uint8_t temp_score;
	
	uint8_t screen_ani = 0;
	uint8_t about_to_start = 0;
	
	SP_GOING = 1;
	
	MUSIC_NUM = 3;
	MUSIC_REPEAT = 1;
	MUSIC_INDEX = search_music(MUSIC_NUM,sheet);
	MUSIC_PLAYING = 0;
	
// INITs//////////
	InitHCLK12M();
	
	InitTIMER0(25000);

	InitTIMER1(10000);
	
	Init_board();
	
	init_LCD();

	InitPWM();
	
//////////////////
	
	/*while(1){
		
		drawSymbol(3,pointY(2),'T');
		drawAlphabet(3,pointY(4),'E');
		drawSymbol(3,pointY(6),'T');
		drawSymbol(3,pointY(8),'R');
		drawSymbol(3,pointY(10),'I');
		drawSymbol(3,pointY(12),'S');
		
		
		if(about_to_start){
			
			if(sheet[MUSIC_INDEX][0] == MUSIC_STOP){
				
				MUSIC_INDEX = search_music(3,sheet);
				NEXT = 0;
				draw_board();
				break;
				
			}
			
				
			if(MUSIC_INDEX % 2){
				

				drawSymbol(5,pointY(5),'S');
				drawSymbol(5,pointY(6),'T');
				drawAlphabet(5,pointY(7),'A');
				drawSymbol(5,pointY(8),'R');
				drawSymbol(5,pointY(9),'T');
				
			}else{
				

				clear(5,pointY(5),8);
				clear(5,pointY(6),8);
				clear(5,pointY(7),8);
				clear(5,pointY(8),8);
				clear(5,pointY(9),8);
				
			}
			

			PWM4_Freq(toneToFreq(sheet[MUSIC_INDEX][MUSIC_LEVEL],
								 sheet[MUSIC_INDEX][MUSIC_SCALE],
								 sheet[MUSIC_INDEX][MUSIC_ACC]), 95);

			
			continue;
		}
		
		
		if(screen_ani){
			

			drawSymbol(5,pointY(5),'S');
			drawSymbol(5,pointY(6),'T');
			drawAlphabet(5,pointY(7),'A');
			drawSymbol(5,pointY(8),'R');
			drawSymbol(5,pointY(9),'T');
			
		}else{
			

			clear(5,pointY(5),8);
			clear(5,pointY(6),8);
			clear(5,pointY(7),8);
			clear(5,pointY(8),8);
			clear(5,pointY(9),8);
			
		}
		
		
		if(NEXT){
			
			screen_ani ^= 1;
			NEXT = 0;
			
		}
		
		read_keyboard(count , 10);

		if(check_count( count , bound , MID_MID )){
		
			MUSIC_INDEX = search_music(4,sheet);
			MUSIC_PLAYING = 1;
			about_to_start = 1;
		}
		
		
	}*/
	
	
	
	
	while(!IS_FAILED)
	{				 
			
			// 控制音樂
		/*	if(sheet[MUSIC_INDEX][0] == MUSIC_STOP)
				if(MUSIC_TEMP){
					MUSIC_INDEX = MUSIC_TEMP;
					MUSIC_TEMP = 0;
				}else
				if(MUSIC_REPEAT)
					MUSIC_INDEX = search_music(MUSIC_NUM,sheet);
				else
					MUSIC_PLAYING = 0;
			
			if(MUSIC_PLAYING)
				PWM4_Freq(toneToFreq(sheet[MUSIC_INDEX][MUSIC_LEVEL],
														 sheet[MUSIC_INDEX][MUSIC_SCALE],
														 sheet[MUSIC_INDEX][MUSIC_ACC]), 95);
			else
				PWM4_Freq(0, 95);*/
			
			
			/*for(i = 0;i < 4;i++){
				
					temp_score = score;
				
					for(j = 0;j < i;j++)
						temp_score /= 10;
				
					temp_score %= 10;
				
					seg[3-i] =  new_seg_intToHex( temp_score );
			}
			set_board_all_seg(seg,5);*/
			
		/*	read_keyboard(count , 2);
			
		/*	if(check_count( count , bound , LEFT_BOT )){
				MUSIC_PLAYING ^= 1;
				RND_SEED++;
			}
					
			if(check_count( count , bound , LEFT_MID )){
				RND_SEED++;
				shift(-1);
				draw_board();
			}
			if(check_count( count , bound , RIGHT_MID )){
				RND_SEED++;
				shift(1);
				draw_board();
			}

			if(check_count( count , bound , MID_TOP )){
				RND_SEED++;
				turn();
				draw_board();
			}
			
			IS_SPEEDUP = is_pressed( MID_BOT );			*/
			
			if(NEXT){
				
				NEXT = 0;
				
				/*if(CLEAR_ANI){
					
					for(i = 0;i < clear_q; i++)
						for(y = BOARD_Y_START;y <= BOARD_Y_LAST;y++)
							if( CLEAR_ANI % 2 )
								clear(clear_buf[i] + 4, pointY( y ) , 8);
							else
								drawSymbol( clear_buf[i] + 4, pointY( y ) , 'c' );

					if(CLEAR_ANI == 4){
						CLEAR_ANI = 0;
						clear_cubes();
					}
						else
						CLEAR_ANI++;
					
				}else{*/
					
					fall();
					
					if(SP_GOING)
						spawn();
				
					draw_board();
					
				}
			}
			

			

			
			
			
			
		
	}
	
	
	/*while(1){
		
			if(sheet[MUSIC_INDEX][0] == MUSIC_STOP)
				if(MUSIC_TEMP){
					MUSIC_INDEX = MUSIC_TEMP;
					MUSIC_TEMP = 0;
				}else
				if(MUSIC_REPEAT)
					MUSIC_INDEX = search_music(MUSIC_NUM,sheet);
				else
					MUSIC_PLAYING = 0;
			
			if(MUSIC_PLAYING)
				PWM4_Freq(toneToFreq(sheet[MUSIC_INDEX][MUSIC_LEVEL],
														 sheet[MUSIC_INDEX][MUSIC_SCALE],
														 sheet[MUSIC_INDEX][MUSIC_ACC]), 95);
			else
				PWM4_Freq(0, 95);
			
			
			for(i = 0;i < 4;i++){
				
					temp_score = score;
				
					for(j = 0;j < i;j++)
						temp_score /= 10;
				
					temp_score %= 10;
				
					seg[3-i] =  new_seg_intToHex( temp_score );
			}
			set_board_all_seg(seg,30);
			

			
			
			read_keyboard(count , 100);
			
			if(check_count( count , bound , LEFT_BOT ))
				MUSIC_PLAYING ^= 1;
		
		
			if(screen_ani)
					draw_board();	
				else{
					for(x = BOARD_X_SC_START;x <= BOARD_X_LAST ;x++)
						for(y = BOARD_Y_START ;y <= BOARD_Y_LAST;y++)
							clear(x-4,pointY(y),8);
				}
				
				if(NEXT){
					NEXT = 0;
					screen_ani ^= 1;
				}
		
		
	}
}*/



