#include <stdio.h>  // console input/output, perror
#include <stdlib.h> // exit
#include <string.h> // string manipulation
#include <netdb.h>  // getnameinfo

#include <sys/socket.h> // socket APIs
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // open, close

#include <signal.h> // signal handling
#include <time.h>   // time



#include "global_inst.h"
#include "program.h"

#define SIZE 1024  // buffer size
#define PORT 2729  // port number
#define BACKLOG 10 // number of pending connections queue will hold


void handleSignal(int signal);


int serverSocket;
int clientSocket;

char *request;

static void OkResponse() {
      const char response[] = "HTTP/1.1 200 OK\r\n\n";
      send(clientSocket, response, sizeof(response), 0);
}


void nisse(int cnt, int pgm, char *name, void *arg) {
   char kalle[50];
   kalle[0] = 0;
   sprintf(kalle,"%d|%s\n",pgm,name);
   strcat(arg,kalle);
}


static void ProgramsResponse(b_instance *inst) {
  char buffer[5000];
  buffer[0] = 0;
  strcat(buffer,"HTTP/1.1 200 OK\r\n\n");
  loopProgammes(inst->progs,0,nisse,buffer);
  send(clientSocket, buffer, strlen(buffer), 0);
}




static void ProgramResponse(b_instance *inst) {
      char response[40];
      sprintf(response, "HTTP/1.1 200 OK\r\n\n%d|%s",installedProgram+1,installedProgramName(inst));
      send(clientSocket, response, strlen(response), 0);
printf("\ncurrent program  %d",installedProgram+1);
}


static void NoCommandResponse() {
      const char response[] = "HTTP/1.1 404 No such command\r\n\n";
      send(clientSocket, response, sizeof(response), 0);
}


http_server_run(b_instance* inst)
{

  signal(SIGINT, handleSignal);

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
    printf("\nMethod %s  Path %s\n",method,route);


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
      } else if (strcmp(token,"program") == 0) {
           char *sProgram =NULL;
           token = strtok(NULL, "/");
           if (token != NULL) { sProgram = token;  token = strtok(NULL, "/");}
           if (sProgram != NULL) {
           	printf("\nSet Program %d\n",atoi(sProgram));
   		installProgram(inst,atoi(sProgram)-1);
	        OkResponse();
           } else {
	   	printf("\nGet program\n");
		ProgramResponse(inst);
           }
      } else if (strcmp(token,"programs") == 0) {
		ProgramsResponse(inst);
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
