void ErrorProcess()
void ProcessGenRocks()
void ProcessGenSpacecraft(int level); //funzione che genera i processi che andranno a gestire le navicelle nemiche
int funct_shoot(int spacecraft_x, int spacecraft_y, int missile_x, int missile_y, int max_x, int max_y); //confronta due coppie di coordinate e restituisce 1 se coincidono
int funct_collision(int object1_x, int object1_y, int object2_x, int object2_y, int object1_max_x, int object1_max_y, int object2_max_x, int object2_max_y); //confronta due coppie di coordinate e restituisce 1 se coincidono
int all_spacecraft_death(int spacecraft_death[N_SPACECRAFT]);
int Engine(int input_pipe[2], int output_ss_pipe[2], int output_sc_pipe[N_SPACECRAFT][2], int output_rk_pipe[N_ROCKS][2]);
