#ifndef CLI_THREAD_H
#define CLI_THREAD_H

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

using namespace std;

#define BUFSIZE 1024

class Clithread
{
public: 
	// Constructor
	Clithread(int socketN, int tidN, struct sockaddr_in addrN);

	// Estructura para manejar datos de conexión
	struct connection_data {
		int socket;
		int tid;
		struct sockaddr_in addr;
	};

	// Datos de conexión
	struct sockaddr_in addr;
	int tid;
	int socket;
	socklen_t len;

	// Métodos de conexión cliente-servidor
	void ConnectWithClient();
	void ManageClient();
	void EndConnection();


	// Variables útiles.
	bool closeConnection;
	char buffer[BUFSIZE];

};

#endif