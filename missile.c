#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#include "global_vars.h"

void InitEnemyMissile(GameObject *temp, int x, int y, int num, int level) //inizializza il GambeObject del missile
{
    temp->iAm = ENEMY_MISSILE;
    temp->pid = getpid();
    temp->num = num;

    switch(level)
    { //in base al livello il missile parte con coordinate centrali rispetto alla navicella
    case 1:
        temp->x = x + 1;
        temp->y = y + 3;
        break;
    case 2:
        temp->x = x + 2;
        temp->y = y + 4;
        break;
    case 3:
        temp->x = x + 2;
        temp->y = y + 5;
        break;
    }
    temp->life = 1;
    temp->level = -1;
}

void EnemyMissile(int input_pipe, int x, int y, int num, int level) //funzione di gioco del missile
{
    GameObject temp;

    InitEnemyMissile(&temp, x, y, num, level);

    write(input_pipe, &temp, sizeof(GameObject));
    usleep(60000);

    while(TRUE)
    {
        temp.y++; //scende di una posizione

        write(input_pipe, &temp, sizeof(GameObject));
        usleep(60000);
    }
}

void InitMissile(GameObject *temp, int x, int y, int num) //inizializza il GambeObject del missile
{
    temp->iAm = MISSILE;
    temp->pid = getpid();
    temp->num = num;
    if(temp->num == 0)
    { //posiziona il proiettile che si sposta in diagonale verso l'angolo alto/sinitro
        temp->x = x+2;
        temp->y = y-1;
        temp->direction = SX;
    }
    if(temp->num == 1)
    { //posiziona il proiettile che si sposta in diagonale verso l'angolo alto/destro
        temp->x = x+3;
        temp->y = y-1;
        temp->direction = DX;
    }
    if(temp->num == 2)
    {
        temp->x = x+2;
        temp->y = y+1;
    }
    temp->life = 1;
    temp->level = -1;
}

void GameMissile(int input_pipe, int x, int y, int num) //funzione di gioco del missile
{
    GameObject temp;

    InitMissile(&temp, x, y, num);

    write(input_pipe, &temp, sizeof(GameObject));
    usleep(30000);

    while(temp.y > 1)
    {
        if((temp.x - 1) == 0) //rimbalzo sul muro sinistro
            temp.direction = DX;

        if((temp.x + 1) == MAXX) //rimbalzo sul muro destro
            temp.direction = SX;

        if(temp.direction == SX)
            temp.x--;
        else
            temp.x++; //si sposta lateralmente di una posizione

        temp.y--; //sale di una posizione

        write(input_pipe, &temp, sizeof(GameObject));
        usleep(30000);
    }

    /* Il missile ha finito la sua corsa
    non verrà più stampato a video, ma inviamo lo stesso un ultima volta i dati nella pipe
    per permettere la cancellazione delle ultime coordinate che altrimenti rimarrebbero
    visualizzate nello schemo */
    temp.y = 0;
    write(input_pipe, &temp, sizeof(GameObject));
    exit(1);
}
