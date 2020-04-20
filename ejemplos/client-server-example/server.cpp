#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <map>
using namespace std;

// #include "mensaje.pb.h"
#include "thread.h"
#include "../../cliente/Usuario.h"

#define MAX_CLIENTS 20
#define BUFSIZE 	1024

// Unas variables útiles
int sockfd, portno;
int clientCount = 0;
int idCount = 1;
struct sockaddr_in serv_addr;
pthread_t threadPool[MAX_CLIENTS];
void * retvals[MAX_CLIENTS];

// Diccionario de usuarios
// Usuario nuevoUsuario("TEST","69.0.0.1",69);

// Diccionario de usuarios
std::map<int, Usuario> Clithread::dict = {};

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

// Estructura para manejar datos de conexión
struct connection_data {
	int socket;
	int cid;
	struct sockaddr_in addr;
};


void *manageNewThread(void *args)
{
	connection_data *data;
	data = (struct connection_data *) args;

	Clithread clithread(
		data->socket,
		data->cid,
		data->addr
	);

	clithread.ManageClient();

	clientCount--;
	pthread_exit(NULL);
}

void Initialize()
{
	// Crear un socket
	// socket(int domain, int type, int protocol)
	sockfd =  socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR no se pudo abrir el socket");

	// Liberar memoria para obtener address de usuario
	bzero((char *) &serv_addr, sizeof(serv_addr));

	/* setup de la dirección del host (host_addr) al hacer el bind de la llamada */
	serv_addr.sin_family = AF_INET;  

	// Usar IP del host automática
	serv_addr.sin_addr.s_addr = INADDR_ANY;  

	// IMPORTANTE: convertir el integer para usarlo como byte de network.
	serv_addr.sin_port = htons(portno);

	// bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
	// bind() pasa lo siguiente: file descriptor, estructura de dirección
	// y el largo de la estructura de dirección
	// Este bind() bindea el socket a la dirección IP actuar en el puerto, portno
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR, no se pudo hacer bind");

	if (listen(sockfd, MAX_CLIENTS) == 0)
		printf("SERVER - Haciendo listen para nuevos clientes...\n");
	else
		error("ERROR, no se pudo hacer listen");
}

void ListenForConnections()
{
	while (clientCount < MAX_CLIENTS)
	{
		struct sockaddr_in cli_addr;
		socklen_t cli_len = sizeof(cli_addr);

        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);

        if (newsockfd < 0)
        	error("ERROR, no se pudo establecer conexión con cliente");
        else
        {
            printf("\nSERVER - Nuevo cliente conectándose...\n");
        }

        struct connection_data new_connection;
        new_connection.socket = newsockfd;
        new_connection.addr = cli_addr;
        new_connection.cid = idCount;

        pthread_create(
        	&threadPool[clientCount],
        	NULL,
        	manageNewThread,
        	(void *)&new_connection
        );

        clientCount++;
        idCount++;
	}
}

void CloseServer()
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (pthread_join(threadPool[i], &retvals[i]) < 0)
			error("No fue posible cerrar el thread pool.");
	}
	close(sockfd);
}

int main(int argc, char *argv[])
{
	// Verificar si se obtuvo argumento o no
	if (argc < 2) {
		fprintf(stderr,"ERROR, no se obtuvo un puerto\n");
		exit(1);
	}

	// Convertir el argumento (puerto) a entero.
	portno = atoi(argv[1]);

	Initialize();
	ListenForConnections();
	CloseServer();

	return 0; 
}