/*
 ============================================================================
 Name        : peg-solitaire.c
 Author      : Maurits van der Schee
 Description : Console version of the game "peg-solitaire" for GNU/Linux
 ============================================================================
 */

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>

#define SIZE 9

typedef struct {
	int8_t x,y;
} position_s;

typedef struct {
	int8_t size;
	int8_t field[SIZE][SIZE];
	position_s cursor;
	position_s win;
	bool selected;
} board_s;

void drawBoard(board_s *board) {
	int8_t x,y,count=0;

	printf("\033[H");

	for (y=0;y<(*board).size;y++) {
		for (x=0;x<(*board).size;x++) {
			count+=(*board).field[x][y]=='o';
		}
	}
	printf("peg-solitaire.c %7d pegs\n\n",count);

	for (y=0;y<(*board).size;y++) {
		for (x=0;x<14-(*board).size;x++) {
			printf(" ");
		}
		for (x=0;x<(*board).size;x++) {
			if ((*board).cursor.x == x && (*board).cursor.y == y) {
				printf("\b%c%c%c",(*board).selected?'<':'(',(*board).field[x][y],(*board).selected?'>':')');
			} else {
				printf("%c ",(*board).field[x][y]);
			}
		}
		for (x=0;x<14-(*board).size;x++) {
			printf(" ");
		}
		printf("\n");
	}
	printf("\n");
	printf("     ←,↑,→,↓,q or enter     \n");
	printf("\033[A");
}

void rotateBoard(board_s *board) {
	int8_t i,j,n=(*board).size;
	int8_t tmp;
	for (i=0; i<n/2; i++){
		for (j=i; j<n-i-1; j++){
			tmp = (*board).field[i][j];
			(*board).field[i][j] = (*board).field[j][n-i-1];
			(*board).field[j][n-i-1] = (*board).field[n-i-1][n-j-1];
			(*board).field[n-i-1][n-j-1] = (*board).field[n-j-1][i];
			(*board).field[n-j-1][i] = tmp;
		}
	}
	i = (*board).cursor.x;
	j = (*board).cursor.y;

	(*board).cursor.x = -(j-n/2)+n/2;
	(*board).cursor.y = (i-n/2)+n/2;
}

bool select(board_s *board) {
	int8_t x,y,(*field)[SIZE];
	bool selected;

	x = (*board).cursor.x;
	y = (*board).cursor.y;
	field = (*board).field;
	selected = (*board).selected;

	if (field[x][y]!='o') return false;
	(*board).selected = !selected;
	return true;
}

bool moveUp(board_s *board) {
	int8_t x,y,(*field)[SIZE];
	bool selected;

	x = (*board).cursor.x;
	y = (*board).cursor.y;
	field = (*board).field;
	selected = (*board).selected;

	if (selected) {
		if (y<2) return false;
		if (field[x][y-2]!='.') return false;
		if (field[x][y-1]!='o') return false;
		if (field[x][y-0]!='o') return false;
		field[x][y-2] = 'o';
		field[x][y-1] = '.';
		field[x][y-0] = '.';
		(*board).cursor.y = y-2;
		(*board).selected = false;
	} else {
		if (y<1) return false;
		if (field[x][y-1]==' ') return false;
		(*board).cursor.y = y-1;
	}
	return true;
}

bool moveLeft(board_s *board) {
	bool success;
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}

bool moveDown(board_s *board) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}

bool moveRight(board_s *board) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	return success;
}

bool won(board_s *board) {
	int8_t x,y;
	int8_t count=0;
	position_s last;

	for (x=0;x<(*board).size;x++) {
		for (y=0;y<(*board).size;y++) {
			if ((*board).field[x][y]=='o') {
				count++;
				last.x = x;
				last.y = y;
			}
		}
	}
	return count==1&&last.x==(*board).win.x&&last.y==(*board).win.y;
}

int8_t validMovesUp(board_s *board) {
	int8_t x,y;
	int8_t count=0;
	for (x=0;x<(*board).size;x++) {
		for (y=(*board).size-1;y>1;y--) {
			if ((*board).field[x][y]=='o') {
				if ((*board).field[x][y-1]=='o') {
					if ((*board).field[x][y-2]=='.') {
						count++;
					}
				}
			}
		}
	}
	return count;
}

bool gameEnded(board_s *board) {
	int8_t i,count=0;
	for (i=0;i<4;i++) {
		count+=validMovesUp(board);
		rotateBoard(board);
	}
	return count==0;
}

