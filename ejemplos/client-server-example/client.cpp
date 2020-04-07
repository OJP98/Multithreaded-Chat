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
	int bufsize = 1024;
	char buffer[bufsize];

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
	recv(sockfd, buffer, bufsize, 0);
	printf("Conexión aceptada!\n\n");


	do {
		printf("\n");
		do {
			printf("Cliente: ");
			bzero(buffer, bufsize);
			fgets(buffer, bufsize, stdin);
			// cin >> buffer;
			send(sockfd, buffer, bufsize, 0);
			if (*buffer == '#') {
				send(sockfd, buffer, bufsize, 0);
				*buffer = '*';
				salir = true;
			}
		} while (*buffer != '*');

		printf("\nServidor: ");
		do {
			recv(sockfd, buffer, bufsize, 0);
			printf("%s ", buffer);
			// cout << buffer << " ";
			if (*buffer == '#') {
				*buffer = '*';
				salir = true;
			}

		} while (*buffer != '*');

	} while (!salir);

	// printf("Ingrese el mensaje a enviar: ");
	// bzero(buffer,256);
	// fgets(buffer,255,stdin);
	// n = write(sockfd, buffer, strlen(buffer));

	// if (n < 0) 
	//      error("ERROR, no se pudo escribir al socket");

	// bzero(buffer,256);
	// n = read(sockfd, buffer, 255);

	// if (n < 0) 
	//      error("ERROR no se pudo leer del socket");

	// printf("%s\n", buffer);
	printf("CONEXIÓN TERMINADA\n");
	close(sockfd);
	return 0;
}