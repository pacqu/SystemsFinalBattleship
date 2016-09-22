all: player1.c player2.c
	gcc player1.c -o player1
	gcc player2.c -o player2

clean:
	rm *~
	rm *.o
