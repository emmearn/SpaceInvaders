#include <ncurses.h>
#include <string.h>

#include "global_vars.h"

#define C_SFONDO 1
#define C_SPACESHIP 2
#define C_BORDER 3
#define C_LABEL 4
#define C_NUMBER 5
#define C_ROCKS 6
#define C_SC_GOOD 7
#define C_SC_MEDIUM 8
#define C_SC_LOW 9
#define C_ROCKS_GOOD 10
#define C_ROCKS_MEDIUM 11
#define C_ROCKS_LOW 12
#define C_ROCKS_LOW 12
#define C_SPACESHIP2 13
#define C_FIRE1 14
#define C_FIRE2 15

#define N_COLORS 15

int fire1 = C_FIRE1, fire2 = C_FIRE2, temp_fire;

typedef struct Rettangle
{
    int TopLeftX;
    int TopLeftY;
    int BottomRightX;
    int BottomRightY;
} Rettangle;

Rettangle scoreTable;

void InitColors(WINDOW *win)
{
     init_pair(C_SFONDO , COLOR_BLACK, COLOR_BLACK);
     init_pair(C_SPACESHIP, COLOR_BLUE, COLOR_BLACK);
     init_pair(C_BORDER, COLOR_WHITE, COLOR_BLACK);
     init_pair(C_LABEL, COLOR_YELLOW, COLOR_BLACK);
     init_pair(C_NUMBER, COLOR_WHITE, COLOR_BLACK);
     init_pair(C_ROCKS, COLOR_MAGENTA, COLOR_BLACK);
     init_pair(C_SC_GOOD, COLOR_GREEN, COLOR_BLACK);
     init_pair(C_SC_MEDIUM, COLOR_YELLOW, COLOR_BLACK);
     init_pair(C_SC_LOW, COLOR_RED, COLOR_BLACK);
     init_pair(C_ROCKS_GOOD, COLOR_GREEN, COLOR_GREEN);
     init_pair(C_ROCKS_MEDIUM, COLOR_YELLOW, COLOR_YELLOW);
     init_pair(C_ROCKS_LOW, COLOR_RED, COLOR_RED);
     init_pair(C_SPACESHIP2, COLOR_BLUE, COLOR_BLUE);
     init_pair(C_FIRE1, COLOR_RED, COLOR_BLACK);
     init_pair(C_FIRE2, COLOR_YELLOW, COLOR_BLACK);

     bkgd(COLOR_PAIR(1));
}

void InitRettangle(Rettangle *r, int x1, int y1, int x2, int y2)
{
    r->TopLeftX = x1;
    r->TopLeftY = y1;
    r->BottomRightX = x2;
    r->BottomRightY = y2;
}

void DrawOnlyAngles (Rettangle table)
{
    attrset(COLOR_PAIR(C_BORDER));

    mvaddch(table.TopLeftY, table.TopLeftX, ACS_ULCORNER); //angolo in alto a sx
    //usleep(555555); refresh();
    mvaddch(table.TopLeftY, table.BottomRightX, ACS_URCORNER); //angolo in alto a dx
    //usleep(555555); refresh();
    mvaddch(table.BottomRightY, table.TopLeftX, ACS_LLCORNER); //angolo in basso a sx
    //usleep(555555); refresh();
    mvaddch(table.BottomRightY, table.BottomRightX, ACS_LRCORNER); //angolo in basso a dx
    //usleep(555555); refresh();

    attroff(COLOR_PAIR(C_BORDER));
}

void DrawRettangle (Rettangle table)
{
    DrawOnlyAngles (table);

    attrset(COLOR_PAIR(C_BORDER));

    mvhline(table.TopLeftY, table.TopLeftX+1,ACS_HLINE, table.BottomRightX-1); //su
    //usleep(555555); refresh();
    mvvline(table.TopLeftY+1, table.TopLeftX,ACS_VLINE, table.BottomRightY-1); //sx
    //usleep(555555); refresh();
    mvhline(table.BottomRightY, table.TopLeftX+1,ACS_HLINE, table.BottomRightX-1); //giu'
    //usleep(555555); refresh();
    mvvline(table.TopLeftY+1, table.BottomRightX,ACS_VLINE, table.BottomRightY-1); //dx
    //usleep(555555); refresh();

    attroff(COLOR_PAIR(C_BORDER));

}

