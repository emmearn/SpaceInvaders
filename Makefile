space_invaders:	main.o texture.o engine.o spaceship.o spacecraft.o missile.o rocks.o
	cc texture.o main.o engine.o spaceship.o spacecraft.o missile.o rocks.o -o space_invaders -lncurses

main.o: main.c global_vars.h
	cc -c main.c

texture.o: texture.c texture.h global_vars.h
	cc -c texture.c

engine.o: engine.c engine.h global_vars.h
	cc -c engine.c

spaceship.o: spaceship.c spaceship.h global_vars.h
	cc -c spaceship.c

spacecraft.o: spacecraft.c spacecraft.h global_vars.h
	cc -c spacecraft.c

missile.o: missile.c missile.h global_vars.h
	cc -c missile.c

rocks.o: rocks.c rocks.h global_vars.h
	cc -c rocks.c

clean:
	rm -f *.o space_invaders
