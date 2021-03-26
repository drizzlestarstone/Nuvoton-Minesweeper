///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Library: Seven Segment Display (RTOS)                                                                              //
//                                                                                                                   //
//Prepared by:                                                                                                       //
//                                                                                                                   //
//	1. Tan Wei Hong 22482                                                                                            //
//	2. Ong Jin Cheng 22487                                                                                           //
//                                                                                                                   //
//for Real-Time Microcontroller System (RTMS) on 6 December 2019.                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int DelayTime, seg_b0, seg_b1, seg_b2, seg_b3, disable1, disable2, disable3, disable4;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void segment_value(int number)     //  Handles which segment to be lit up when input digits came in. 
	{                                //  Segment LEDs are active-low.
		switch (number)
		{
			case 0: PE3 = 0; PE4 = 0; PE0 = 0; PE5 = 0; PE6 = 0; PE2 = 0; PE7 = 1; PE1 = 1;
					break;
			case 1: PE4 = 0; PE0 = 0; PE3 = 1; PE5 = 1; PE6 = 1; PE2 = 1; PE7 = 1; PE1 = 1;
					break;
			case 2: PE3 = 0; PE4 = 0; PE0 = 1; PE5 = 0; PE6 = 0; PE2 = 1; PE7 = 0; PE1 = 1;
					break;
			case 3: PE3 = 0; PE4 = 0; PE0 = 0; PE5 = 0; PE6 = 1; PE2 = 1; PE7 = 0; PE1 = 1;
					break;
			case 4: PE3 = 1; PE4 = 0; PE0 = 0; PE5 = 1; PE6 = 1; PE2 = 0; PE7 = 0; PE1 = 1;
					break;
			case 5: PE3 = 0; PE4 = 1; PE0 = 0; PE5 = 0; PE6 = 1; PE2 = 0; PE7 = 0; PE1 = 1;
					break;
			case 6: PE3 = 0; PE4 = 1; PE0 = 0; PE5 = 0; PE6 = 0; PE2 = 0; PE7 = 0; PE1 = 1;
					break;
			case 7: PE3 = 0; PE4 = 0; PE0 = 0; PE5 = 1; PE6 = 1; PE2 = 1; PE7 = 1; PE1 = 1;
					break;
			case 8: PE3 = 0; PE4 = 0; PE0 = 0; PE5 = 0; PE6 = 0; PE2 = 0; PE7 = 0; PE1 = 1;
					break;
			case 9: PE3 = 0; PE4 = 0; PE0 = 0; PE5 = 0; PE6 = 1; PE2 = 0; PE7 = 0; PE1 = 1;
					break;
			default: PE3 = 1; PE4 = 1; PE0 = 1; PE5 = 1; PE6 = 1; PE2 = 1; PE7 = 1; PE1 = 0;
					break;
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void seven_segment_begin(void)     //   Handles digits display and which block to be lit up according to which position
{                                  //   the digit should be.
	int block;
	while(1){
		for (block = 3; block >= 0; block--)
		{
			switch (block)
			{
				case 0: if (disable1) {PC4 = 0; PC5 = 0; PC6 = 0; PC7 = 0;} else {PC4 = 1; PC5 = 0; PC6 = 0; PC7 = 0;}
						segment_value(seg_b0);
						break;
				case 1: if (disable2) {PC4 = 0; PC5 = 0; PC6 = 0; PC7 = 0;} else {PC4 = 0; PC5 = 1; PC6 = 0; PC7 = 0;}
						segment_value(seg_b1);
						break;
				case 2: if (disable3) {PC4 = 0; PC5 = 0; PC6 = 0; PC7 = 0;} else {PC4 = 0; PC5 = 0; PC6 = 1; PC7 = 0;}
						segment_value(seg_b2);
						break;
				case 3: if (disable4) {PC4 = 0; PC5 = 0; PC6 = 0; PC7 = 0;} else {PC4 = 0; PC5 = 0; PC6 = 0; PC7 = 1;}
						segment_value(seg_b3);
						break;
				default: PC4 = 0; PC5 = 0; PC6 = 0; PC7 = 0;
						 break;
			}
			osDelay(1);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void seven_segment_digit_input(int seg_block, int number, int state)     //   Inputs Receive: 
{                                                                        //
	switch (seg_block)                                                     //   1. Which block the digit belongs to.
		{                                                                    //   2. Which digit to be displayed.
			case 0: seg_b0 = number;                                           //   3. Display the digits? 1 = ON, 0 = OFF.
					if (state) disable1 = 0; else disable1 = 1;                    //
					break;
			case 1: seg_b1 = number;
					if (state) disable2 = 0; else disable2 = 1;
					break;
			case 2: seg_b2 = number;
					if (state) disable3 = 0; else disable3 = 1;
					break;
			case 3: seg_b3 = number;
					if (state) disable4 = 0; else disable4 = 1;
					break;
		}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void seven_segment_value_input(int integer_input)                        //   Input Receive:
{                                                                        //
	int integer_digit, block_state;                                        //   1. Any value to be displayed in integer 
	                                                                       //      data type.
	integer_digit = integer_input%10000/1000;                              //
	if (integer_input>=1000) block_state=1; else block_state=0;            //   The digits position of the value will be 
	seven_segment_digit_input(3,integer_digit,block_state);                //   automatically assigned to the block they
	                                                                       //   belongs to. Not related block will be
	integer_digit = integer_input%1000/100;                                //   turned off automatically.
	if (integer_input>=100) block_state=1; else block_state=0;
	seven_segment_digit_input(2,integer_digit,block_state);
	
	integer_digit = integer_input%100/10;
	if (integer_input>=10) block_state=1; else block_state=0;
	seven_segment_digit_input(1,integer_digit,block_state);
	
	integer_digit = integer_input%10/1;
	if (integer_input>=0) block_state=1; else block_state=0;
	seven_segment_digit_input(0,integer_digit,block_state);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
