# Chenny Wang
CC=gcc

client : 	cavallino_client.o
	$(CC) -o client.exe cavallino_client.o -lzmq
#
cavallino_client.o : cavallino_client.c	cavallino_client.h 
	$(CC) -c cavallino_client.c cavallino_client.h
	
#
.PHONY : 	clean
clean :
	rm -f *.o *.exe