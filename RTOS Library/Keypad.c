///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Library: 3x3 Keypad Value Return (RTOS)                                                                            //
//                                                                                                                   //
//Prepared by:                                                                                                       //
//                                                                                                                   //
//	1. Tan Wei Hong 22482                                                                                            //
//	2. Ong Jin Cheng 22487                                                                                           //
//                                                                                                                   //
//for Real-Time Microcontroller System (RTMS) on 6 December 2019.                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t Keypad_Return()
{
	uint8_t act[4]={0x3b, 0x3d, 0x3e};    
	uint8_t i, temp,pin;
	
	for(i=0;i<3;i++)
	{
		temp=act[i];
		for(pin=0;pin<6;pin++)
		{
			if((temp&0x01)==0x01)
				GPIO_PIN_DATA(0, pin )=1;
			else
				GPIO_PIN_DATA(0, pin )=0;
			temp>>=1;
		}
		if(PA3==0)
			return(i+1);
		if(PA4==0)
			return(i+4);
		if(PA5==0)
			return(i+7);
	}
		return 0;
}
