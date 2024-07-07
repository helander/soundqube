#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <poll.h>
#include <alsa/asoundlib.h>

struct Connection {
  char *from;
  char *to;
  bool hasSubscription;
};

typedef struct Connection Connection;


#define UNASSIGNED 255

static snd_seq_t *seq;

static snd_seq_addr_t lastFrom = {.client = UNASSIGNED, .port = 0};
static snd_seq_addr_t lastTo = {.client = UNASSIGNED, .port = 0};

static void printConnection(Connection *connection, snd_seq_addr_t *fromaddr, snd_seq_addr_t *toaddr) {
  printf("\n  Connection %s -> %s ( %d:%d -> %d:%d ) subs %d",connection->from,connection->to,
            fromaddr->client,fromaddr->port,toaddr->client,toaddr->port,connection->hasSubscription);
}

bool  forceUnsubscribe = false;

static void stateMachine(Connection *connection) {
	snd_seq_addr_t fromAddr;
	snd_seq_addr_t toAddr;
        bool doSubscribe = false;
        bool doUnsubscribe = false;

        fromAddr.client = toAddr.client = UNASSIGNED;
	snd_seq_parse_address(seq, &fromAddr, connection->from);
	snd_seq_parse_address(seq, &toAddr, connection->to);
        printConnection(connection,&fromAddr,&toAddr);

        if (fromAddr.client != UNASSIGNED && toAddr.client != UNASSIGNED) {

	  snd_seq_port_subscribe_t *subs;

          printf("\n    Both ports exists");

   	  snd_seq_port_subscribe_alloca(&subs);
	  snd_seq_port_subscribe_set_sender(subs, &fromAddr);
	  snd_seq_port_subscribe_set_dest(subs, &toAddr);
	  if (snd_seq_get_port_subscription(seq, subs) == 0) {
		printf("\n    Connection is already subscribed");
                	if (connection->hasSubscription == false) {
                   		doUnsubscribe = true;
                   		doSubscribe = true;
		   		lastFrom = fromAddr;
		   		lastTo = toAddr;
                	}
	  } else {
		printf("\n    Connection is not subscribed");
                doSubscribe = true;
          }


        } else {
          printf("\n    Both ports does not exist");
          if (lastFrom.client != UNASSIGNED && lastTo.client != UNASSIGNED) {
  	    snd_seq_port_subscribe_t *subs;
            printf("\n    Old values exists for both ports");
	    snd_seq_port_subscribe_alloca(&subs);
	    snd_seq_port_subscribe_set_sender(subs, &lastFrom);
	    snd_seq_port_subscribe_set_dest(subs, &lastTo);
	    if (snd_seq_get_port_subscription(seq, subs) == 0) {
		printf("\n    Connection has old subscription");
                doUnsubscribe = true;
	    }
          } else {
            printf("\n    Old values does not exist for both ports");
          }
        }

                if (forceUnsubscribe == true) {
                   	doUnsubscribe = true;
                   	doSubscribe = false;
		} 


        if (doUnsubscribe) {
	  snd_seq_port_subscribe_t *subs;
          printf("\n    DO unsubscribe");fflush(stdout);

   	  snd_seq_port_subscribe_alloca(&subs);
	  snd_seq_port_subscribe_set_sender(subs, &lastFrom);
	  snd_seq_port_subscribe_set_dest(subs, &lastTo);
	  snd_seq_get_port_subscription(seq, subs);

	  if (snd_seq_unsubscribe_port(seq, subs) < 0) {
		printf("\n    Failed to unsubscribe (%s)", snd_strerror(errno));
	  }

          connection->hasSubscription = false;
        }
        if (doSubscribe) {
	  snd_seq_port_subscribe_t *subs;
          printf("\n    DO subscribe");fflush(stdout);
          lastFrom = fromAddr;
          lastTo = toAddr;

	  snd_seq_port_subscribe_alloca(&subs);
	  snd_seq_port_subscribe_set_sender(subs, &fromAddr);
	  snd_seq_port_subscribe_set_dest(subs, &toAddr);
	  snd_seq_port_subscribe_set_queue(subs, 0);
	  snd_seq_port_subscribe_set_exclusive(subs, 0);
	  snd_seq_port_subscribe_set_time_update(subs, 0);
	  snd_seq_port_subscribe_set_time_real(subs, 0);

	  if (snd_seq_subscribe_port(seq, subs) < 0) {
		printf("\n    Failed to subscribe (%s)", snd_strerror(errno));
	  }

          connection->hasSubscription = true;
        }
        printConnection(connection,&fromAddr,&toAddr);

}

