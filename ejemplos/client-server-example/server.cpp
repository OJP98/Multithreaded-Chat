#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// #include "mensaje.pb.h"
// using namespace chat;
using namespace std;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	bool salir = false;
	int bufsize = 1024;
	char buffer[bufsize];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

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

	// listen() le indica al socket escuchar conexiones.
	// listen() automaticamente tiene una cola de requests
	// El máximo, por defecto, es 5.
	listen(sockfd,SOMAXCONN);

	// Se prepara un espacio para la dirección del cliente
	clilen = sizeof(cli_addr);

	// Se acepta la conexión con la infromación del cliente en la estructura del cliente.
	// Retorna un nuevo file descriptor de la conexión aceptada, este se puede seguir usando
	// para mantener comunicación con el cliente.
	int cantClientes = 1;
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

	if (newsockfd < 0)
		 error("ERROR, no fue posible aceptar el socket del cliente");


	while (newsockfd > 0) 
	{
		strcpy(buffer, "SERVIDOR CONECTADO!\n");
		send(newsockfd, buffer, clilen, 0);
		cout << "Conectado con el cliente #" << cantClientes << "\n";
		cout << "Para terminar la conexión, escribir #\n";

		printf("\nCliente: ");
		do {
			bzero(buffer, bufsize);
			recv(newsockfd, buffer, bufsize, 0);
			printf("%s ", buffer);
			// cout << buffer << " ";
			if (*buffer == '#') {
				*buffer = '*';
				salir = true;
			}
		} while (*buffer != '*');

		do {
			printf("\n");
			do {
				printf("Servidor: ");
				bzero(buffer, bufsize);
				fgets(buffer, bufsize, stdin);
				// cin >> buffer;
				send(newsockfd, buffer, bufsize, 0);
				if (*buffer == '#') {
					send(newsockfd, buffer, bufsize, 0);
					*buffer = '*';
					salir = true;
				}
			} while (*buffer != '*');

			printf("\nCliente: ");
			do {
				recv(newsockfd, buffer, bufsize, 0);
				printf("%s ", buffer);
				// cout << buffer << " ";
				if (*buffer == '#') {
					*buffer == '*';
					salir = true;
				}
			} while (*buffer != '*');

		} while (!salir);

		printf("\nTerminando conexión con: %s\n", inet_ntoa(cli_addr.sin_addr));
		close(newsockfd);
		salir = false;
		exit(1);
	}


	// printf("server: Se obtuvo conexión exitosa de %s en el puerto %d\n",
	// 	inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port));

	// // Enviar hola mundo al usuario
	// send(newsockfd, "Holaa, mundo!\n", 13, 0);

	// // Preparar buffer
	// bzero(buffer,256);

	// // Leer ocntenido del socket
	// n = read(newsockfd,buffer,255);

	// if (n < 0)
	// 	error("ERROR no se pudo leer del socket");

	// printf("El mesnaje del cliente es: %s\n",buffer);

	// Cerrar conexiones	
	// close(newsockfd);
	close(sockfd);
	return 0; 
}