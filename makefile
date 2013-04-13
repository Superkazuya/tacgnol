CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` -lm
SRC = $(wildcard *dct.c)

all : dct coding

dct : $(SRC) 
	gcc -o $@ $^ $(CFLAGS) $(LIBS) 

coding : $(wildcard block*.c) coding.c iEBMA.c
	gcc -o $@ $^ $(CFLAGS) $(LIBS) 
