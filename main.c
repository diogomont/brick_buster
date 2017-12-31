#include <math.h>
#include "myLib.h"
#include "sprites.h"

typedef struct {
	const u16* image;
	u8 x;
	u8 y;
	u8 alive;
} BRICK;

typedef struct {
	double x;
	double y;
	double speed;
	s16 direction;
	double dx;
	double dy;
	u8 alive;
} BALL;

typedef struct {
	s16 x;
	s16 y;
	s16 speed;
	u8 lives;
} PLAYER;

// globals
u16 *videoBuffer = (u16*)0x6000000;
BRICK brickgrid[BRICKGRID_SIZE];
BALL ball;
PLAYER player;

// ====================================================================

int main()
{
	REG_DISPCNT = MODE3 | BG2_ENABLE;

	// game starts at the splash screen
	splash_screen();

	while(TRUE) {

		while (KEY_DOWN_NOW(BUTTON_SELECT));

		// wait for SCANLINECOUNTER to be at vsync to avoid tearing
		wait_for_vsync();

		// restart after losing life
		if (ball.alive == FALSE && ball.y > 180) {
			// clear the previous player and ball sprites
			draw_background(ball.x, ball.y, background_green, BALL1_WIDTH, BALL1_HEIGHT);
			draw_background(player.x, player.y, background_green, PLAYER1_WIDTH, PLAYER1_HEIGHT);

			--player.lives;
			if (player.lives > 0)
				new_life();
			else
				game_over_screen();
		}

		// first thing to do in a new frame is to redraw the background at
		// the previous frame's positions of the player and ball
		draw_background(ball.x, ball.y, background_green, BALL1_WIDTH, BALL1_HEIGHT);
		draw_background(player.x, player.y, background_green, PLAYER1_WIDTH, PLAYER1_HEIGHT);

		// check position of the ball of ball relative to the bricks
		s8 row_ball_top = BRICK_ROW(ball.y);
		s8 row_ball_bot = BRICK_ROW(ball.y + BALL1_HEIGHT);

		// =========== check collisions b/w ball and bricks ===========
		if (ball.y >= 20 - BALL1_HEIGHT  && row_ball_top <= 5) {
			s8 col_ball_left = BRICK_COL(ball.x);
			s8 col_ball_right = BRICK_COL(ball.x + BALL1_WIDTH);

			// limit col-ball_right to 8 to prevent killing brick on the next row
			if (col_ball_right > 8)
				col_ball_right = 8;

			s8 killed_a_brick = FALSE;
			// if the ball hits the side of a brick
			if (kill_brick(row_ball_top*9 + col_ball_left) 
					|| kill_brick(row_ball_bot*9 + col_ball_left)) {
				killed_a_brick = TRUE;
			// if the ball hits the bottom/top of a brick
			}
			if (kill_brick(row_ball_top*9 + col_ball_right)
						|| kill_brick(row_ball_bot*9 + col_ball_right)) {
					killed_a_brick = TRUE;
			}

			if (killed_a_brick) {
				if (col_ball_left == col_ball_right) {
					ball.dy = -ball.dy;
				} else {
					if (row_ball_bot == row_ball_top) {
						ball.dx = -ball.dx;
					} else {
						ball.dx = -ball.dx;
						ball.dy = -ball.dy;
					}
				}
			}
		}
		// ============================================================

		// update the player and ball positions
		player.x = player.x + player.speed * player_direction();
		ball.x = (ball.x + ball.dx);
		ball.y = (ball.y + ball.dy);

		// have the ball stick to the player if it hasn't been launched
		if (ball.speed == 0) {
			ball.x = player.x + (0.6*PLAYER1_WIDTH);
		}

		// ================ check if ball is in bounds ================
		if (ball.y < UPPER_BOUND) {
			ball.y = UPPER_BOUND;
			ball.dy = -ball.dy;
		}
		if (ball.y > player.y - BALL1_HEIGHT && ball.alive) {
			if (check_paddle_collision()) {
				// get the new angle of direction of the ball, after hitting the paddle
				ball.direction = new_ball_direction();
				// update the x and y velociies of the ball
				ball.dx = ball.speed * cos(RAD(ball.direction));
				ball.dy = ball.speed * sin(RAD(ball.direction));

				ball.y = player.y - BALL1_HEIGHT;
			}
			// else: stop checking this. Lose a life.
		}

		// if the ball is in a row of bricks, redraw the entire row
		// (optimize this to be more efficient later)
		draw_brick_row(row_ball_top);
		if (row_ball_bot != row_ball_top) draw_brick_row(row_ball_bot);
		
		if (ball.x > RIGHT_BOUND - BALL1_WIDTH) {
			ball.x = RIGHT_BOUND - BALL1_WIDTH;
			ball.dx = -ball.dx;
		}
		if (ball.x < LEFT_BOUND) {
			ball.x = LEFT_BOUND;
			ball.dx = -ball.dx;
		}
		if (player.x > (RIGHT_BOUND - PLAYER1_WIDTH)) {
			player.x = (RIGHT_BOUND - PLAYER1_WIDTH);
		}
		if (player.x < LEFT_BOUND) {
			player.x = LEFT_BOUND;
		}
		// ============================================================


		//draw the player
        draw_no_corners(player.x, player.y, player1, PLAYER1_WIDTH, PLAYER1_HEIGHT);

        // draw the ball
        draw_no_corners((s16) ball.x, (s16) ball.y, ball1, BALL1_WIDTH, BALL1_HEIGHT);

        // launch the ball off the paddle
        if (KEY_DOWN_NOW(BUTTON_A))
        	launch_ball();

        // reset back to the splash screen if select is pressed
		if (KEY_DOWN_NOW(BUTTON_SELECT))
			splash_screen();
	}
}

