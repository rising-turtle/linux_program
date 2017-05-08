#
# makefile for play_main - sort of like pong, but with no walls or paddle
#

bounce2d: play_main.o set_ticker.o paddle.o pp_table.o
	cc -o play_pong play_main.o set_ticker.o pp_table.o paddle.o -lcurses

clean:
	rm -f *.o 