void initialize(board_s *board, int8_t layout) {
	int8_t x,y;
	int8_t configuration[5][SIZE][SIZE*2] = {{
		"                  ",
		"      o o o       ",
		"    o o o o o     ",
		"  o o o . o o o   ",
		"  o o o o o o o   ",
		"  o o o o o o o   ",
		"    o o o o o     ",
		"      o o o       ",
		"                  "
	},{
		"      o o o       ",
		"      o o o       ",
		"      o o o       ",
		"o o o o o o o o o ",
		"o o o o . o o o o ",
		"o o o o o o o o o ",
		"      o o o       ",
		"      o o o       ",
		"      o o o       "
	},{
		"      o o o       ",
		"      o o o       ",
		"      o o o       ",
		"  o o o o o o o o ",
		"  o o o . o o o o ",
		"  o o o o o o o o ",
		"      o o o       ",
		"      o o o       ",
		"                  "
	},{
		"                  ",
		"      o o o       ",
		"      o o o       ",
		"  o o o o o o o   ",
		"  o o o . o o o   ",
		"  o o o o o o o   ",
		"      o o o       ",
		"      o o o       ",
		"                  "
	},{
		"        o         ",
		"      o o o       ",
		"    o o o o o     ",
		"  o o o o o o o   ",
		"o o o o . o o o o ",
		"  o o o o o o o   ",
		"    o o o o o     ",
		"      o o o       ",
		"        o         "
	}};
	(*board).size = 9;
	(*board).cursor.x = 4;
	(*board).cursor.y = 4;
	(*board).win.x = 4;
	(*board).win.y = (layout==0)?5:4;
	(*board).selected = false;

	memset((*board).field,0,sizeof((*board).field));
	for (y=0;y<(*board).size;y++) {
		for (x=0;x<(*board).size;x++) {
			(*board).field[x][y]=configuration[layout][y][x*2];
		}
	}
}

void setBufferedInput(bool enable) {
	static bool enabled = true;
	static struct termios old;
	struct termios new;

	if (enable && !enabled) {
		// restore the former settings
		tcsetattr(STDIN_FILENO,TCSANOW,&old);
		// set the new state
		enabled = true;
	} else if (!enable && enabled) {
		// get the terminal settings for standard input
		tcgetattr(STDIN_FILENO,&new);
		// we want to keep the old setting to restore them at the end
		old = new;
		// disable canonical mode (buffered i/o) and local echo
		new.c_lflag &=(~ICANON & ~ECHO);
		// set the new settings immediately
		tcsetattr(STDIN_FILENO,TCSANOW,&new);
		// set the new state
		enabled = false;
	}
}

void signal_callback_handler(int signum) {
	printf("         TERMINATED         \n");
	setBufferedInput(true);
	printf("\033[?25h");
	exit(signum);
}

int main(int argc, char *argv[]) {
	board_s board_alloc,*board=&board_alloc;
	char c;
	bool success;
	int8_t layout = 3;

	if (true&& argc == 2 && strcmp(argv[1],"french")==0) {
		layout = 0;
	} else if (argc == 2 && strcmp(argv[1],"german")==0) {
		layout = 1;
	} else if (argc == 2 && strcmp(argv[1],"asymmetric")==0) {
		layout = 2;
	} else if (argc == 2 && strcmp(argv[1],"english")==0) {
		layout = 3;
	} else if (argc == 2 && strcmp(argv[1],"diamond")==0) {
		layout = 4;
	}

	printf("\033[?25l\033[2J\033[H");

	// register signal handler for when ctrl-c is pressed
	signal(SIGINT, signal_callback_handler);

	initialize(board,layout);
	drawBoard(board);
	setBufferedInput(false);
	while (true) {
		c=getchar();
		switch(c) {
			case 97:	// 'a' key
			case 104:	// 'h' key
			case 68:	// left arrow
				success = moveLeft(board);  break;
			case 100:	// 'd' key
			case 108:	// 'l' key
			case 67:	// right arrow
				success = moveRight(board); break;
			case 119:	// 'w' key
			case 107:	// 'k' key
			case 65:	// up arrow
				success = moveUp(board);    break;
			case 115:	// 's' key
			case 106:	// 'j' key
			case 66:	// down arrow
				success = moveDown(board);  break;
			case 10:	// enter key
			case 13:	// enter key
				success = select(board);    break;
			default: success = false;
		}
		if (success) {
			drawBoard(board);
			if (won(board)) {
				printf("          YOU WON           \n");
				break;
			}
			if (gameEnded(board)) {
				printf("         GAME OVER          \n");
				break;
			}
		}
		if (c=='q') {
			printf("        QUIT? (y/n)         \n");
			c=getchar();
			if (c=='y'){
				break;
			}
			drawBoard(board);
		}
		if (c=='r') {
			printf("       RESTART? (y/n)       \n");
			c=getchar();
			if (c=='y'){
				initialize(board,layout);
			}
			drawBoard(board);
		}
	}
	setBufferedInput(true);
	printf("\033[?25h");

	return EXIT_SUCCESS;
}
