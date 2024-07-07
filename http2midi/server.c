#include <stdio.h>  // console input/output, perror
#include <stdlib.h> // exit
#include <string.h> // string manipulation
#include <netdb.h>  // getnameinfo

#include <sys/socket.h> // socket APIs
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // open, close

#include <signal.h> // signal handling
#include <time.h>   // time

#define SIZE 1024  // buffer size
#define PORT 2728  // port number
#define BACKLOG 10 // number of pending connections queue will hold

#include <alsa/asoundlib.h>

int outport = 0;

snd_seq_t* sequencer = NULL;

void open_alsa_connection(char *clientname)
{
	int client;

	if (snd_seq_open(&sequencer, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		printf("Couldn't open ALSA sequencer");

	snd_seq_set_client_name(sequencer, clientname);
	client = snd_seq_client_id(sequencer);

	outport = snd_seq_create_simple_port(sequencer, "out",
		SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		printf("Couldn't create output port");

	printf("OUT = %d:%d\n", client, outport);
}

void handleSignal(int signal);


int serverSocket;
int clientSocket;

char *request;


static void MidiSendNoteOn(int channel, int key, int velocity) {
        snd_seq_event_t ev;

        snd_seq_ev_clear(&ev);
        snd_seq_ev_set_source(&ev, outport);
        snd_seq_ev_set_subs(&ev);
        snd_seq_ev_set_direct(&ev);
        snd_seq_ev_set_noteon( &ev, channel, key, velocity);

        snd_seq_event_output(sequencer, &ev);
        snd_seq_drain_output(sequencer);
}

static void MidiSendNoteOff(int channel, int key, int velocity) {
        snd_seq_event_t ev;

        snd_seq_ev_clear(&ev);
        snd_seq_ev_set_source(&ev, outport);
        snd_seq_ev_set_subs(&ev);
        snd_seq_ev_set_direct(&ev);
        snd_seq_ev_set_noteoff( &ev, channel, key, velocity);

        snd_seq_event_output(sequencer, &ev);
        snd_seq_drain_output(sequencer);
}

static void MidiSendController(int channel, int controller, int value) {
        snd_seq_event_t ev;

        snd_seq_ev_clear(&ev);
        snd_seq_ev_set_source(&ev, outport);
        snd_seq_ev_set_subs(&ev);
        snd_seq_ev_set_direct(&ev);
        snd_seq_ev_set_controller( &ev, channel, controller, value);

        snd_seq_event_output(sequencer, &ev);
        snd_seq_drain_output(sequencer);
}

static void OkResponse() {
      const char response[] = "HTTP/1.1 200 OK\r\n\n";
      send(clientSocket, response, sizeof(response), 0);
}

static void NoCommandResponse() {
      const char response[] = "HTTP/1.1 404 No such command\r\n\n";
      send(clientSocket, response, sizeof(response), 0);
}

int main(int argc, char* argv[])
{

  signal(SIGINT, handleSignal);

  char *name = "http2midi";

  if (argc > 1) {
    name = argv[1];
  }
  open_alsa_connection(name);


  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;                     // IPv4
  serverAddress.sin_port = htons(PORT);                   // port number in network byte order (host-to-network short)
  serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // localhost (host to network long)

  serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

  if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
  {
    printf("Error: The server is not bound to the address.\n");
    return 1;
  }

  if (listen(serverSocket, BACKLOG) < 0)
  {
    printf("Error: The server is not listening.\n");
    return 1;
  }

  char hostBuffer[NI_MAXHOST], serviceBuffer[NI_MAXSERV];
  int error = getnameinfo((struct sockaddr *)&serverAddress, sizeof(serverAddress), hostBuffer,
                          sizeof(hostBuffer), serviceBuffer, sizeof(serviceBuffer), 0);

  if (error != 0)
  {
    printf("Error: %s\n", gai_strerror(error));
    return 1;
  }

  printf("\nServer is listening on http://%s:%s/\n\n", hostBuffer, serviceBuffer);

  while (1)
  {
    request = (char *)malloc(SIZE * sizeof(char));
    char method[10], route[100];

    clientSocket = accept(serverSocket, NULL, NULL);
    read(clientSocket, request, SIZE);

    sscanf(request, "%s %s", method, route);

    free(request);

    if (strcmp(method, "GET") != 0)
    {
      const char response[] = "HTTP/1.1 400 Bad Request\r\n\n";
      send(clientSocket, response, sizeof(response), 0);
    }
    else
    {

      char *token;

      token = strtok(route, "/");
      if (token == NULL) {
         NoCommandResponse();
      } else if (strcmp(token,"noteon") == 0) {
           char *sChannel = NULL;
           char *sKey = NULL;
           char *sVelocity =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sChannel = token;  token = strtok(NULL, "/");}
           if (token != NULL) { sKey = token;  token = strtok(NULL, "/");}
           if (token != NULL) { sVelocity = token;  token = strtok(NULL, "/");}
	   MidiSendNoteOn(atoi(sChannel), atoi(sKey), atoi(sVelocity));
           OkResponse();
      } else if (strcmp(token,"noteoff") == 0) {
           char *sChannel = NULL;
           char *sKey = NULL;
           char *sVelocity =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sChannel = token;  token = strtok(NULL, "/");}
           if (token != NULL) { sKey = token;  token = strtok(NULL, "/");}
           if (token != NULL) { sVelocity = token;  token = strtok(NULL, "/");}
	   MidiSendNoteOff(atoi(sChannel), atoi(sKey), atoi(sVelocity));
           OkResponse();
      } else if (strcmp(token,"controller") == 0) {
           char *sChannel = NULL;
           char *sController = NULL;
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sChannel = token;  token = strtok(NULL, "/");}
           if (token != NULL) { sController = token;  token = strtok(NULL, "/");}
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
	   MidiSendController(atoi(sChannel), atoi(sController), atoi(sValue));
           OkResponse();
      } else {
         NoCommandResponse();
      } 

    }
    close(clientSocket);
    printf("\n");
  }
}


void handleSignal(int signal)
{
  if (signal == SIGINT)
  {
    printf("\nShutting down server...\n");

    close(clientSocket);
    close(serverSocket);

    if (request != NULL)
      free(request);

    exit(0);
  }
}

