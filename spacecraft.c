#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#include "global_vars.h"

//gli esami del CdL in Informatica determinano i nomi delle navicelle nemiche
char exam[85] = {"PR1 FDI M_D AST ARE CDI FIS ALF SO1 CSM EDI R_C PR2 STI AMM LIP BD1 IUM PSI M_R ISW"};

void InitSpacecraft(GameObject *temp, int i, int level) //inizializza il GambeObject della spacecraft
{
    int r, j;
    int size;

    size = level + 2;

    temp->num = i;
    temp->iAm = SPACECRAFT;
    temp->pid = getpid();

    r = (getpid()*(random()%100))%21; //un semplice numero random causava l'estrazione di un numero random uguale per tutti i task

    for(j = 0; j < 3; j++)
    {
        temp->name[j] = exam[(r*4)+j];
    }

    if(temp->num < 5)
    {
        temp->y = 1;
        temp->step = 1;
        temp->x = ((temp->num + 1) * ((MAXX - ((N_SPACECRAFT/2) * size)) / ((N_SPACECRAFT/2) + 1))) + (temp->num * size); //posiziona le spacecraft correttamente distanziate tra loro
    }
    else
    {
        temp->y = 8;
        temp->step = -1;
        temp->x = (((temp->num - 5) + 1) * ((MAXX - ((N_SPACECRAFT/2) * size)) / ((N_SPACECRAFT/2) + 1))) + ((temp->num - 5) * size); //posiziona le spacecraft correttamente distanziate tra loro
    }
    temp->life = level+1;
    temp->level = level;
}

void GameSpacecraft(int input_pipe, int output_sc_pipe, int i, int level) //funzione di gioco della spacecraft
{
    GameObject temp;
    what msg;
    pid_t pid_missile;
    int step_counter = (getpid()*random()*i)%300;
    //un semplice random()%300 causava la medesiva scelta di numero da parte di tutte le spacecraft
    int backup_x, backup_y;

    InitSpacecraft(&temp, i, level);

    write(input_pipe, &temp, sizeof(GameObject));

    while(TRUE)
    {
        read(output_sc_pipe, &msg, sizeof(what));

        if(msg == SHOOT) //la spacecraft è stata colpita da missile
            temp.life -= 1;

        if(temp.life == 0)
        {
            temp.x = OBJECT_DELETE;
            write(input_pipe, &temp, sizeof(GameObject));
            //invio un ulteriore GameObject sulla pipe per permettere la registrazione della morte da parte di Engine
        }

        if(msg == COLLISION)
        { //in caso di vicinanza con un altra spacecraft inverte le rotta
            temp.step = -temp.step;
            temp.x = backup_x; //se Engine ha trovato una collisione, ignora le coordinate che generano la collisione
            temp.y = backup_y; //c'è bisogno di ripristinarle con il backup
            temp.x += temp.step;
        }

        if(temp.x > (MAXX-(temp.level+3)) || temp.x < 2)
        { //in caso di vicinanza con il bordo inverte le rotta
            temp.step = - temp.step;
            temp.x += temp.step;
        }

        switch(temp.level)
        { //a seconda del livello della spacecraft aumenta la frequenza con cui vengono generati missili
        case 1:
            if(step_counter == 150)
                switch(pid_missile = fork())
                {
                case -1:
                    ErrorProcess();
                case 0:
                    EnemyMissile(input_pipe, temp.x, temp.y, i, 1);
                }
            break;
        case 2:
            if((step_counter == 150) || (step_counter == 300))
                switch(pid_missile = fork())
                {
                case -1:
                    ErrorProcess();
                case 0:
                    EnemyMissile(input_pipe, temp.x, temp.y, i, 2);
                }
            break;
        case 3:
            if((step_counter == 100) || (step_counter == 200) || (step_counter == 300))
                switch(pid_missile = fork())
                {
                case -1:
                    ErrorProcess();
                case 0:
                    EnemyMissile(input_pipe, temp.x, temp.y, i, 3);
                }
            break;
        }

        if(msg == NOTHING) //solo nel caso in cui sia tutto regolare attende prima di riprendere, altrimenti serve una risposta celere
        {
            if(step_counter == 150)
                temp.y += temp.level; //scende verticalmente di un numero di righe pari al livello
            /* non è stato ritenuto necessario aumentare la frequenza di discesa a seconda del livello perché
                aumenta invece la velocità di movimento quindi è come se aumentasse la frequenza di discesa */

            temp.x += temp.step; //sposta orizzontalmente l'oggetto
            switch(temp.level)
            { //più il livello è alto più sarà veloce nel spostarsi
            case 1:
                usleep(100000);
                break;
            case 2:
                usleep(75000);
                break;
            case 3:
                usleep(50000);
                break;
            }
            step_counter++;
            if(step_counter > 300)
                step_counter = 0; //ricomincia a contare da 0
            backup_x = temp.x; //backup della posizione
            backup_y = temp.y;
        }
        write(input_pipe, &temp, sizeof(GameObject));
    }
}
