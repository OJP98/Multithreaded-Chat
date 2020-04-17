#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <condition_variable>
#include <map>

#include "thread.h"
#include "mensaje.pb.h"
#include "../../cliente/Usuario.h"

using namespace std;
using namespace chat;
using namespace google::protobuf;

Clithread::Clithread(int socketN, int cidN, struct sockaddr_in addrN)
{
	socket = socketN;
	cid = cidN;
	addr = addrN;
	len = sizeof(addr);
	closeConnection = false;
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

/* METODOS DE CONEXIÓN CON EL CLIENTE */
void Clithread::ConnectWithClient()
{

	// Recibir mensajes del cliente conectado
	bzero(buffer, BUFSIZE);
	int received = recv(socket, buffer, BUFSIZE, 0);

	if (received > 0)
	{
		ManageProtoOption();
	}
	else
		ManageRecvError(received);
}


void Clithread::ManageClient()
{
	printf("SERVER - Ahora a escuchar al cliente\n\n");
	while (socket > 0 && !closeConnection)
	{
		// Recibir mensajes del cliente constantemente
		bzero(buffer, BUFSIZE);
		int received = recv(socket, buffer, BUFSIZE, 0);

		if (received > 0)
		{

			if (*buffer == '#') {
				printf("\nSERVER - Me pierdes cliente...\nSERVER - F en el chat amigos\n");
				closeConnection = true;
			}

			else if (strcmp(buffer, "privado") == 0)
			{
				printf("SERVER - Alguien está enviando un mensaje privado...\n");
				GetMap();
				// SendPrivateMessage();
			}

			else
			{
				ManageProtoOption();
			}
		}
		else
			ManageRecvError(received);
	}

	EndConnection();
}


/* METODOS RELACIONADOS CON EL PROTOCOLO */
void Clithread::ManageProtoOption()
{
	printf("SERVER - Evaluando opciones del client message\n");
	string ret(buffer, BUFSIZE);

	ClientMessage cm;
	cm.ParseFromString(buffer);
	int32 option = cm.option();

	// MY INFO SYNCHRONIZE
	if(option == 1)
	{
		string cm_username = cm.synchronize().username();
		string cm_ip = cm.synchronize().ip();
		printf("SERVER - La opción es 1! synchronize con user e ip\n");
		// if (cm_ip != '')
			// cm_ip = inet_ntoa(addr.sin_addr);

		Synchronize(cm_username, cm_ip);
	}

	// CONNECTED USERS
	else if(option == 2)
	{
		int cm_userId = cm.connectedusers().userid();
  		string cm_username = cm.connectedusers().username();

		SendConnectedUsers(cm_userId, cm_username);
	}

	// CHANGE STATUS
	else if(option == 3)
	{
		string cm_status = cm.changestatus().status();
		ChangeUserStatus(cm_status);
	}

	// BROADCAST
	else if(option == 4)
	{
		string cm_message = cm.broadcast().message();
		BroadcastMessage(cm_message);
	}

	// DIRECT MESSAGE
	else if(option == 5)
	{
		string cm_message = cm.directmessage().message();
		int cm_userId = cm.directmessage().userid();
		string cm_username = cm.directmessage().username();
		SendPrivateMessage(cm_message, cm_userId, cm_username);
	}

	// INFO ACKNOWLEDGE
	else if (option == 6)
	{
		int cm_userId = cm.acknowledge().userid();
		printf("SERVER - La opción es 6! Acknowledge por parte del cliente\n");
		AcknowledgeFromClient(cm_userId);
	}

}


void Clithread::Synchronize(string username, string ip = "")
{
	// Preparar mensaje de respuesta
	MyInfoResponse myInfo;
	myInfo.set_userid(cid);

	string binary;
	myInfo.SerializeToString(&binary);

	char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

	// Enviar el mensaje serializado al cliente.
	int sent = send(socket, cstr, strlen(cstr), 0);
	if (sent == 0)
	{
		fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
		closeConnection = true;
	}

	printf("SERVER - El id fue entregado al cliente\n");
	user.user = username;
	user.ip = ip;
	user.socket = socket;

}


void Clithread::SendConnectedUsers(int userId = 0, string username = "")
{
	// TODO: usar ConnectedUser
}


void Clithread::ChangeUserStatus(string newStatus)
{
	// TODO: usar ChangeStatusResponse
}


void Clithread::BroadcastMessage(string message)
{
	// TODO: usar BroadcastResponse y BroadcastMessage
}


void Clithread::SendPrivateMessage(string message, int userId, string username)
{
	// Recibir mensajes del cliente constantemente
	bzero(buffer, BUFSIZE);
	int received = recv(socket, buffer, BUFSIZE, 0);

	if (received > 0)
	{
		// Quitar el caracter de espacio \n
		if('\n' == buffer[strlen(buffer) - 1])
			buffer[strlen(buffer) - 1] = '\0';

		int destUserId = GetUsernameId(buffer);

		if (destUserId >= 0)
		{
			Usuario destUser = dict[destUserId];

			bzero(buffer, BUFSIZE);
			strcpy(buffer, "MENSAJE PRIVADO DE PARTE DE ALGUIEN...\n");
			send(destUser.socket, buffer, BUFSIZE, 0);
		}
		else
		{
			// TODO: Indicarle al usuario que ese usuario no existe
			printf("ALGUIEN DIGALE A ESTE PENDEJO QUE ESE USUARIO NO EXISTE!\n");
		}
	}

	else
		ManageRecvError(received);
}


void Clithread::SendError(int userId)
{
	// TODO: Usar ErrorResponse
}

void Clithread::AcknowledgeFromClient(int userId)
{
	// Comprobar que el user id sea el mismo y luego
	// intentar registrar al usuario
	if (userId == cid)
	{
		printf("SERVER - Se logra recibir acknowledge del cliente\n");
		if(RegisterUser(user) != 0)
			// Informar al cliente que user o ip ya existen
			SendError(userId);
	}
	// De lo contrario, no se recibió lo que se esparaba... mandar error
	else
		SendError(userId);
}


/* FUNCIONES RELACIONADAS AL MAP */
void Clithread::GetMap()
{
	printf("LA LISTA DE USUARIOS CONECTADOS ES:\n");
	map<int, Usuario>::iterator itr; 
	cout << "\tUSERNAME\tESTADO\n"; 
    for (itr = dict.begin(); itr != dict.end(); ++itr) { 

		int userId = itr->first;
		Usuario u = itr->second;

        cout << '\t' << u.user
             << '\t' << u.estado << '\n'; 
    } 
    cout << endl;

}


void Clithread::AddToMap(Usuario newUser)
{
	dict.insert(pair<int, Usuario>(cid, newUser));
	printf("SERVER - Usuario agregado al diccionario!\n");
	GetMap();
}


int Clithread::GetUsernameId(string username)
{
    map<int, Usuario>::iterator itr;
	for (itr = dict.begin(); itr != dict.end(); ++itr)
	{
		// Se itera todos los id's y usuarios en el diccionario
		int userId = itr->first;
		Usuario u = itr->second;

		// Se compara el nombre del usuario con el parámetro
		if (u.user.compare(username) == 0)
			return userId;
	}
	return -1;
}


int Clithread::RegisterUser(Usuario newUser)
{
	if (!UsernameExists(newUser.user) && !IpExists(newUser.ip))
	{
		printf("SERVER - El usuario no está registrado, agregando al map.\n");
		AddToMap(newUser);
		return 1;
	}
	else
	{
		printf("IP O USERNAME YA EXISTEN\n");
		closeConnection = true;
		return 0;
	}
}


bool Clithread::UsernameExists(string username)
{
	map<int, Usuario>::iterator itr;
	for (itr = dict.begin(); itr != dict.end(); ++itr)
	{
		Usuario u = itr->second;
		if (u.user.compare(username) == 0)
			return true;
	}
	return false;
}


bool Clithread::IpExists(string ip)
{
	map<int, Usuario>::iterator itr;
	for (itr = dict.begin(); itr != dict.end(); ++itr)
	{
		Usuario u = itr->second;
		if (u.ip.compare(ip) == 0)
			return true;
	}
	return false;
}


/* MANEJO DE ERRORES Y CIERRE DE CONEXIÓN */
void Clithread::EndConnection()
{
	printf("SERVER - Terminando conexión con: %s, socket %d\n", inet_ntoa(addr.sin_addr), socket);
	// Eliminar usuario del map si existe
	dict.erase(cid);
	// Cerrar socket
	close(socket);

	// exit(0);
}


void Clithread::ManageRecvError(int errorno)
{
	if (errorno == -1)
	{
		printf("\nSERVER - ERROR, no se pudo recibir el mensaje del cliente\n");
	}

	else if (errorno == 0)
	{
		printf("\nSERVER - Conexión cerrada por parte del cliente: %d\n",socket);
	}

	else
	{
		error("ERROR desconocido al recibir mensaje");
	}
	closeConnection = true;
}


void Clithread::error(const char *msg)
{
	perror(msg);
	exit(0);
}
