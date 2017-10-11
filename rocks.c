#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#include "global_vars.h"

void InitRocks(GameObject *temp, int i) //inizializza il GambeObject della roccia
{
    temp->num = i;
    temp->iAm = ROCK;
	temp->pid = getpid();
	temp->x = ((temp->num+1) * ((MAXX - (N_ROCKS * 6)) / (N_ROCKS + 1))) + (temp->num * 6); //posiziona le rocce correttamente distanziate tra loro
	temp->y = MAXY-10;
	temp->life = 4;
	temp->level = 0;
}

void GameRocks(int input_pipe, int output_rk_pipe, int i) //funzione di gioco della roccia
{
    GameObject temp;
    what msg;

    InitRocks(&temp, i);

    write(input_pipe, &temp, sizeof(GameObject));

    while(TRUE)
    {
        read(output_rk_pipe, &msg, sizeof(what));

        if(msg == SHOOT) //la roccia è stata colpita da missile
            temp.life -= 1;

        if(temp.life == 0)
        {
            temp.x = OBJECT_DELETE;
            write(input_pipe, &temp, sizeof(GameObject));
            //invio un ulteriore GameObject sulla pipe per permettere la registrazione della morte da parte di Engine
        }

        if(msg == NOTHING) //solo nel caso in cui sia tutto regolare attende prima di riprendere, altrimenti serve una risposta celere
            usleep(100000);

        write(input_pipe, &temp, sizeof(GameObject));
    }
}
