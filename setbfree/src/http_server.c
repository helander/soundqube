#include <stdio.h>  // console input/output, perror
#include <stdlib.h> // exit
#include <string.h> // string manipulation
#include <netdb.h>  // getnameinfo

#include <sys/socket.h> // socket APIs
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // open, close

#include <signal.h> // signal handling
#include <time.h>   // time

#include <math.h>   // math



#include "global_inst.h"
#include "program.h"
//#include "tonegen.h"
//#include "overdrive.h"
//#include "reverb.h"
//#include "whirl.h"

//#define UPPER_BUS_LO 0
//#define LOWER_BUS_LO 9

//#define RT_UPPRVIB 0x02
//#define RT_LOWRVIB 0x01

#define SIZE 1024  // buffer size

#define PORT 2729  // port number
#define BACKLOG 10 // number of pending connections queue will hold


extern void setDrawBar (struct b_tonegen* t, int bus, unsigned int setting);

void handleSignal(int signal);


int serverSocket;
int clientSocket;

char *request;

static void OkResponse() {
      const char response[] = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
      send(clientSocket, response, strlen(response), 0);
}


void nisse(int cnt, int pgm, char *name, void *arg) {
   char kalle[50];
   kalle[0] = 0;
   sprintf(kalle,"%d|%s\n",pgm,name);
   strcat(arg,kalle);
}



static void
pelle (int fnid, const char* key, const char* kv, unsigned char val, void* arg)
{
   char kalle[50];
   kalle[0] = 0;
   sprintf(kalle,"%d|%s=%d\n",fnid,key,val);
   strcat(arg,kalle);
}

typedef struct {
  char* key;
  void* arg;
} foo;

static void
rolle (int fnid, const char* key, const char* kv, unsigned char val, void* arg)
{
   foo* x = (foo*) arg;
   char kalle[50];
   kalle[0] = 0;
   if (strcmp(key,x->key) == 0) {
      sprintf(kalle,"%d|%s=%d\n",fnid,key,val);
      strcat(x->arg,kalle);
   }
}





