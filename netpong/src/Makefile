#
# makefile for play_main - sort of like pong, but with no walls or paddle
#

net_main: net_main.o pp_request.o set_ticker.o paddle.o pp_table.o conn_reply.o bounce.o
	cc -o net_main net_main.o pp_request.o set_ticker.o paddle.o pp_table.o conn_reply.o bounce.o -lcurses

# bounce2d: play_main.o set_ticker.o paddle.o pp_table.o
#	cc -o play_pong play_main.o set_ticker.o pp_table.o paddle.o -lcurses

clean:
	rm -f *.o 
	rm net_main