inline void splash_screen()
{

	draw(0, 0, splash, 240, 160);

	while (KEY_DOWN_NOW(BUTTON_SELECT));
	while (!KEY_DOWN_NOW(BUTTON_SELECT));

	// once select is pressed, start the game
	init_game_values();
	init_brick_array();


	draw(0, 0, black, 240, 160);
	// draw the entire background.
    draw(40, 0, background_green, 160, 160);

    // draw all the blocks
    for (int i = 0; i < BRICKGRID_SIZE; i++) {
    	if(brickgrid[i].alive) draw(brickgrid[i].x, brickgrid[i].y, brickgrid[i].image, BLOCK_BLUE_WIDTH, BLOCK_BLUE_HEIGHT);
    }
}

inline void game_over_screen() {

	draw(0, 0, game_over, 240, 160);

	while (KEY_DOWN_NOW(BUTTON_SELECT));
	while (!KEY_DOWN_NOW(BUTTON_SELECT));

	splash_screen();

}

inline void init_game_values()
{
	player.x = 120 - (0.5 * PLAYER1_WIDTH);
	player.lives = 1;
	new_life();
}

inline void new_life()
{
	player.y = 150;
	player.speed = 1;

	ball.x = player.x + (0.6*PLAYER1_WIDTH);
	ball.speed = 0;
	ball.direction = -50;

	ball.dx = 0;
	ball.dy = 0;

	ball.y = player.y - BALL1_HEIGHT;
	ball.alive = TRUE;
}

inline void init_brick_array()
{
	for (int i = 0; i < BRICKGRID_SIZE; i++) {
		brickgrid[i].image = block_blue;
		// x values: 48 + 16*i 
		brickgrid[i].x = LEFT_BOUND + 1 + (BLOCK_BLUE_WIDTH*(i%9));
		// y values: 20 + 8*i
		brickgrid[i].y = 20 + (BLOCK_BLUE_HEIGHT*(i/9));
		brickgrid[i].alive = TRUE;
	}
}

inline void launch_ball()
{
	if (ball.speed == 0) {
		ball.speed = 3;
		ball.dx = ball.speed * cos(RAD(ball.direction));
		ball.dy = ball.speed * sin(RAD(ball.direction));
	}
}


void wait_for_vsync()
{
	while (SCANLINECOUNTER >= 160);
	while (SCANLINECOUNTER < 160);
}

inline void dma_cpy(u16* dst, const volatile u16* src, u32 count, u8 fixed_src)
{
    DMA[3].cnt = 0;
    DMA[3].src = src;
    DMA[3].dst = dst;
    if (fixed_src) count = count | DMA_SOURCE_FIXED;
    DMA[3].cnt = count | DMA_ON;
}

inline void draw(volatile s16 x, volatile s16 y, const volatile u16* image, u8 width, u8 height)
{
	for (int i = y; i < y + height; i++) {
   	    dma_cpy(videoBuffer + OFFSET(i ,x), image + (i-y)*width, width, 0);
    }
}

inline void draw_background(volatile s16 x, volatile s16 y, const volatile u16* image, u8 width, u8 height)
{
	for (int i = y; i < y + height; i++) {
   	    dma_cpy(videoBuffer + OFFSET(i ,x), image + (i * 160 + x) - 40, width, 0);
    }
}

inline void draw_no_corners(volatile s16 x, volatile s16 y, const volatile u16* image, u8 width, u8 height)
{
	// copy the first row, without the left and right corner.
	dma_cpy(videoBuffer + 1 +  OFFSET(y ,x), image + 1, width-2, 0);
	// copy the middle rows of the image normally
	for (int i = 1; i < height-1; i++) {
   	    dma_cpy(videoBuffer + OFFSET(i + y ,x), image + i*width, width, 0);
    }
    // copy the last row, without the left or right corner.
    dma_cpy(videoBuffer + 1 + OFFSET(height-1 + y ,x), image + 1 + (height-1)*width, width-2, 0);
}

inline void draw_brick_row(s8 row_number)
{
	if (row_number < 0 || row_number > BRICKGRID_SIZE/9 - 1) return;
	for (int i = 9*row_number; i < 9*row_number+9; i++) 
    {
		if(brickgrid[i].alive)
			draw(brickgrid[i].x, brickgrid[i].y, brickgrid[i].image, BLOCK_BLUE_WIDTH, BLOCK_BLUE_HEIGHT);
		else
			draw_background(brickgrid[i].x, brickgrid[i].y, background_green, BLOCK_BLUE_WIDTH, BLOCK_BLUE_HEIGHT);
	}
}

// returns true if there was a living brick at the passed in index.
inline u8 kill_brick(s8 index)
{
	if (index < 54) {
		if (brickgrid[index].alive) {
			brickgrid[index].alive = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

inline s8 player_direction() 
{
	return (KEY_DOWN_NOW(BUTTON_LEFT) ? -2 : KEY_DOWN_NOW(BUTTON_RIGHT) ? 2 : 0);
}


inline s8 check_paddle_collision()
{
	if (ball.x < player.x + PLAYER1_WIDTH && ball.x + BALL1_WIDTH > player.x) 
		return 1;
	ball.alive = 0;
	return 0;
}

// left edge of the paddle produces a -75 degree angle of ball movement,
// right edge of the paddle produces a 75 degree angle of ball movement.
inline s16 new_ball_direction()
{
	return (-165 + (150 * (ball.x - (player.x - BALL1_WIDTH)) / (PLAYER1_WIDTH + BALL1_WIDTH)));
}
