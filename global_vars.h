#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define SPACE 32
#define UP 65
#define DOWN 66
#define LEFT 68
#define RIGHT 67

#define OBJECT_DELETE -100

#define MAXX 100
#define MAXY 35

#define SX 0
#define DX 1

#define N_ROCKS 4
#define N_SPACECRAFT 10
#define N_MISSILE 2

#define SCORE_DIM 3

typedef enum who {SPACESHIP, SPACECRAFT, ROCK, MISSILE, ENEMY_MISSILE} who;

typedef enum what {SHOOT, COLLISION, NOTHING} what;

typedef struct GameObject //ogni elemento del gioco può essere rappresentato con questa struttura
{
	who iAm;
	pid_t pid;
	char name[4];
	int num;
	int direction;
	int step;
	int x;
	int y;
	int life;
	int level;
}GameObject;

//stringhe di servizio per la stampa a video di informazioni
char score_string[10];
char life_string[2];
char level_string[2];

//le prossime sono pipe
int input_pipe[2];
int output_ss_pipe[2];
int output_sc_pipe[N_SPACECRAFT][2];
int output_rk_pipe[N_ROCKS][2];
