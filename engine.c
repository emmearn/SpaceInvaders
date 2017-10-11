#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#include "global_vars.h"

void ErrorProcess() //funzione richiamata in caso di errore nella creazione di processi
{
    perror("Errore nell'esecuzione della fork.");
    exit(1);
}

void ProcessGenRocks() //genera i processi roccia
{
    pid_t pid;
    int i;

    for(i = 0; i < N_ROCKS-1; i++) //3 cicli
    {
        switch(pid = fork()) //di conseguenza qui vengono generati 3 processi
        {
        case -1:
            ErrorProcess();
        case 0:
            close(input_pipe[0]); //chiuso il descrittore in lettura
            close(output_rk_pipe[i][1]); //chiuso il descrittore in scrittura
            GameRocks(input_pipe[1], output_rk_pipe[i][0], i); //il processo figlio diventa una roccia
            break;
        default:
            continue; //il quarto processo cicla e genera i figli, finito il for esce
        }
    }
    close(input_pipe[0]); //chiuso il descrittore in lettura
    close(output_rk_pipe[N_ROCKS-1][1]); //chiuso il descrittore in scrittura
    GameRocks(input_pipe[1], output_rk_pipe[N_ROCKS-1][0], N_ROCKS-1);//anche l'N-esimo processo viene utilizzato al pari dei processi figli
}

void ProcessGenSpacecraft(int level) //funzione che genera i processi navicella nemica
{
    pid_t pid;
    int i;

    for(i = 0; i < N_SPACECRAFT-1; i++) //9 cicli
    {
        switch(pid = fork()) //di conseguenza qui vengono generati 9 processi
        {
        case -1:
            ErrorProcess();
            break;
        case 0:
            close(input_pipe[0]); //chiuso il descrittore in lettura
            close(output_sc_pipe[i][1]); //chiuso il descrittore in scrittura
            GameSpacecraft(input_pipe[1], output_sc_pipe[i][0], i, level); //il processo figlio diventa una navicella nemica
            break;
        default:
            continue; //il decimo processo cicla e genera i figli, finito il for esce
        }
    }

    /*  La parte di codice che segue deriva da una particolare scelta di implementazione:
        la prima volta che questa funzione viene richiamata, avrà come parametro 1 per generare
        navicelle di livello 1 e questo viene fatto nel main. In questo caso non c'è bisogno di un processo
        che esce da questa funzione e torna nel main perché questa chiamata di funzione viene fatta nel ramo
        figlio di una fork che risiede nel main stesso.
        Nel caso in cui le navicelle nemiche vengono richieste di livello superiore a 1, la chiamata viene fatta da engine.
        Di conseguenza c'è bisogno di un 11-esimo processo che esce da questa funzione e torna a gestire engine. */
    if(level > 1)
    {
        switch(pid = fork())
        {
        case -1:
            ErrorProcess();
            break;
        case 0:
            close(input_pipe[0]); //chiuso il descrittore in lettura
            close(output_sc_pipe[N_SPACECRAFT-1][1]); //chiuso il descrittore in scrittura
            GameSpacecraft(input_pipe[1], output_sc_pipe[N_SPACECRAFT-1][0], N_SPACECRAFT-1, level); //il processo figlio diventa una navicella nemica
        }
    }
    else
    {
        close(input_pipe[0]); //chiuso il descrittore in lettura
        close(output_sc_pipe[N_SPACECRAFT-1][1]); //chiuso il descrittore in scrittura
        GameSpacecraft(input_pipe[1], output_sc_pipe[N_SPACECRAFT-1][0], N_SPACECRAFT-1, level); //anche l'N-esimo processo viene utilizzato al pari dei processi figli
    }
}

int funct_shoot(int spacecraft_x, int spacecraft_y, int missile_x, int missile_y, int max_x, int max_y)
{
    //confronta una matrice di coppie di coordinate con una coppia di coordinate e restituisce 1 se trova sovrapposizioni
    int i, j;

    for(i = 0; i < max_y; i++)
        for(j = 0; j < max_x; j++) //entrambi i for servono per scorrere la matrice
            if((spacecraft_x + j == missile_x) && (spacecraft_y + i == missile_y)) //le coordinate coincidono ?
                return TRUE;

    return FALSE;
}

