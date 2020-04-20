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






	/* THREE-WAY-HANDSHAKE */





	//			1. SOLICITAR SINCRONIZACION DE INFO

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


	// 			2. ESPERAR RESPUESTA DE SINCRONIZACIÓN

	// Recibir mensajes del cliente conectado
	bzero(buffer, BUFSIZE);
	recv(sockfd, buffer, BUFSIZE, 0);

	if (1)
	{
		string ret(buffer, BUFSIZE);
	}
	ServerMessage sm;
	sm.ParseFromString(buffer);
	int option = sm.option();


	// MANEJAR MY INFO RESPONSE
	if (option == 4)
	{
		userId = sm.myinforesponse().userid();
		cout << "MY ID ES: " << userId << endl;


		// 			3. HACER SABER AL SERVIDOR QUE ESTAMOS LISTOS

		// Se crea instancia tipo MyInfoAcknowledge
		MyInfoAcknowledge * ack(new MyInfoAcknowledge);
		ack->set_userid(userId);

		// Se crea instancia de Mensaje, se setea los valores deseados
		ClientMessage m2;
		m2.set_option(6);
		m2.set_allocated_acknowledge(ack);

		// Se serializa el message a string
		string binary2;
		m2.SerializeToString(&binary2);

		char cstr2[binary2.size() + 1];
	    strcpy(cstr2, binary2.c_str());

		send(sockfd, cstr2, strlen(cstr2), 0);

	}



	sleep(5);


	// A PARTIR DE AQUÍ, VERIFICAR OPCIÓN QUE QUIERA ENVIAR
	// EL USUARIO AL SERVIDOR

	// EJEMPLO: CAMBIAR DE ESTADO


	// Se crea instancia de Mensaje, se setea los valores deseados
	connectedUserRequest * usersRequest(new connectedUserRequest);
	usersRequest->set_userid(0);
	// usersRequest->set_username(argv[1]);


	ClientMessage m3;
	m3.set_option(2);
	m3.set_allocated_connectedusers(usersRequest);

	// Se serializa el message a string
	string binary3;
	m3.SerializeToString(&binary3);

	char cstr3[binary3.size() + 1];
    strcpy(cstr3, binary3.c_str());

	send(sockfd, cstr3, strlen(cstr3), 0);

	printf("Esperando confirmación del servidor...\n");

	char buffer2[BUFSIZE];
	// Esperar respuesta del servidor
	bzero(buffer2, BUFSIZE);
	recv(sockfd, buffer2, BUFSIZE, 0);

	string ret(buffer2, BUFSIZE);
	ServerMessage sm2;
	sm2.ParseFromString(buffer2);
	int option2 = sm2.option();

	// MANEJAR ERRORES DE PARTE DEL CLIENTE
	if (option2 == 3)
	{
		string errorMessage = sm2.error().errormessage();
		cout << "El error es: " << errorMessage << endl;
	}













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