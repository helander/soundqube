// TODO lägg till en startparameter som sätter antalet kanaler som noter genereras för (fluid verkar "tåla" 12 i ensamt majestät)

#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <alsa/asoundlib.h>

int inport = 0;
int outport = 0;

snd_seq_t* sequencer = NULL;

void open_alsa_connection(char *clientname)
{
	int client;

	if (snd_seq_open(&sequencer, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		printf("Couldn't open ALSA sequencer");

	snd_seq_set_client_name(sequencer, clientname);
	client = snd_seq_client_id(sequencer);

	inport = snd_seq_create_simple_port(sequencer, "in",
		SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		printf("Couldn't create input port");

	outport = snd_seq_create_simple_port(sequencer, "out",
		SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		printf("Couldn't create output port");

	printf("IN = %d:%d, OUT = %d:%d\n",
		client, inport,
		client, outport);
}


void process_events(void)
{
	int numpolldesc = snd_seq_poll_descriptors_count(sequencer, POLLIN);
	struct pollfd* pfd;
	snd_seq_event_t* event;

	pfd = (struct pollfd*) malloc(sizeof(struct pollfd) * numpolldesc);
	if (!pfd)
		printf("Memory allocation failed");

	snd_seq_poll_descriptors(sequencer, pfd, numpolldesc, POLLIN);

	for (;;)
	{
		if (snd_seq_event_input(sequencer, &event) < 0)
		{
			poll(pfd, 1, 100000);
			continue;
		}

		snd_seq_ev_set_subs(event);  
		snd_seq_ev_set_direct(event);

		switch (event->type)
		{

			case SND_SEQ_EVENT_NOTEON:
			case SND_SEQ_EVENT_NOTEOFF:
				printf("\nEvent type %d note %d",event->type,event->data.note.note);
				snd_seq_ev_set_source(event, outport);
				event->data.note.channel = 0;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 1;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 2;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 3;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 4;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 5;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 6;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 7;
				snd_seq_event_output_direct(sequencer, event);
				/*
				event->data.note.channel = 8;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 9;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 10;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 11;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 12;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 13;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 14;
				snd_seq_event_output_direct(sequencer, event);
				event->data.note.channel = 15;
				snd_seq_event_output_direct(sequencer, event);
				*/
                        default:
                                break;
		}

		snd_seq_free_event(event);
	}
}

int main(int argc, char* argv[])
{
	open_alsa_connection(argv[1]);
	process_events();

	return 0;
}