static void ProgramsResponse(b_instance *inst) {
  char buffer[5000];
  buffer[0] = 0;
  strcat(buffer,"HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
  loopProgammes(inst->progs,0,nisse,buffer);
  send(clientSocket, buffer, strlen(buffer), 0);
}




static void ProgramResponse(b_instance *inst) {
      char response[100];
      if (installedProgram == -1 ) installProgram(inst,0);
      sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d|%s",installedProgram+1,installedProgramName(inst));
      send(clientSocket, response, strlen(response), 0);
printf("\ncurrent program  %d",installedProgram+1);
}


static void NoCommandResponse() {
      const char response[] = "HTTP/1.1 404 No such command\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
      send(clientSocket, response, strlen(response), 0);
}


int http_server_run(b_instance* inst)
{

  signal(SIGINT, handleSignal);

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;                     // IPv4
  serverAddress.sin_port = htons(PORT);                   // port number in network byte order (host-to-network short)
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // localhost (host to network long)

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
  fflush(stdout);

  while (1)
  {
    request = (char *)malloc(SIZE * sizeof(char));
    char method[10], route[100];

    clientSocket = accept(serverSocket, NULL, NULL);
    read(clientSocket, request, SIZE);

    sscanf(request, "%s %s", method, route);

    free(request);
    printf("\nMethod %s  Path %s\n",method,route);
    fflush(stdout);


    if (strcmp(method, "GET") != 0)
    {
      const char response[] = "HTTP/1.1 400 Bad Request\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
      send(clientSocket, response, strlen(response), 0);
    }
    else
    {

      char *token;


      token = strtok(route, "/");
      if (token == NULL) {
         NoCommandResponse();
      } else if (strcmp(token,"control") == 0) {
           char *sKey =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sKey = token;  token = strtok(NULL, "/");}
printf("\nsKey %s",sKey);fflush(stdout);
           if (sKey != NULL) {
              char *sValue =NULL;
              //token = strtok(NULL, "/");
              if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
printf("\nsValue %s",sValue);fflush(stdout);
              if (sValue != NULL) {
		callMIDIControlFunction(inst->midicfg, sKey, atoi(sValue));
                 notifyControlChangeByName(inst->midicfg, sKey, atoi(sValue));
    	         OkResponse();
              } else {
                char buffer[500];
                foo y;
                buffer[0] = 0;
                y.key = sKey;
                y.arg = buffer;
                strcat(buffer,"HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
                rc_loop_state (inst->state, &rolle, &y);
                send(clientSocket, buffer, strlen(buffer), 0);
              }
           } else {
                 char buffer[5000];
                 buffer[0] = 0;
                 strcat(buffer,"HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
                 rc_loop_state (inst->state, &pelle, buffer);
                 send(clientSocket, buffer, strlen(buffer), 0);
           }
      } else if (strcmp(token,"program") == 0) {
 	   char *sProgram =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sProgram = token;  token = strtok(NULL, "/");}
           if (sProgram != NULL) {
           	printf("\nSet Program %d\n",atoi(sProgram));
                fflush(stdout);
 		installProgram(inst,atoi(sProgram)-1);
	        OkResponse();
           } else {
	   	printf("\nGet program\n");
                fflush(stdout);
		ProgramResponse(inst);
           }

/*
      } else if (strcmp(token,"drawbar.upper.1") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+0, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+0]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.upper.2") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+1, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+1]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.upper.3") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+2, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+2]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.upper.4") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+3, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+3]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.upper.5") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+4, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+4]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.upper.6") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+5, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+5]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.upper.7") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+6, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+6]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.upper.8") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+7, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+7]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.upper.9") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,UPPER_BUS_LO+8, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[UPPER_BUS_LO+8]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.1") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+0, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+0]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.2") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+1, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+1]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.3") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+2, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+2]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.4") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+3, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+3]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.5") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+4, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+4]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.6") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+5, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+5]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.7") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+6, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+6]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.8") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+7, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+7]);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"drawbar.lower.9") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
		setDrawBar(inst->synth,LOWER_BUS_LO+8, rint (atof(sValue)) );
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", 8.0*inst->synth->drawBarGain[LOWER_BUS_LO+8]);
      		send(clientSocket, response, strlen(response), 0);
           }

      } else if (strcmp(token,"vibrato.upper") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setVibratoUpper (inst->synth, atoi(sValue));
	        OkResponse();
           } else {
	        char response[100];
                int enabled = 0;
                if (inst->synth->newRouting &= RT_UPPRVIB) enabled = 1;
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", enabled);
      		send(clientSocket, response, strlen(response), 0);
           }

      } else if (strcmp(token,"vibrato.lower") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setVibratoLower (inst->synth, atoi(sValue));
	        OkResponse();
           } else {
	        char response[100];
                int enabled = 0;
                if (inst->synth->newRouting &= RT_LOWRVIB) enabled = 1;
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", enabled);
      		send(clientSocket, response, strlen(response), 0);
           }

      } else if (strcmp(token,"vibrato.level") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setVibratoFromMIDI (inst->synth, 23*atoi(sValue));
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", 1);
      		send(clientSocket, response, strlen(response), 0);
           }


      } else if (strcmp(token,"percussion.active") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setPercussionEnabled (inst->synth, atoi(sValue));
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", inst->synth->percEnabled);
      		send(clientSocket, response, strlen(response), 0);
           }

      } else if (strcmp(token,"percussion.volume") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setPercussionVolume (inst->synth, atoi(sValue));
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", inst->synth->percIsSoft);
      		send(clientSocket, response, strlen(response), 0);
           }

      } else if (strcmp(token,"percussion.speed") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setPercussionFast (inst->synth, atoi(sValue));
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", inst->synth->percIsFast);
      		send(clientSocket, response, strlen(response), 0);
           }


      } else if (strcmp(token,"percussion.harmonics") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setPercussionFirst (inst->synth, atoi(sValue));
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", inst->synth->percSendBus == inst->synth->percSendBusA);
      		send(clientSocket, response, strlen(response), 0);
           }

      } else if (strcmp(token,"overdrive.active") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setClean (inst->preamp, atoi(sValue));
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", ((struct b_preamp*)(inst->preamp))->isClean);
      		send(clientSocket, response, strlen(response), 0);
           }

      } else if (strcmp(token,"rotary.speed") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                fflush(stdout);
                setRevSelect (inst->whirl, rint(atof(sValue)));
                fflush(stdout);
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%d", inst->whirl->revSelect);
      		send(clientSocket, response, strlen(response), 0);
           }

      } else if (strcmp(token,"reverb.mix") == 0) {
           char *sValue =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
           if (sValue != NULL) {
                setReverbMix (inst->reverb, atof(sValue));
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", inst->reverb->wet/(inst->reverb->wet+inst->reverb->dry));
      		send(clientSocket, response, strlen(response), 0);
           }


      } else if (strcmp(token,"swell") == 0) {
           char *sLevel =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sLevel = token;  token = strtok(NULL, "/");}
           if (sLevel != NULL) {
   		inst->synth->swellPedalGain = atof(sLevel);
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", inst->synth->swellPedalGain);
      		send(clientSocket, response, strlen(response), 0);
           }
      } else if (strcmp(token,"volume") == 0) {
           char *sLevel =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sLevel = token;  token = strtok(NULL, "/");}
           if (sLevel != NULL) {
   		inst->synth->outputLevelTrim = atof(sLevel);
	        OkResponse();
           } else {
	        char response[100];
      		sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n%f", inst->synth->outputLevelTrim);
      		send(clientSocket, response, strlen(response), 0);
           }
*/
      } else if (strcmp(token,"programs") == 0) {
		ProgramsResponse(inst);
/*
      } else if (strcmp(token,"noteoff") == 0) {
           //char *sChannel = NULL;
           //char *sKey = NULL;
           //char *sVelocity =NULL;
           //token = strtok(NULL, "/");
           //if (token != NULL) { sChannel = token;  token = strtok(NULL, "/");}
           //if (token != NULL) { sKey = token;  token = strtok(NULL, "/");}
           //if (token != NULL) { sVelocity = token;  token = strtok(NULL, "/");}
	   //MidiSendNoteOff(atoi(sChannel), atoi(sKey), atoi(sVelocity));
           OkResponse();
      } else if (strcmp(token,"controller") == 0) {
           //char *sChannel = NULL;
           //char *sController = NULL;
           //char *sValue =NULL;
           //token = strtok(NULL, "/");
           //if (token != NULL) { sChannel = token;  token = strtok(NULL, "/");}
           //if (token != NULL) { sController = token;  token = strtok(NULL, "/");}
           //if (token != NULL) { sValue = token;  token = strtok(NULL, "/");}
	   //MidiSendController(atoi(sChannel), atoi(sController), atoi(sValue));
           OkResponse();
*/
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
