#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "mensaje.pb.h"

using namespace std;
using namespace chat;
using namespace google::protobuf;

#define BUFSIZE 1024

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n, userId;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	bool salir = false;
	char buffer[BUFSIZE];

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc < 3) {
	   fprintf(stderr,"usage %s hostname port\n", argv[0]);
	   exit(0);
	}

	portno = atoi(argv[3]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
		error("ERROR al abrir socket");

	
	// Obtener host desde consola
	server = gethostbyname(argv[2]);

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


	/*  ENVIAR MENSAJE AL SERVIDOR */

	// Se crea instacia tipo MyInfoSynchronize y se setean los valores deseables
	MyInfoSynchronize * miInfo(new MyInfoSynchronize);
	miInfo->set_username(argv[1]);
	miInfo->set_ip(argv[4]);

	// Se crea instancia de Mensaje, se setea los valores deseados
	ClientMessage m;
	m.set_option(1);
	m.set_allocated_synchronize(miInfo);

	// Se serializa el message a string
	string binary;
	m.SerializeToString(&binary);

	char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

	send(sockfd, cstr, strlen(cstr), 0);

	printf("Esperando confirmación del servidor...\n");

	// Recibir mensajes del cliente conectado
	bzero(buffer, BUFSIZE);
	recv(sockfd, buffer, BUFSIZE, 0);

	string ret(buffer, 8192);
	MyInfoResponse myInfo;
	myInfo.ParseFromString(buffer);
	userId = myInfo.userid();

	cout << "MY ID ES: " << userId << endl;

	// Se crea instancia de Mensaje, se setea los valores deseados
	MyInfoAcknowledge ack;
	ack.set_userid(userId);

	// Se serializa el message a string
	string binary2;
	ack.SerializeToString(&binary2);

	char cstr2[binary2.size() + 1];
    strcpy(cstr2, binary2.c_str());

	send(sockfd, cstr2, strlen(cstr2), 0);


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

			else if (strcmp(buffer, "privado") == 0)
			{
				printf("Ingrese el tid del cliente: ");
				bzero(buffer, BUFSIZE);
				fgets(buffer, BUFSIZE, stdin);
				send(sockfd, buffer, BUFSIZE, 0);
			}

		} while (*buffer != '#');

	} while (!salir);

	printf("CONEXIÓN TERMINADA\n");
	close(sockfd);
	ShutdownProtobufLibrary();
	return 0;
}