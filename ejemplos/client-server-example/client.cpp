#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

// Pa' compilar
#define BUFSIZE 1024

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	bool salir = false;
	char buffer[BUFSIZE];

	if (argc < 3) {
	   fprintf(stderr,"usage %s hostname port\n", argv[0]);
	   exit(0);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
		error("ERROR al abrir socket");

	
	// Obtener host desde consola
	server = gethostbyname(argv[1]);

	if (server == NULL) {
		fprintf(stderr,"ERROR, no existe la dirección del host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	bcopy((char *)server -> h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR, no se pudo conectar al servidor");

	printf("Esperando confirmación del servidor...\n");
	recv(sockfd, buffer, BUFSIZE, 0);
	printf("Conexión aceptada!\n\n");


	do {
		printf("\n");
		do {
			printf("Cliente: ");
			bzero(buffer, BUFSIZE);
			fgets(buffer, BUFSIZE, stdin);
			send(sockfd, buffer, BUFSIZE, 0);
			if (*buffer == '#') {
				send(sockfd, buffer, BUFSIZE, 0);
				salir = true;
			}

		} while (*buffer != '#');

	} while (!salir);

	printf("CONEXIÓN TERMINADA\n");
	close(sockfd);
	return 0;
}