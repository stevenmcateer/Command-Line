all: mc0 mc1 mc2

mc2: mc2.o
	gcc -g mc2.o -o mc2

mc2.o: mc2.c
	gcc -g -c mc2.c

mc1: mc1.o
	gcc -g mc1.o -o mc1

mc1.o: mc1.c
	gcc -g -c mc1.c
	
mc0: mc0.o
	gcc -g mc0.o -o mc0

mc0.o: mc0.c
	gcc -g -c mc0.c

clean:
	rm -f *.o
	rm -f mc0 mc1 mc2

