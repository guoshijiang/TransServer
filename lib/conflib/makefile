.PHONY:clean

WORKDIR=.

VPATH = ./src

LIBOBJS=  WriteLog.o Cfifo.o Config.o
LIB1 = Config.so

CC = gcc
CFLGS= -Wall -g -I$(WORKDIR)/inc/
LIBFLAG = -L$(HOME)/lib

all: $(LIB1) 


$(LIB1):$(LIBOBJS)
	$(CC) -shared -fPIC $^ -o $@ 
	cp  $(LIB1) $(HOME)/lib
	mv  $(LIB1) ./lib

.c.o:
	$(CC) $(CFLGS) -c $< -o $@  -shared -fPIC
	
clean:
	rm -rf $(BIN) *.o $(HOME)/lib/$(LIB1) ./$(LIB1) 
		