int funct_collision(int object1_x, int object1_y, int object2_x, int object2_y, int object1_max_x, int object1_max_y, int object2_max_x, int object2_max_y) //confronta due coppie di coordinate e restituisce 1 se coincidono
{
    //confronta due matrici di coppie di coordinate e restituisce 1 se trova sovrapposizioni
    int h, i, j, k;

    for(h = 0; h < object2_max_y; h++)
        for(k = 0; k < object2_max_x; k++)
            for(i = 0; i < object1_max_y; i++)
                for(j = 0; j < object1_max_x; j++) //tutti i for servono per scorrere le matrici
                    if((object1_x + j  == object2_x + k) && (object1_y + i == object2_y + h)) //le coordinate coincidono ?
                        return TRUE;

    return FALSE;
}

int all_spacecraft_death(int spacecraft_death[N_SPACECRAFT])
{
    //controlla l'array dove vengono flaggate le morti delle navicelle
    int i;
    for(i = 0; i < N_SPACECRAFT; i++)
        if(spacecraft_death[i] == FALSE) //un flag non è ancora impostato ? cioé c'è ancora una navicella viva ?
            return FALSE;

    return TRUE;
}

int Engine(int input_pipe[2], int output_ss_pipe[2], int output_sc_pipe[N_SPACECRAFT][2], int output_rk_pipe[N_ROCKS][2])
{
    //funzione che comunica, controlla, regola e stampa a video tutti gli altri processi in gioco
    int i;
    int shoot, spaceship_shoots, rocks_shoots[N_ROCKS], spacecraft_shoots[N_SPACECRAFT], spacecraft_death[N_SPACECRAFT];
    GameObject back_sc, enemy_missile[N_SPACECRAFT], spaceship, rocks[N_ROCKS], spacecraft[N_SPACECRAFT], missile[N_MISSILE], info_read;
    what msg;
    pid_t pid;

    /* seguono varie inizializzazioni */
    int score = 0;

    for(i = 0; i < N_SPACECRAFT; i++)
    {
        spacecraft_shoots[i] = FALSE;
        spacecraft_death[i] = FALSE;
    }

    for(i = 0; i < N_ROCKS; i++)
        rocks_shoots[i] = FALSE;

    spaceship.x = OBJECT_DELETE;
    spaceship_shoots = FALSE;

    for(i = 0; i < N_ROCKS; i++)
        rocks[i].x = OBJECT_DELETE;

    for(i = 0; i < N_SPACECRAFT; i++)
    {
        spacecraft[i].x = OBJECT_DELETE;
        enemy_missile[i].x = OBJECT_DELETE;
    }

    for(i = 0; i < N_MISSILE; i++)
        missile[i].x = OBJECT_DELETE;

    do
    {
        //in questa pipe vengono scritti i vari GameObject scritti da tutti i processi in gioco
        read(input_pipe[0], &info_read, sizeof(GameObject));

        switch(info_read.iAm)
        {
        case SPACESHIP: //case della navicella utente
            if(spaceship.x > 0) //serve per capire se è il primo avvio
                DeleteSpaceship(spaceship.y, spaceship.x); //la spaceship viene cancellata

            if(spaceship_shoots == TRUE) //flag del colpo andato a segno da parte di un missile
            {
                msg = SHOOT; //questo messaggio verrà inviato alla navicella
                spaceship_shoots = FALSE; //deflag
            }
            else //se nessun colpo è andato a segno
            {
                msg = NOTHING; //questo messaggio verrà inviato alla navicella
                /* solo in questo caso aggiorno le informazioni lette dalla pipe.
                    In caso contrario c'è bisogno di un informazione celere da parte della navicella.
                    Infatti la navicella risponde immediatamente quando riceve il messaggio SHOOT. */
                spaceship = info_read;
            }
            // ogni lettura sulla input_pipe viene corrisposto da un messaggio sull'outputpipe specifica
            write(output_ss_pipe[1], &msg, sizeof(what));
            DrawSpaceship(spaceship.y, spaceship.x); //la spaceship viene disegnata

            if(spaceship.life == 0)
            {
                //se la navicella ha finito le vita killa tutti i processi
                for(i = 0; i < N_SPACECRAFT; i++)
                {
                    if(spacecraft[i].x > 0)
                        kill(spacecraft[i].pid, 1);
                    if(enemy_missile[i].x > 0)
                        kill(enemy_missile[i].pid, 1);
                }

                if(spaceship.x > 0)
                    kill(spaceship.pid, 1);

                if(missile[0].x > 0)
                    kill(missile[0].pid, 1);

                if(missile[1].x > 0)
                    kill(missile[1].pid, 1);

                for(i = 0; i < N_ROCKS; i++)
                    if(rocks[i].x > 0)
                        kill(rocks[i].pid, 1);

                WriteScore(score); //aggiorna lo score visualizzato
                YouLose(); //termina il gioco comunicando l'esito
                exit(1);
            }
            break;

        case ROCK: //case delle rocce
            if(rocks[info_read.num].x > 0) //serve per capire se è il primo avvio
                DeleteRocks(rocks[info_read.num].y, rocks[info_read.num].x); //la roccia viene cancellata

            if(info_read.life > 0) //se è ancora in vita
            {
                for(i = 0; i < N_SPACECRAFT; i++)
                    if(spacecraft[i].x > 0)
                    {
                        //controllo di collisione tra roccia e navicella
                        shoot = funct_collision(spacecraft[i].x, spacecraft[i].y, rocks[info_read.num].x,  rocks[info_read.num].y, spacecraft[i].level + 2, spacecraft[i].level + 2, 6, 3);
                        if(shoot == TRUE) //se navicella colpisce la roccia
                        {
                            DeleteRocks(rocks[info_read.num].y, rocks[info_read.num].x); //la roccia viene cancellata
                            DeleteSpacecraft(spacecraft[i].y, spacecraft[i].x, spacecraft[i].level+2); //la spacecraft viene cancellata
                            kill(rocks[info_read.num].pid, 1); //la roccia viene killata
                            kill(spacecraft[i].pid, 1); //la spacecraft viene killata
                            rocks[info_read.num].x = OBJECT_DELETE; //la roccia viene spostata dalla tabella di gioco
                            spacecraft[i].x = OBJECT_DELETE; //la navicella viene spostata dalla tabella di gioco
                            /* vengono spostate dalla tabella di gioco perché le coordinate x, y che rimangono memorizzate
                                in questa funzione anche se il processo non è più attivo.
                                Se questo non venisse fatto vedremo missili che colpiscono qualcosa che in verità non c'è */

                            spacecraft_death[info_read.num] = TRUE; //flag morte navicella

                            if((spacecraft[i].level < 3) && (all_spacecraft_death(spacecraft_death) == TRUE))
                            {
                                //se il livello della navicella in questione è inferiore a 3 e tutte le navicelle sono morte
                                ProcessGenSpacecraft(spacecraft[0].level+1); //genera le navicelle del livello successivo

                                WriteLevel(spacecraft[0].level+1); //aggiorna la stampa a video del livello

                                for(i = 0; i < N_SPACECRAFT; i++) //deflagga l'array delle morti
                                    spacecraft_death[i] = FALSE;
                            }
                            else if(all_spacecraft_death(spacecraft_death))
                            {
                                //se il livello della navicella in questione è superiore a 3 e tutte le navicelle sono morte killa tutti i processi
                                for(i = 0; i < N_SPACECRAFT; i++)
                                {
                                    if(spacecraft[i].x > 0)
                                        kill(spacecraft[i].pid, 1);
                                    if(enemy_missile[i].x > 0)
                                        kill(enemy_missile[i].pid, 1);
                                }

                                if(spaceship.x > 0)
                                    kill(spaceship.pid, 1);

                                if(missile[0].x > 0)
                                    kill(missile[0].pid, 1);

                                if(missile[1].x > 0)
                                    kill(missile[1].pid, 1);

                                for(i = 0; i < N_ROCKS; i++)
                                    if(rocks[i].x > 0)
                                        kill(rocks[i].pid, 1);

                                WriteScore(score); //aggiorna lo score visualizzato
                                YouWin(); //termina il gioco comunicando l'esito
                                exit(1);
                            }
                            break;
                        }
                    }

                if(rocks_shoots[info_read.num] == TRUE) //flag del colpo andato a segno da parte di un missile
                {
                    msg = SHOOT; //questo messaggio verrà inviato alla roccia
                    rocks_shoots[info_read.num] = FALSE; //deflag
                }
                else
                {
                    msg = NOTHING; //questo messaggio verrà inviato alla roccia
                    /* solo in questo caso aggiorno le informazioni lette dalla pipe.
                    In caso contrario c'è bisogno di un informazione celere da parte della roccia.
                    Infatti la roccia risponde immediatamente quando riceve il messaggio SHOOT. */
                    rocks[info_read.num] = info_read;
                }
                // ogni lettura sulla input_pipe viene corrisposto da un messaggio sull'outputpipe specifica
                write(output_rk_pipe[info_read.num][1], &msg, sizeof(what));

                if((msg == NOTHING) && (shoot == FALSE))
                    DrawRocks(rocks[info_read.num].y, rocks[info_read.num].x, rocks[info_read.num].life); //la roccia viene disegnata
            }
            else //quanto la roccia termina le vite termina di esistere il processo
                kill(info_read.pid, 1);

            break;

        case SPACECRAFT: //case delle navicelle nemiche
            if(spacecraft[info_read.num].x > 0) //serve per capire se è il primo avvio
                DeleteSpacecraft(spacecraft[info_read.num].y, spacecraft[info_read.num].x, spacecraft[info_read.num].level+2); //la spacecraft viene cancellata

            if(info_read.life > 0) //se è ancora in vita
            {
                spacecraft_death[info_read.num] = FALSE; //aggiorna il flag delle morti per maggior sicurezza
                if(spacecraft_shoots[info_read.num] == TRUE) //flag del colpo andato a segno da parte di un missile
                {
                    msg = SHOOT; //questo messaggio verrà inviato alla navicella
                    spacecraft_shoots[info_read.num] = FALSE; //deflag
                }
                else
                {
                    msg = NOTHING; //questo messaggio verrà inviato alla navicella
                    back_sc = spacecraft[info_read.num]; //backup della GameObject della spacecraft
                    /* solo in questo caso aggiorno le informazioni lette dalla pipe.
                    In caso contrario c'è bisogno di un informazione celere da parte della roccia.
                    Infatti la roccia risponde immediatamente quando riceve il messaggio SHOOT. */
                    spacecraft[info_read.num] = info_read;

                    for(i = 0; i < N_SPACECRAFT; i++)
                        if(i != info_read.num) //scorre tutte le navicelle nemiche a parte se stessa
                            switch(spacecraft[info_read.num].level)
                            {
                            case 1:
                                if(funct_collision(spacecraft[info_read.num].x, spacecraft[info_read.num].y, spacecraft[i].x-1, spacecraft[i].y-1, 3, 3, 5, 5))
                                {
                                    //controllo di collisione tra navicelle
                                    msg = COLLISION; //questo messaggio verrà inviato alla navicella
                                    spacecraft[info_read.num] = back_sc; //bisogna ripristinare le vecchie coordinate per evitare scatti grafici sgradevoli, anche la navicella farà lo stesso
                                }
                                break;
                            case 2:
                                if(funct_collision(spacecraft[info_read.num].x, spacecraft[info_read.num].y, spacecraft[i].x-1, spacecraft[i].y-1, 4, 4, 6, 6))
                                {
                                    //controllo di collisione tra navicelle
                                    msg = COLLISION; //questo messaggio verrà inviato alla navicella
                                    spacecraft[info_read.num] = back_sc; //bisogna ripristinare le vecchie coordinate per evitare scatti grafici sgradevoli, anche la navicella farà lo stesso
                                }
                                break;
                            case 3:
                                if(funct_collision(spacecraft[info_read.num].x, spacecraft[info_read.num].y, spacecraft[i].x-1, spacecraft[i].y-1, 5, 5, 7, 7))
                                {
                                    //controllo di collisione tra navicelle
                                    msg = COLLISION; //questo messaggio verrà inviato alla navicella
                                    spacecraft[info_read.num] = back_sc; //bisogna ripristinare le vecchie coordinate per evitare scatti grafici sgradevoli, anche la navicella farà lo stesso
                                }
                                break;
                            }
                }
                // ogni lettura sulla input_pipe viene corrisposto da un messaggio sull'outputpipe specifica
                write(output_sc_pipe[info_read.num][1], &msg, sizeof(what));

                if(msg == NOTHING) //sempre per evitare strani effetti grafici la stampa viene fatta solo se non c'è nulla da comunicare
                    DrawSpacecraft(spacecraft[info_read.num].y, spacecraft[info_read.num].x, spacecraft[info_read.num].name, spacecraft[info_read.num].level, spacecraft[info_read.num].life); //la spacecraft viene disegnata

                if(((spacecraft[info_read.num].y + spacecraft[info_read.num].level+2 - 1) >= MAXY - 7) && (spacecraft[info_read.num].x > 0))
                {
                    //se la navicella in questione supera la linea difensiva killa tutti i processi
                    for(i = 0; i < N_SPACECRAFT; i++)
                    {
                        if(spacecraft[i].x > 0)
                            kill(spacecraft[i].pid, 1);
                        if(enemy_missile[i].x > 0)
                            kill(enemy_missile[i].pid, 1);
                    }

                    if(spaceship.x > 0)
                        kill(spaceship.pid, 1);

                    if(missile[0].x > 0)
                        kill(missile[0].pid, 1);

                    if(missile[1].x > 0)
                        kill(missile[1].pid, 1);

                    for(i = 0; i < N_ROCKS; i++)
                        if(rocks[i].x > 0)
                            kill(rocks[i].pid, 1);

                    WriteScore(score); //aggiorna lo score visualizzato
                    YouLose(); //termina il gioco comunicando l'esito
                    exit(1);
                }
            }
            else //quanto la spacecraft termina le vite termina di esistere il processo
            {
                kill(info_read.pid, 1);
                spacecraft_death[info_read.num] = TRUE; //flag spacecraft morta

                if((info_read.level < 3) && (all_spacecraft_death(spacecraft_death) == TRUE))
                {
                    //se il livello della navicella in questione è superiore a 3 e tutte le navicelle sono morte killa tutti i processi
                    ProcessGenSpacecraft(info_read.level+1); //genera le navicelle del livello successivo

                    WriteLevel(spacecraft[0].level+1); //aggiorna la stampa a video del livello

                    for(i = 0; i < N_SPACECRAFT; i++) //deflagga l'array delle morti
                        spacecraft_death[i] = FALSE;
                }
                else if(all_spacecraft_death(spacecraft_death))
                {
                    //se il livello della navicella in questione è superiore a 3 e tutte le navicelle sono morte killa tutti i processi
                    for(i = 0; i < N_SPACECRAFT; i++)
                    {
                        if(spacecraft[i].x > 0)
                            kill(spacecraft[i].pid, 1);
                        if(enemy_missile[i].x > 0)
                            kill(enemy_missile[i].pid, 1);
                    }

                    if(spaceship.x > 0)
                        kill(spaceship.pid, 1);

                    if(missile[0].x > 0)
                        kill(missile[0].pid, 1);

                    if(missile[1].x > 0)
                        kill(missile[1].pid, 1);

                    for(i = 0; i < N_ROCKS; i++)
                        if(rocks[i].x > 0)
                            kill(rocks[i].pid, 1);

                    WriteScore(score); //aggiorna lo score visualizzato
                    YouWin(); //termina il gioco e comunicando l'esito
                    exit(1);
                }
            }
            break;

        case MISSILE: //case dei missili
            if(missile[info_read.num].x > 0) //serve per capire se è il primo avvio
                DeleteMissile(missile[info_read.num].y, missile[info_read.num].x); //il missile viene cancellato

            missile[info_read.num] = info_read; //salvo il contenuto letto dalla pipe

            for(i = 0; i < N_SPACECRAFT; i++)
            {
                shoot = funct_shoot(spacecraft[i].x, spacecraft[i].y, missile[info_read.num].x, missile[info_read.num].y, spacecraft[i].level+2, spacecraft[i].level+2);
                if(shoot == TRUE) //missile colpisce navicella nemica
                {
                    DeleteMissile(missile[info_read.num].y, missile[info_read.num].x); //il missile viene cancellato
                    spacecraft_shoots[i] = TRUE; //flag navicella colpita
                    kill(missile[info_read.num].pid, 1); //il missile viene killato

                    if(spacecraft[i].life == 1)
                    {
                        //la spacecraft in seguito a questo colpo morirà
                        DeleteSpacecraft(spacecraft[i].y, spacecraft[i].x, spacecraft[i].level+2); //la spacecraft viene cancellata
                        spacecraft[i].x = OBJECT_DELETE; //viene spostata dal campo di gioco
                        missile[info_read.num].x = OBJECT_DELETE; //viene spostato dal campo di gioco
                        /* vengono spostate dalla tabella di gioco perché le coordinate x, y che rimangono memorizzate
                            in questa funzione anche se il processo non è più attivo.
                            Se questo non venisse fatto vedremo missili che colpiscono qualcosa che in verità non c'è */
                        switch(spacecraft[info_read.num].level)
                        {
                            //aggiorna lo score a seconda del livello della navicella uccisa
                        case 1:
                            score += 100;
                            break;
                        case 2:
                            score += 200;
                            break;
                        case 3:
                            score += 300;
                            break;
                        }
                    }
                    else //la spacecraft in seguito a questo colpo ha ancora vite a disposizione
                        switch(spacecraft[i].level)
                        {
                            //aggiorna lo score a seconda del livello della navicella colpita
                        case 1:
                            score += 10;
                            break;
                        case 2:
                            score += 20;
                            break;
                        case 3:
                            score += 30;
                            break;
                        }

                    break;
                }
            }

            for(i = 0; i < N_ROCKS; i++)
            {
                shoot = funct_shoot(rocks[i].x, rocks[i].y, missile[info_read.num].x, missile[info_read.num].y, 6, 3);
                if(shoot == TRUE) //missile colpisce roccia
                {
                    DeleteMissile(missile[info_read.num].y, missile[info_read.num].x); //il missile viene cancellato
                    rocks_shoots[i] = TRUE; //flag roccia colpita
                    kill(missile[info_read.num].pid, 1); //il missile viene killato

                    if(rocks[i].life == 1)
                    {
                        //la roccia in seguito a questo colpo morirà
                        DeleteRocks(rocks[i].y, rocks[i].x, rocks[i].level); //la roccia viene cancellata
                        rocks[i].x = OBJECT_DELETE; //viene spostata dal campo di gioco
                    }
                    /* vengono spostate dalla tabella di gioco perché le coordinate x, y che rimangono memorizzate
                        in questa funzione anche se il processo non è più attivo.
                        Se questo non venisse fatto vedremo missili che colpiscono qualcosa che in verità non c'è */
                    missile[info_read.num].x = OBJECT_DELETE; //viene spostato dal campo di gioco
                    break;
                }
            }

            if(missile[info_read.num].y == 0) //il missile ha raggiunto la linea superiore dello schermo
            {
                kill(missile[info_read.num].pid, 1); //il missile viene killato
                missile[info_read.num].x = OBJECT_DELETE; //viene spostato dal campo di gioco
            }

            if(shoot == FALSE) //se il missile non è stato colpito
                DrawMissile(missile[info_read.num].y, missile[info_read.num].x); //il missile viene disegnato
            break;

        case ENEMY_MISSILE: //case dei missili nemici
            if(enemy_missile[info_read.num].x > 0) //serve per capire se è il primo avvio
                DeleteMissile(enemy_missile[info_read.num].y, enemy_missile[info_read.num].x); //il missile viene cancellato

            enemy_missile[info_read.num] = info_read; //salvo il contenuto letto dalla pipe

            shoot = FALSE;
            if(spaceship.x > 0)
                shoot = funct_shoot(spaceship.x, spaceship.y, enemy_missile[info_read.num].x, enemy_missile[info_read.num].y, 6, 6);

            if(shoot == TRUE) //missile colpisce navicella
            {
                DeleteMissile(enemy_missile[info_read.num].y, enemy_missile[info_read.num].x); //il missile viene cancellato
                spaceship_shoots = TRUE; //flag navicella colpita
                kill(enemy_missile[info_read.num].pid, 1); //killo il missile

                WriteLife(spaceship.life-1); //aggiorna la stampa a video del livello

                if(spaceship.life == 1)
                { //la spaceship in seguito a questo colpo morirà killa tutti i processi
                    WriteLife(0); //aggiorna la stampa a video del livello
                    refresh();
                    for(i = 0; i < N_SPACECRAFT; i++)
                    {
                        if(spacecraft[i].x > 0)
                            kill(spacecraft[i].pid, 1);
                        if(enemy_missile[i].x > 0)
                            kill(enemy_missile[i].pid, 1);
                    }

                    if(spaceship.x > 0)
                        kill(spaceship.pid, 1);

                    if(missile[0].x > 0)
                        kill(missile[0].pid, 1);

                    if(missile[1].x > 0)
                        kill(missile[1].pid, 1);

                    for(i = 0; i < N_ROCKS; i++)
                        if(rocks[i].x > 0)
                            kill(rocks[i].pid, 1);

                    DeleteSpaceship(spaceship.y, spaceship.x); //la spaceship viene cancellata
                    spaceship.x = OBJECT_DELETE; //la spaceship viene spostata dalla tabella di gioco

                    WriteScore(score); //aggiorna lo score visualizzato
                    YouLose(); //termina il gioco comunicandone l'esito
                    exit(1);
                }

                enemy_missile[info_read.num].x = OBJECT_DELETE; //il missile viene spostato dalla tabella di gioco
                break;
            }

            for(i = 0; i < N_ROCKS; i++)
            {
                shoot = funct_shoot(rocks[i].x, rocks[i].y, enemy_missile[info_read.num].x, enemy_missile[info_read.num].y, 6, 3);
                if(shoot == TRUE)
                { //missile colpisce roccia
                    DeleteMissile(enemy_missile[info_read.num].y, enemy_missile[info_read.num].x); //cancello il missile
                    rocks_shoots[i] = TRUE; //flag navicella colpita
                    kill(enemy_missile[info_read.num].pid, 1); //il processo missile viene killato

                    if(rocks[i].life == 1)
                    { //la roccia in seguito a questo colpo morirà
                        DeleteRocks(rocks[i].y, rocks[i].x); //la roccia viene cancellata
                        rocks[i].x = OBJECT_DELETE; //la roccia viene spostata dalla tabella di gioco
                    }

                    enemy_missile[info_read.num].x = OBJECT_DELETE; //il missile viene spostato dalla tabella di gioco
                    break;
                }
            }

            if(enemy_missile[info_read.num].y == MAXY) //il missile ha raggiunto la linea inferiore dello schermo
            {
                kill(enemy_missile[info_read.num].pid, 1); //il processo missile viene killato
                enemy_missile[info_read.num].x = OBJECT_DELETE; //il missile viene spostato dalla tabella di gioco
            }

            DrawMissile(enemy_missile[info_read.num].y, enemy_missile[info_read.num].x); //il missile viene disegnato
            break;
        }

        WriteScore(score); //aggiorna lo score visualizzato
        refresh();
    }
    while(1);

    return 0;
}