int ChooseColor()
{


    int r;
    do
        r = random()%N_COLORS+1;
    while(r>=10 && r<=13 || r == 1);

    return r;


}

void WriteScore(int n)
{
    attrset(COLOR_PAIR(C_BORDER));
    snprintf(score_string,sizeof(score_string),"%d",n) ;
    attrset(COLOR_PAIR(C_LABEL));
    mvaddnstr(MAXY+2, 1, "SCORE:", 6);
    attroff(COLOR_PAIR(C_LABEL));

    attrset(COLOR_PAIR(C_NUMBER));
    mvaddnstr(MAXY+2, 8, score_string, 10);
    attroff(COLOR_PAIR(C_NUMBER));
    //usleep(555555);
    attroff(COLOR_PAIR(C_BORDER));
}

void WriteLife(int life)
{
    snprintf(life_string,sizeof(life_string),"%d", life) ;
    attrset(COLOR_PAIR(C_LABEL));
    mvaddnstr(MAXY+2, MAXX-8, "LIFE:", 5);
    attroff(COLOR_PAIR(C_LABEL));

     attrset(COLOR_PAIR(C_NUMBER));
    mvaddnstr(MAXY+2, MAXX-2, life_string, 1);
    attroff(COLOR_PAIR(C_NUMBER));
}

void WriteLegend()
{
    attrset(COLOR_PAIR(C_ROCKS_GOOD));
    attron(A_BOLD);
    mvaddnstr(MAXY + 2, 20, "  ", 2);
    attroff(A_BOLD);
    attroff(COLOR_PAIR(C_ROCKS_GOOD));
    attrset(COLOR_PAIR(C_NUMBER));
    mvaddnstr(MAXY+2, 23, "GOOD", 4);
    attroff(COLOR_PAIR(C_NUMBER));

    attrset(COLOR_PAIR(C_ROCKS_MEDIUM));
    mvaddnstr(MAXY+2, 30, "  ", 2);
    attroff(COLOR_PAIR(C_ROCKS_MEDIUM));
    attrset(COLOR_PAIR(C_NUMBER));
    mvaddnstr(MAXY+2, 33, "MEDIUM", 6);
    attroff(COLOR_PAIR(C_NUMBER));


    attrset(COLOR_PAIR(C_ROCKS_LOW));
    mvaddnstr(MAXY+2, 41, "  ", 2);
    attroff(COLOR_PAIR(C_ROCKS_LOW));
    attrset(COLOR_PAIR(C_NUMBER));
    mvaddnstr(MAXY+2, 44, "LOW", 3);
    attroff(COLOR_PAIR(C_NUMBER));
    //attroff(COLOR_PAIR(C_LABEL));

    /* attrset(COLOR_PAIR(C_NUMBER));
    mvaddnstr(MAXY+2, MAXX-2, life_string, 3);
    attroff(COLOR_PAIR(C_NUMBER));*/
}

void WriteLevel(int level)
{
    snprintf(level_string,sizeof(level_string),"%d", level) ;
    attrset(COLOR_PAIR(C_LABEL));
    mvaddnstr(MAXY+2, 65, "LEVEL:", 7);
    attroff(COLOR_PAIR(C_LABEL));

     attrset(COLOR_PAIR(C_NUMBER));
    mvaddnstr(MAXY+2, 72, level_string, 1);
    attroff(COLOR_PAIR(C_NUMBER));
}

