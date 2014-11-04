 CFLAGS = -g -Wall -ansi -pedantic -lpthread 

CC = gcc

.SUFFIXES: .cpp .o .C .c


SOURCE = producerConsumer.c
PROJECT = prodcon
prodcon:	$(SOURCE)
	$(CC) -o $(PROJECT) $(CFLAGS) $(SOURCE)



clean:
	/bin/rm -fr $(OBJ) $(PROJECT) *.o *~ core*; clear

