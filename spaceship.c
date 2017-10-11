#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "global_vars.h"

void InitSpaceship(GameObject *temp) //inizializza il GambeObject della spaceship
{
    temp->iAm = SPACESHIP;
    temp->pid = getpid();
    temp->x = ((MAXX/2) -3); //centrata nella riga in basso dello schermo
    temp->y = MAXY - 6;
    temp->life = 3;
    temp->level = -1;

    WriteLife(3); //aggiorna il video delle vite
}

void GameSpaceship(int input_pipe, int output_ss_pipe)
{
    GameObject temp;
    what msg;
    char c;
    int i;
    pid_t pid_missile[N_MISSILE];

    InitSpaceship(&temp);

    write(input_pipe, &temp, sizeof(GameObject));

    while(TRUE)
    {
        read(output_ss_pipe, &msg, sizeof(what));

        if(msg == SHOOT) //la spaceship è stata colpita da missile
            temp.life -= 1;

        if(temp.life == 0)
        {
            temp.x = OBJECT_DELETE;
            write(input_pipe, &temp, sizeof(GameObject));
            //invio un ulteriore GameObject sulla pipe per permettere la registrazione della morte da parte di Engine
        }

        switch(c = getch())
        { //la spaceship si sposta con input da tastiera
        case LEFT:
            if(temp.x > 1) //se non supera il bordo
                temp.x -= 1; //permette lo spostamento a sinistra
            break;

        case RIGHT:
            if(temp.x < MAXX-6) //se non supera il bordo
                temp.x += 1; //permette lo spostamento a destra
            break;
        case SPACE:
            if((waitpid(pid_missile[0], NULL, WNOHANG)) && (waitpid(pid_missile[1], NULL, WNOHANG))) //non permette missili multipli, solo 2 per volta come nel gioco originale
                for(i = 0; i < N_MISSILE; i++) //2 cicli
                    switch(pid_missile[i] = fork()) //di conseguenza vengono generati 2 processi
                    {
                    case -1:
                        ErrorProcess();
                    case 0:
                        GameMissile(input_pipe, temp.x, temp.y, i);
                        //richiamo della funzione di gioco deli missili. I parametri servono per avere il descrittore in scrittura e per conoscere il punto di partenza
                    }
            break;
        }
        write(input_pipe, &temp, sizeof(GameObject));
    }
}