void DrawSpaceship(int y, int x)
{
    attrset(COLOR_PAIR(C_SPACESHIP));
    //riga 1
    //mvaddch(y, x, ' ');
    mvaddch(y, x+1, ' ');
    mvaddch(y, x+2, '/'|WA_BOLD);
    mvaddch(y, x+3, '\\'|WA_BOLD);
    mvaddch(y, x+4, ' ');
    mvaddch(y, x+5, ' ');

    //riga 2
    //mvaddch(y+1, x, ' ');
    mvaddch(y+1, x+1, '0'|WA_BOLD);
    mvaddch(y+1, x+2, '_'|WA_BOLD);
    mvaddch(y+1, x+3, '_'|WA_BOLD);
    mvaddch(y+1, x+4, '0'|WA_BOLD);
    //mvaddch(y+1, x+5, ' ');

    //riga 3
    mvaddch(y+2, x, '/'|WA_BOLD);
    mvaddch(y+2, x+1, '.'|WA_BOLD);
    mvaddch(y+2, x+2, '^'|WA_BOLD);
    mvaddch(y+2, x+3, '^'|WA_BOLD);
    mvaddch(y+2, x+4, '.'|WA_BOLD);
    mvaddch(y+2, x+5, '\\'|WA_BOLD);

    //riga 4
    mvaddch(y+3, x, '['|WA_BOLD);
    mvaddch(y+3, x+1, '*'|WA_BOLD);
    mvaddch(y+3, x+2, 'M');
    mvaddch(y+3, x+3, 'E');
    mvaddch(y+3, x+4, '*'|WA_BOLD);
    mvaddch(y+3, x+5, ']'|WA_BOLD);

    //riga 5
    //mvaddch(y+5, x, '\\');
    mvaddch(y+4, x+1, '\\'|WA_BOLD);
    mvaddch(y+4, x+2, '_'|WA_BOLD);
    mvaddch(y+4, x+3, '_'|WA_BOLD);
    mvaddch(y+4, x+4, '/'|WA_BOLD);
    //mvaddch(y+5, x+5, '/');
    attroff(COLOR_PAIR(C_SPACESHIP));

    //riga 6
    //mvaddch(y+5, x, '\\');
    attrset(COLOR_PAIR(fire1));
    mvaddch(y+5, x+1, '\'');
    attroff(COLOR_PAIR(fire1));

    attrset(COLOR_PAIR(fire2));
    mvaddch(y+5, x+2, '\'');
    attroff(COLOR_PAIR(fire2));

    attrset(COLOR_PAIR(fire1));
    mvaddch(y+5, x+3, '\'');
    attroff(COLOR_PAIR(fire1));

    attrset(COLOR_PAIR(fire2));
    mvaddch(y+5, x+4, '\'');
    attroff(COLOR_PAIR(fire2));
    //mvaddch(y+5, x+5, '/');

    temp_fire = fire1;
    fire1 = fire2;
    fire2 = temp_fire;

}

void DeleteSpaceship(int y, int x)
{
    int i, j;

    for(i = 0; i < 6; i++)
    {
        for(j = 0; j < 6; j++)
        {
            mvaddch(y +i, x +j, ' ');
        }
    }
}

