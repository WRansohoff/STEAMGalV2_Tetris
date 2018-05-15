# Overview

This is firmware for a 'tetris' game on a V2 'STEAMGal' board driven by an STM32 chip - now with a color display!

# Current Status

Working, but incomplete. The firmware initializes the system clock to 48MHz driven by the HSI oscillator, then draws a simple menu to the OLED screen.

It sets up hardware interrupts for each of the 6 buttons - the 'A' button selects the test menu's start menu to start the game, and the 'Up' button returns to the menu from within the game. I'm hoping to add a 'pause' menu, though.

The onboard LED blinks on and off each game 'tick', which causes the current block to drop if it can, and fix in place on the grid if not. A 'game over' happens when a brick gets fixed in place while part of it is above the top line. Rows are cleared if necessary when a brick is fixed in place.

But there's no scoring, the game doesn't get faster as it progresses, there's no "next brick" display, etc. Just the basics.

Currently, only the STM32F051K8 and STM32F031K6 are supported, but I hope to add the STM32F303K8 as well if time permits.

Based off of a similar firmware for an earlier revision of the board; I should probably merge this with the other project and support multiple boards, but I don't know if it's worth continuing to use the monochrome displays for this sort of board; the lack of color is pretty limiting:

https://github.com/WRansohoff/STEAMGal_Tetris
