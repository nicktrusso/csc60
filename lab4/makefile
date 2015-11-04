#makefile for csc60mshell.c
#Programmed by: Nick Trusso
CC=gcc
CFLAGS=-g -Wall
OBJ=csc60mshell.o
EXE=shell

all: $(EXE)
$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE)
csc60mshell.o: csc60mshell.c
	$(CC) $(CFLAGS) -c csc60mshell.c
clean:
	rm $(OBJ)