void DrawSpacecraft(int y, int x, char name[], int level, int life)
{
    int temp_color;
    switch(level)
    {
    case 1:

        switch(life)
        {
            case 2:
                temp_color = C_SC_GOOD;
                attrset(COLOR_PAIR(C_SC_GOOD));
                break;
            case 1:
                temp_color = C_SC_LOW;
                attrset(COLOR_PAIR(C_SC_LOW));
                break;
        }
        //riga 1
        mvaddch(y, x, '\\');
        mvaddch(y, x+1, '_');
        mvaddch(y, x+2, '/');

        //riga 2
        mvaddch(y+1, x, name[0]);
        mvaddch(y+1, x+1, name[1]);
        mvaddch(y+1, x+2, name[2]);

        //riga 3
        mvaddch(y+2, x+1, '\'');

        attroff(COLOR_PAIR(temp_color));

        break;

    case 2:
        switch(life)
        {
            case 3:
                temp_color = C_SC_GOOD;
                attrset(COLOR_PAIR(C_SC_GOOD));
                break;
            case 2:
                temp_color = C_SC_MEDIUM;
                attrset(COLOR_PAIR(C_SC_MEDIUM));
                break;
            case 1:
                temp_color = C_SC_LOW;
                attrset(COLOR_PAIR(C_SC_LOW));
                break;
        }
        //riga 1
        mvaddch(y, x, '\\');
        mvaddch(y, x+1, '_');
        mvaddch(y, x+2, '_');
        mvaddch(y, x+3, '/');

        //riga 2
        mvaddch(y+1, x, '.');
        mvaddch(y+1, x+1, name[0]);
        mvaddch(y+1, x+2, '/');
        mvaddch(y+1, x+3, '.');

        //riga 3
        mvaddch(y+2, x, '_');
        mvaddch(y+2, x+1, name[1]);
        mvaddch(y+2, x+2, '_');
        mvaddch(y+2, x+3, '_');

        //riga 4
        mvaddch(y+3, x, '\\');
        mvaddch(y+3, x+1, name[2]);
        mvaddch(y+3, x+2, '_');
        mvaddch(y+3, x+3, '/');

        attroff(COLOR_PAIR(temp_color));
        break;
    case 3:
     switch(life)
        {
            case 4:
                temp_color = C_SC_GOOD;
                attrset(COLOR_PAIR(C_SC_GOOD));
                break;
            case 3:
            case 2:
                temp_color = C_SC_MEDIUM;
                attrset(COLOR_PAIR(C_SC_MEDIUM));
                break;
            case 1:
                temp_color = C_SC_LOW;
                attrset(COLOR_PAIR(C_SC_LOW));
                break;
        }
        //riga 1
        mvaddch(y, x, '"');
        mvaddch(y, x+1, '"');
        mvaddch(y, x+2, ' ');
        mvaddch(y, x+3, '"');
        mvaddch(y, x+4, '"');

        //riga 2
        mvaddch(y+1, x, '\\');
        mvaddch(y+1, x+1, '\\');
        mvaddch(y+1, x+2, '|');
        mvaddch(y+1, x+3, '/');
        mvaddch(y+1, x+4, '/');

        //riga 3
        mvaddch(y+2, x, '.');
        mvaddch(y+2, x+1, name[0]);
        mvaddch(y+2, x+2, name[1]);
        mvaddch(y+2, x+3, name[2]);
        mvaddch(y+2, x+4, '.');

        //riga 4
        mvaddch(y+3, x, '_');
        mvaddch(y+3, x+1, '/');
        mvaddch(y+3, x+2, '+');
        mvaddch(y+3, x+3, '\\');
        mvaddch(y+3, x+4, '_');

        //riga 5
        mvaddch(y+4, x, '\\');
        mvaddch(y+4, x+1, '\\');
        mvaddch(y+4, x+2, '+');
        mvaddch(y+4, x+3, '/');
        mvaddch(y+4, x+4, '/');

        attroff(COLOR_PAIR(temp_color));
        break;
    }
}

void DeleteSpacecraft(int y, int x, int size)
{
    int i, j;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            mvaddch(y +i, x +j, ' ');
}

void DrawRocks(int y, int x, int life)
{

    attrset(COLOR_PAIR(C_ROCKS));
    //riga 2
    mvaddch(y, x, '/');
    mvaddch(y, x+1, '(');
    mvaddch(y, x+2, '^'|WA_BOLD);
    mvaddch(y, x+3, '^'|WA_BOLD);
    mvaddch(y, x+4, ')');
    mvaddch(y, x+5, '\\');


    //riga
    mvaddch(y+1, x, '|');
    attroff(COLOR_PAIR(C_ROCKS));
    if (life == 1)
    {
        attrset(COLOR_PAIR(C_ROCKS_LOW));
        mvaddch(y+1, x+1, ' ');
        attroff(COLOR_PAIR(C_ROCKS_LOW));
    }
    else if (life == 2)
    {
        attrset(COLOR_PAIR(C_ROCKS_MEDIUM));
        mvaddch(y+1, x+1, ' ');
        mvaddch(y+1, x+2, ' ');
        attroff(COLOR_PAIR(C_ROCKS_MEDIUM));
    }
    if (life == 3)
    {
        attrset(COLOR_PAIR(C_ROCKS_MEDIUM));
        mvaddch(y+1, x+1, ' ');
        mvaddch(y+1, x+2, ' ');
        mvaddch(y+1, x+3, ' ');
        attroff(COLOR_PAIR(C_ROCKS_MEDIUM));
    }
    if (life == 4)
    {
        attrset(COLOR_PAIR(C_ROCKS_GOOD));
        mvaddch(y+1, x+1, ' ');
        mvaddch(y+1, x+2, ' ');
        mvaddch(y+1, x+3, ' ');
        mvaddch(y+1, x+4, ' ');
        attroff(COLOR_PAIR(C_ROCKS_GOOD));

    }
    attrset(COLOR_PAIR(C_ROCKS));
    mvaddch(y+1, x+5, '|');

    //riga 3
    mvaddch(y+2, x, '\\');
    mvaddch(y+2, x+1, '/');
    mvaddch(y+2, x+2, '"');
    mvaddch(y+2, x+3, '"');
    mvaddch(y+2, x+4, '\\');
    mvaddch(y+2, x+5, '/');
    attroff(COLOR_PAIR(C_ROCKS));
}

