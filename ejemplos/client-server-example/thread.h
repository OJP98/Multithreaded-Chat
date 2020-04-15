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
#include <map>

using namespace std;
#include "../../cliente/Usuario.h"

#define BUFSIZE 1024

class Clithread
{
public: 
	// Constructor
	Clithread(int socketN, int cidN, struct sockaddr_in addrN);

	// Datos de conexión
	struct sockaddr_in addr;
	int cid, socket, *clientCount;
	socklen_t len;
	Usuario user;

	// Métodos de conexión cliente-servidor
	void ConnectWithClient();
	int RegisterUser();
	void ManageClient();
	void EndConnection();

	// Métodos de comunicación cliente - cliente
	void SendPrivateMessage();

	// Métodos relacionados al diccionario
	void GetMap();
	void AddToMap(Usuario user);
	bool UsernameExists(string username);
	bool IpExists(string ip);
	int GetUsernameId(string username);

	// Manejo de errores
	void error(const char *msg);
	void ManageRecvError(int errorno);

	// Variables útiles.
	bool closeConnection;
	char buffer[BUFSIZE];


private:
	// Diccionario de usuarios
	static map<int, Usuario> dict;
};

#endif