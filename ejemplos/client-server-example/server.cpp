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
#include <vector> 
using namespace std;
// #include "mensaje.pb.h"

#define MAX_CLIENTS 20
#define BUFSIZE 	1024

// Pa' compilar: g++ -o server server.cpp -lpthread -lns

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

struct connection_data {
	int cli_socket;
	int tid;
	struct sockaddr_in cli_addr;
	socklen_t cli_len;
};

void *connectWithClient(void *args)
{
	char buffer[BUFSIZE];
	bool salir = false;

	struct connection_data *data;
	data = (struct connection_data *) args;

	struct sockaddr_in serv_addr;
	int cli_socket = data->cli_socket;
	int tid = data->tid;
	struct sockaddr_in cli_addr = data->cli_addr;
	socklen_t cli_len = data -> cli_len;

	if (cli_socket > 0) 
	{
		strcpy(buffer, "SERVIDOR CONECTADO!\n");
		send(cli_socket, buffer, cli_len, 0);

		do {
			printf("\nCliente: ");
			recv(cli_socket, buffer, BUFSIZE, 0);
			printf("%s", buffer);
			if (*buffer == '#') {
				printf("\nSERVER: Me pierdes cliente...\nF en el chat amigos\n");
				salir = true;
			}
		} while (*buffer != '#');


		printf("Terminando conexión con: %s\n", inet_ntoa(cli_addr.sin_addr));
		close(cli_socket);
		salir = false;
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int sockfd, portno;
	struct sockaddr_in serv_addr;
	vector<pthread_t> threadVector;
	pthread_t threadPool[MAX_CLIENTS];
	void * retvals[MAX_CLIENTS];

	if (argc < 2) {
		fprintf(stderr,"ERROR, no se obtuvo un puerto\n");
		exit(1);
	}
	// Crear un socket
	// socket(int domain, int type, int protocol)
	sockfd =  socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR no se pudo abrir el socket");

	// Liberar memoria para obtener address de usuario
	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

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
		printf("Haciendo listen para nuevos clientes...\n");
	else
		error("ERROR, no se pudo hacer listen");

	int clientCount = 0;

	while (clientCount < MAX_CLIENTS)
	{
		struct sockaddr_in cli_addr;
		socklen_t cli_len = sizeof(cli_addr);

        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);

        if (newsockfd < 0)
        	error("ERROR, no se pudo establecer conexión con cliente");
        else
        {
            printf("NUEVO CLIENTE ONLINE!\n");
        }

        struct connection_data new_connection;
        new_connection.cli_socket = newsockfd;
        new_connection.cli_addr = cli_addr;
        new_connection.cli_len = cli_len;
        new_connection.tid = clientCount;

        pthread_create(
        	&threadPool[clientCount],
        	NULL,
        	connectWithClient,
        	(void *)&new_connection
        );

        clientCount++;
	}

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (pthread_join(threadPool[i], &retvals[i]) < 0)
			error("No fue posible cerrar el thread pool.");
	}
	close(sockfd);
	return 0; 
}