void DeleteRocks(int y, int x)
{
    int i, j;

    for(i = 0; i < 3; i++)
        for(j = 0; j < 6; j++)
            mvaddch(y +i, x +j, ' ');
}

void DrawMissile(int y, int x)
{
    int i = random()%9+1;
    attrset(COLOR_PAIR(i));
    mvaddch(y, x, '*');
    attroff(COLOR_PAIR(i));
}

void DeleteMissile(int y, int x)
{
    mvaddch(y, x, ' ');
}

void DrawBannerSPACE(int y, int x)
{
        mvaddnstr(y, x,   "     _______..______      ___       ______  _______", 52);
        mvaddnstr(y+1, x, "    /       ||   _  \\    /   \\     /      ||   ____|", 53);
        mvaddnstr(y+2, x, "   |   (----`|  |_)  |  /  ^  \\   |  ,----'|  |__", 50);
        mvaddnstr(y+3, x, "    \\   \\    |   ___/  /  /_\\  \\  |  |     |   __|", 51);
        mvaddnstr(y+4, x, ".----)   |   |  |     /  _____  \\ |  `----.|  |____", 52);
        mvaddnstr(y+5, x, "|_______/    | _|    /__/     \\__\\ \\______||_______|", 53);
}

void DeleteBannerSPACE(int y, int x)
{
        mvaddnstr(y, x,   "                                                    ", 52);
        mvaddnstr(y+1, x, "                                                     ", 53);
        mvaddnstr(y+2, x, "                                                  ", 50);
        mvaddnstr(y+3, x, "                                                   ", 51);
        mvaddnstr(y+4, x, "                                                    ", 52);
        mvaddnstr(y+5, x, "                                                     ", 53);
}


void DrawBannerINVADERS(int y, int x)
{
        mvaddnstr(y , x, " __  .__   __. ____    ____  ___       _______   _______ .______          _______.", 83);
        mvaddnstr(y + 1, x, "|  | |  \\ |  | \\   \\  /   / /   \\     |       \\ |   ____||   _  \\        /       |", 83);
        mvaddnstr(y + 2, x, "|  | |   \\|  |  \\   \\/   / /  ^  \\    |  .--.  ||  |__   |  |_)  |      |   (----`", 83);
        mvaddnstr(y + 3, x, "|  | |  . `  |   \\      / /  /_\\  \\   |  |  |  ||   __|  |      /        \\   \\", 79);
        mvaddnstr(y + 4, x, "|  | |  |\\   |    \\    / /  _____  \\  |  '--'  ||  |____ |  |\\  \\----.----)   |", 80);
        mvaddnstr(y + 5, x, "|__| |__| \\__|     \\__/ /__/     \\__\\ |_______/ |_______|| _| `._____|_______/", 79);
}

void DeleteBannerINVADERS(int y, int x)
{
        mvaddnstr(y , x, "                                                                                   ", 83);
        mvaddnstr(y + 1, x, "                                                                                   ", 83);
        mvaddnstr(y + 2, x, "                                                                                   ", 83);
        mvaddnstr(y + 3, x, "                                                                               ", 79);
        mvaddnstr(y + 4, x, "                                                                                ", 80);
        mvaddnstr(y + 5, x, "                                                                               ", 79);
}


