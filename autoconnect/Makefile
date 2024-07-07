PROG = autoconnect
CFLAGS = -g -O2 -Wall 
CC = gcc
LIBS = -lasound



OBJS = \
	autoconnect.o

$(PROG): $(OBJS)  
	$(CC) -o $(PROG) $(OBJS) $(LIBS) 

autoconnect.o: autoconnect.c 

clean:
	$(RM) $(PROG) $(OBJS)