static volatile sig_atomic_t stop = 0;
snd_seq_port_subscribe_t *subs;



static void fatal(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

static void check_snd(const char *operation, int err)
{
	if (err < 0)
		fatal("Cannot %s - %s", operation, snd_strerror(err));
}



static void dump_event(Connection *connections,int nmbConnections,const snd_seq_event_t *ev)
{
	//printf("%3d:%-3d ", ev->source.client, ev->source.port);

	switch (ev->type) {
	case SND_SEQ_EVENT_PORT_START:
		printf("\nPort start     %d:%d \n",ev->data.addr.client, ev->data.addr.port);
       for(int i = 0; i < nmbConnections; i++) {
         stateMachine(&connections[i]);
       }
		break;
	case SND_SEQ_EVENT_PORT_EXIT:
		printf("\nPort exit     %d:%d \n",ev->data.addr.client, ev->data.addr.port);
       for(int i = 0; i < nmbConnections; i++) {
         stateMachine(&connections[i]);
       }
		break;
	case SND_SEQ_EVENT_PORT_CHANGE:
		printf("\nPort change     %d:%d \n",ev->data.addr.client, ev->data.addr.port);
       for(int i = 0; i < nmbConnections; i++) {
         stateMachine(&connections[i]);
       }
		break;
	default:
		//printf("Event type %d\n",  ev->type);
	}
}


int nmbConnections;
Connection *connections;

static void sighandler(int sig ATTRIBUTE_UNUSED)
{
	printf("\nProgram terminates");
	forceUnsubscribe = true;
       for(int i = 0; i < nmbConnections; i++) {
         stateMachine(&connections[i]);
       }
	fflush(stdout);

	stop = 1;
}

int main(int argc, char *argv[])
{

	struct pollfd *pfds;
	int npfds;
	int err;

	printf("\nProgram start");

	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);


	err = snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
	check_snd("open sequencer", err);

	err = snd_seq_set_client_name(seq, "autoconnect");
	check_snd("set client name", err);

	err = snd_seq_create_simple_port(seq, "announcements",
					 SND_SEQ_PORT_CAP_WRITE |
					 SND_SEQ_PORT_CAP_SUBS_WRITE,
					 SND_SEQ_PORT_TYPE_MIDI_GENERIC |
					 SND_SEQ_PORT_TYPE_APPLICATION);
	check_snd("create port", err);

	err = snd_seq_connect_from(seq, 0, 0, 1); /* connect to system:announce port (0:1) */
	if (err < 0)
		fatal("Cannot connect from port %d:%d - %s", 0, 1, snd_strerror(err));

	err = snd_seq_nonblock(seq, 1);
	check_snd("set nonblock mode", err);

	nmbConnections = (argc - 1)/2;

        connections = (Connection *)malloc(nmbConnections * sizeof(Connection));

       for(int i = 0; i < nmbConnections; i++) {
         connections[i].from = argv[(2*i) + 1];
         connections[i].to = argv[(2*i) + 2];
         connections[i].hasSubscription = false;
         stateMachine(&connections[i]);
       }

	printf("\nSource  Event\n");
	
	npfds = snd_seq_poll_descriptors_count(seq, POLLIN);
	pfds = alloca(sizeof(*pfds) * npfds);
	for (;;) {
		snd_seq_poll_descriptors(seq, pfds, npfds, POLLIN);
		if (poll(pfds, npfds, -1) < 0)
			break;
		for (;;) {
			snd_seq_event_t *event;

			err = snd_seq_event_input(seq, &event);
			if (err < 0)
				break;
			if (event)
				dump_event(connections,nmbConnections,event);
		}
		fflush(stdout);
		if (stop)
			break;
	}

	snd_seq_close(seq);
	return 0;
}
