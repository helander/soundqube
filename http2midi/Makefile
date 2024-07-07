PROG = http2midi
CFLAGS = -g -O2 -Wall 
CC = gcc
LIBS = -lasound



OBJS = \
        server.o

$(PROG): $(OBJS)  
	$(CC) -o $(PROG) $(OBJS) $(LIBS) 

server.o: server.c 

clean:
	$(RM) $(PROG) $(OBJS)
