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

#include "Usuario.h"

using namespace std;
#define BUFSIZE 1024

class Clithread
{
public: 
	// Constructor
	Clithread(int socketN, int cidN, struct sockaddr_in addrN);

	// Datos de conexión
	struct sockaddr_in addr;
	int cid, socket, inactiveTime;
	time_t serverTime;
	socklen_t len;
	Usuario user;

	// Métodos de conexión cliente-servidor
	void ManageClient();
	int RegisterUser(Usuario newUser);
	void EndConnection();

	// Métodos de google protobuf
	void ManageProtoOption();
	void Synchronize(string username, string ip);
	void SendConnectedUsers(int userId, string username);
	void ChangeUserStatus(string newStatus);
	void SendBroadcastMessage(string message);
	void SendPrivateMessage(string message, int userId, string username);
	void SendError(int userId, string errorMsg);
	void AcknowledgeFromClient(int userId);

	// Métodos relacionados al diccionario
	void GetMap();
	void AddToMap(Usuario user);
	bool UsernameExists(string username);
	bool IpExists(string ip);
	int GetUsernameId(string username);
	void UpdateUsersStatus();

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