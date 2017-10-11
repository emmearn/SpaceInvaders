#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#include "global_vars.h"

int main()
{
    int main_pid;
    int i;

    WINDOW *gameWindow;
    InitBoard(gameWindow); //inizializza lo schermo e la grafica

    //la pipe con nome input.. porterà informazioni dai processi navicelle, missili, rocce etc. al processo che di controllo e disegno
    if(pipe(input_pipe)==-1) { perror("Errore nella creazione della pipe."); exit(1); }

    //le pipe con nome output.. porteranno informazioni dal processo di controllo ai processi navicelle, missili, rocce etc.
    if(pipe(output_ss_pipe)==-1) { perror("Errore nella creazione della pipe."); exit(1); }

    for(i = 0; i < N_SPACECRAFT; i++)
        if(pipe(output_sc_pipe[i])==-1) { perror("Errore nella creazione della pipe."); exit(1); }

    for(i = 0; i < N_ROCKS; i++)
        if(pipe(output_rk_pipe[i])==-1) { perror("Errore nella creazione della pipe."); exit(1); }

    switch(main_pid=fork())
    {
    case -1: ErrorProcess();
    case 0:
        close(input_pipe[0]); //chiuso il descrittore in lettura
        close(output_ss_pipe[1]); //chiuso il descrittore in scrittura
        GameSpaceship(input_pipe[1], output_ss_pipe[0]);
        //richiamo della funzione di gioco della navicella pilotata da console. I parametri sono gli opportuni descrittori
    default:
        switch(main_pid=fork())
        {
        case -1: ErrorProcess();
        case 0:
            ProcessGenRocks();
            //richiamo della funzione di generazione delle roccie
        default:
            switch(main_pid=fork())
            {
            case -1: ErrorProcess();
            case 0:
                /** ATTENZIONE:
                La successiva riga di codice chiama la funzione che genera i processi che andranno a gestire le navicelle nemiche.
                Il parametro indica il livello delle navicelle create. Tuttavia se viene impostato a un numero > 1 per controllare rapidamente
                il funzionamento delle navicelle di secondo e terzo livello causa problemi.
                Il motivo come si può capire controllando la funzione ProcessGenSpacecraft è che per navicelle di livello > 1 viene creato
                un processo in più per motivi gestionali.
                Per controllare rapidamente il funzionamento delle navicelle di secondo e terzo livello bisogna modificare opportunamente
                anche ProcessGenSpacecraft. **/
                ProcessGenSpacecraft(1);
            default:
                //engine riceve le pipe con entrambi i descrittore perché dovrà gestire anche la nascita delle navicelle di secondo e terzo livello
                Engine(input_pipe, output_ss_pipe, output_sc_pipe, output_rk_pipe); //richiamo della funzione che gestirà il controllo dei processi e passaggio dei descrittori
            }
        }
    }

    return 0;
}
