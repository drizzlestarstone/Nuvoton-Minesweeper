# Nuvoton-Minesweeper (c)CHAN TIAN WEN, 26TH MARCH 2021

This is a repository for Nuvoton minesweeper code on RTOS. To run this code, you must have

1. A working Nuvoton board
2. RTOS settings on the minesweeper board (for running multiple parallel threads).
- RTOS settings require configuration of Keil > Flash > Configure Flash tools, as well as the Driver folder in NuMicro Pack- 
- The code can also be adapted to run on normal Nuvoton settings. To do that, you will have to analyse the code yourself and restructure the arrangement of the logic flow to appear in the main loop instead of the threads (remove the threads as they can only run in RTOS). 
- Not to worry, each function is well-commented and there should be no problem at all to understand things!

The code files in this repository are:
1. main.c
2. lcd.c (RTOS library)
3. Keypad.c (RTOS library)
4. Seven_Segment_Display.c (RTOS library)
5. Variable_Resistor.c (RTOS library)

If you wish to run the code in normal settings, you can use normal Nuvoton libraries. However, running it on RTOS will require the RTOS libraries.
This game utilizes the on-board keypad, variable resistor, 7 segment display, red LEDs and RGB leds. No other extra components are needed.

You may contact me at rebeccactw1998@gmail.com for any help. Be warned though, that I have a life outside and I may not reply to every email.

Have fun mine-sweeping!
