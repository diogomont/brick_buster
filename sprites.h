/*
 * Exported with nin10kit v1.7
 * Invocation command was nin10kit --mode=3 sprites background_green.jpg ball1.jpg black.jpg block_blue.jpg game_over.jpg player1.jpg splash.jpg 
 * Time-stamp: Wednesday 11/08/2017, 18:42:33
 * 
 * Image Information
 * -----------------
 * background_green.jpg 160@160
 * ball1.jpg 6@5
 * black.jpg 240@160
 * block_blue.jpg 16@8
 * game_over.jpg 240@160
 * player1.jpg 24@8
 * splash.jpg 240@160
 * 
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef SPRITES_H
#define SPRITES_H

extern const unsigned short background_green[25600];
#define BACKGROUND_GREEN_SIZE 51200
#define BACKGROUND_GREEN_LENGTH 25600
#define BACKGROUND_GREEN_WIDTH 160
#define BACKGROUND_GREEN_HEIGHT 160

extern const unsigned short ball1[30];
#define BALL1_SIZE 60
#define BALL1_LENGTH 30
#define BALL1_WIDTH 6
#define BALL1_HEIGHT 5

extern const unsigned short black[38400];
#define BLACK_SIZE 76800
#define BLACK_LENGTH 38400
#define BLACK_WIDTH 240
#define BLACK_HEIGHT 160

extern const unsigned short block_blue[128];
#define BLOCK_BLUE_SIZE 256
#define BLOCK_BLUE_LENGTH 128
#define BLOCK_BLUE_WIDTH 16
#define BLOCK_BLUE_HEIGHT 8

extern const unsigned short game_over[38400];
#define GAME_OVER_SIZE 76800
#define GAME_OVER_LENGTH 38400
#define GAME_OVER_WIDTH 240
#define GAME_OVER_HEIGHT 160

extern const unsigned short player1[192];
#define PLAYER1_SIZE 384
#define PLAYER1_LENGTH 192
#define PLAYER1_WIDTH 24
#define PLAYER1_HEIGHT 8

extern const unsigned short splash[38400];
#define SPLASH_SIZE 76800
#define SPLASH_LENGTH 38400
#define SPLASH_WIDTH 240
#define SPLASH_HEIGHT 160

#endif