void DrawGameTitle()
{
    int i, j;
    int r1;

    for (i = 1; i < MAXX/2 - 52/2;i++)
    {
        r1 = ChooseColor();

        attrset(COLOR_PAIR(r1));
        DrawBannerSPACE((MAXY/2) - 6, i);

        refresh();
        usleep(60000);

        if (!(i == MAXX/2 -52/2 - 1)) DeleteBannerSPACE((MAXY/2) - 6, i);

        refresh();
        attroff(COLOR_PAIR(r1));
    }

    for (j = MAXX-83; j > MAXX/2 - 84/2 ; j--)
    {
        r1 = ChooseColor();
        attrset(COLOR_PAIR(r1));

        DrawBannerINVADERS(MAXY/2, j);

        refresh();
        usleep(60000);

        if (j != MAXX/2 - 84/2 - 1) DeleteBannerINVADERS(MAXY/2, j);

        refresh();
        attroff(COLOR_PAIR(r1));
    }


    for (i = 0; i < 5; i ++)
    {

        r1 = ChooseColor();

        attrset(COLOR_PAIR(r1));
        attron(A_BOLD);

        DrawBannerSPACE(MAXY/2 - 6, MAXX/2 - 52/2 - 1);
        DrawBannerINVADERS(MAXY/2, MAXX/2 - 84/2);

        attroff(A_BOLD);
        attroff(COLOR_PAIR(r1));

        attrset(COLOR_PAIR(C_LABEL));
        mvaddnstr(30, MAXX/2 - 32/2, "DEVELOPED BY M.ARNONE & M.CADEDDU", 33);
        attroff(COLOR_PAIR(C_LABEL));

        refresh();
        usleep(500000);
    }

}


void ClearGameTable()
{
    int i,j;
    for (i = 1; i < MAXX-1; i++)
        for (j = 1; j < MAXY-1; j++)
            mvaddch(j,i,' ');
}

void YouWin()
{
    char string_win[] = "YOU WIN";
    int i, j, r1, n, half_maxx = MAXX/2;

    n = 100;

    ClearGameTable();

    while (n > 0)
    {
        for (j = 1; j < MAXX - strlen(string_win) ; j = j + strlen(string_win) +1)
            for (i = 1; i < MAXY; i++)
            {
                r1 = ChooseColor();
                attrset(COLOR_PAIR(r1));
                attron(A_BOLD);
                mvaddnstr(i, j, string_win, strlen(string_win));
                attroff(A_BOLD);
                attroff(COLOR_PAIR(r1));
                refresh();
                //usleep(9000);
            }
            n--;
    }
    refresh();
    clear();
    endwin();
}

void YouLose()
{
    int r1;
    char string_lose[] = "YOU LOSE";

    ClearGameTable();

    usleep(1000);

    r1 = ChooseColor();
    attrset(COLOR_PAIR(r1));
    mvaddnstr(MAXY/2, MAXX/2 - strlen(string_lose), string_lose, strlen(string_lose));
    attroff(COLOR_PAIR(r1));
    refresh();
    sleep(3);
    clear();
    endwin();
}

void InitBoard (WINDOW* gameWindow)
{
    gameWindow = initscr(); /* inizializzazione dello schermo */
    curs_set(0); /* nasconde il cursore */
    noecho(); /* i caratteri corrispondenti ai tasti premuti non saranno visualizzati sullo schermo del terminale*/

    start_color();
    InitColors(gameWindow);

    Rettangle gameTable;
    Rettangle legend;

    InitRettangle(&gameTable, 0, 0, MAXX, MAXY);
    InitRettangle(&scoreTable, 0, MAXY+1, MAXX, SCORE_DIM + MAXY);
    InitRettangle(&legend, MAXX+1, 0, MAXX+30, 4);

    DrawRettangle(gameTable); //disegnamo la tabella di gioco
    DrawGameTitle();

    clear();
    refresh();

    DrawRettangle(gameTable);
    DrawOnlyAngles(scoreTable);

    WriteLegend();
    WriteLevel(1);
    WriteScore(0);

    refresh();
}
