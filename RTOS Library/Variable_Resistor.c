///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Library: On-board Variable Resistor (VR1) Analog-Digital-Converter (ADC) (RTOS)                                    //
//                                                                                                                   //
//Prepared by:                                                                                                       //
//                                                                                                                   //
//	1. Tan Wei Hong 22482                                                                                            //
//	2. Ong Jin Cheng 22487                                                                                           //
//                                                                                                                   //
//for Real-Time Microcontroller System (RTMS) on 6 December 2019.                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InitVRADC(void)
{
	PA->OFFD|=0x00800000;                                      // 1. Disable digital input path.
	ADC_CONFIG_CH7(ADC, ADC_ADCHER_PRESEL_EXT_INPUT_SIGNAL);   // 2. Function called from ADC.h. Refer here for further information.

	CLK->CLKSEL1|=CLK_CLKSEL1_ADC_S_Msk;                       // 1. Assign Clock Sel 1 to ADC. Select 22Mhz for ADC.
	CLK->CLKSEL1=2;
	CLK->CLKDIV|=CLK_CLKDIV_ADC_N_Msk;                         // 2. ADC clock source = 22Mhz/2 =11Mhz.
	CLK->APBCLK|=CLK_APBCLK_ADC_EN_Msk;                        // 3. Enable clock source.
	ADC->ADCR|=ADC_ADCR_ADEN_Msk;                              // 4. Enable ADC module.
	
	ADC->ADCR&=~ADC_ADCR_DIFFEN_Msk;                           // 1. Select Operation mode to single end input & single conversion mode.
	ADC->ADCR&=~ADC_ADCR_ADMD_Msk;                             // 2. Note: On-board VR is connected in single end input type. Single conversion
	                                                           //          mode will only do conversion when we request.
	ADC->ADCHER|=ADC_ADCHER_CHEN_Msk;                          // 3. Select and assign ADC Channel to ADC Channel Register.
	ADC->ADCHER=0x80;
	
	ADC->ADSR|=ADC_ADSR_ADF_Msk;                               // 1. Clear the A/D interrupt flags for safe.
	ADC->ADCR|=ADC_ADCR_ADIE_Msk;                              // 2. Enable ADC Interrupt.
	
	ADC->ADCR|=ADC_ADCR_ADST_Msk;                              // 1. Enable Watchdog Timer (WDT) module.
}
                                                             // Note: "|=" set bit (1). "&=~" clear bit (0).
int Variable_Resistor_Value()
{
	int32_t VRV;
	
	while(1)
	{
		while(ADC->ADSR==0);                                     // 1. Wait till conversion flag = 1, conversion is done.
		ADC->ADSR|=ADC_ADSR_ADF_Msk;                             // 2. Write 1 to clear the flag.
		VRV=ADC_GET_CONVERSION_DATA(ADC,7)>>4;                   // 3. Assign ADC output value to variable VRV. ADC_GET_CONVERSION_DATA(), function
		ADC->ADCR|=ADC_ADCR_ADST_Msk;                            //    called from ADC.h. Refer here for further information.
		osDelay(100);                                            // 4. Restart ADC sample.
		return VRV;
	}
}
