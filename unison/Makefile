PROG = unison
CFLAGS = -g -O2 -Wall 
CC = gcc
LIBS = -lasound



OBJS = \
	unison.o

$(PROG): $(OBJS)  
	$(CC) -o $(PROG) $(OBJS) $(LIBS) 

unison.o: unison.c 

clean:
	$(RM) $(PROG) $(OBJS)
