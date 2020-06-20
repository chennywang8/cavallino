# Chenny Wang
CC=gcc

all : client
# link
client : 	cavallino_client.o
	$(CC) -o client.exe *.o -lzmq
# compile
cavallino_client.o : ../src/cavallino_client.c	../src/cavallino_client.h 
	$(CC) -c ../src/cavallino_client.c
	
# clean
.PHONY : 	clean
clean :
	rm -v -f *.o *.exe