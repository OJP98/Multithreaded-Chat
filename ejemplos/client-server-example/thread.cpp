#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <condition_variable>
#include <map>
#include <chrono>

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
	client_inactivty_time = 5;
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

/* METODOS DE CONEXIÓN CON EL CLIENTE */
void Clithread::ManageClient()
{
	printf("SERVER - Ahora escuchando al cliente: %d\n",socket);
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
	printf("\nSERVER - Evaluando opciones del client message\n");
	string ret(buffer, BUFSIZE);

	ClientMessage cm;
	cm.ParseFromString(buffer);
	int32 option = cm.option();

	// MY INFO SYNCHRONIZE
	if(option == 1)
	{
		string cm_ip = "";
		string cm_username = cm.synchronize().username();

		if (cm.synchronize().has_ip())
			cm_ip = cm.synchronize().ip();
		else
			cm_ip = inet_ntoa(addr.sin_addr);

		printf("SERVER - La opción es 1! synchronize con user e ip\n");
		Synchronize(cm_username, cm_ip);
	}

	// CONNECTED USERS
	else if(option == 2)
	{
		int cm_userId = -1;
		string cm_username = "";
		if (cm.connectedusers().has_userid())
			cm_userId = cm.connectedusers().userid();

		if (cm.connectedusers().has_username())
			cm_username = cm.connectedusers().username();

  		printf("SERVER - La opción es 2! Enviar usuario(s) conectado(s)\n");
		SendConnectedUsers(cm_userId, cm_username);
	}

	// CHANGE STATUS
	else if(option == 3)
	{
		if (cm.changestatus().has_status())
		{
			string cm_status = cm.changestatus().status();
			printf("SERVER - La opción es 3! Cambiar de status\n");
			ChangeUserStatus(cm_status);
		}
		else
			SendError(cid, "Server didn't recieved any status.");

	}

	// BROADCAST
	else if(option == 4)
	{
		string cm_message;

		if (cm.directmessage().has_message() && cm.directmessage().message().compare("") != 0)
		{
			cm_message = cm.broadcast().message();
			printf("SERVER - La opción es 4! Broadcast message\n");
			SendBroadcastMessage(cm_message);
		}
		else
			SendError(cid, "Server didn't received any message.");
	}

	// DIRECT MESSAGE
	else if(option == 5)
	{
		string cm_message;
		int cm_userId = -1;
		string cm_username = "";

		if (cm.directmessage().has_message() && cm.directmessage().message().compare("") != 0)
		{
			cm_message = cm.directmessage().message();

			if (cm.directmessage().has_userid())
				cm_userId = cm.directmessage().userid();

			if (cm.directmessage().has_username())
				cm_username = cm.directmessage().username();

			printf("SERVER - La opción es 5! Mandar mensaje directo\n");
			SendPrivateMessage(cm_message, cm_userId, cm_username);
		}
		else
			SendError(cid, "Server didn't received any message.");

	}

	// INFO ACKNOWLEDGE
	else if (option == 6)
	{
		if (cm.acknowledge().has_userid())
		{
			int cm_userId = cm.acknowledge().userid();
			printf("SERVER - La opción es 6! Acknowledge por parte del cliente\n");
			AcknowledgeFromClient(cm_userId);
		}
		else
			SendError(cid, "Server didn't received any user id.");
	}

	// El cliente mandó una opción desconocida
	else
	{
		SendError(cid, "Server couldn't match any of the options requested");
	}

}


void Clithread::Synchronize(string username, string ip = "")
{
	// Preparar mensaje de respuesta

	MyInfoResponse * infoResponse(new MyInfoResponse);
	infoResponse->set_userid(cid);

	ServerMessage sm;
	sm.set_option(4);
	sm.set_allocated_myinforesponse(infoResponse);

	string binary;
	sm.SerializeToString(&binary);

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
	// Indicar objeto a enviar
	ConnectedUserResponse * response(new ConnectedUserResponse);

	// La opción a mandar del lado del servidor es la 6
	ServerMessage sm;
	sm.set_option(5);

	// Todos los usuarios
	if (userId == 0)
	{
		map<int, Usuario>::iterator itr; 
	    for (itr = dict.begin(); itr != dict.end(); ++itr)
	    { 

	    	// Obtener el id del usuario iterado
	    	int itrUserId = itr->first;
	    	// Obtener el usuario del diccionario
			Usuario u = itr->second;

			// Armar el usuario y agregarlo al response
			ConnectedUser* connectedUser = response->add_connectedusers();
			connectedUser->set_username(u.user);
			connectedUser->set_status(u.estado);
			connectedUser->set_ip(u.ip);
	    } 
	}

	// Usuario específico
	else if (userId > 0)
	{
		Usuario u = dict[userId];

		ConnectedUser* connectedUser = response->add_connectedusers();
		connectedUser->set_username(u.user);
		connectedUser->set_status(u.estado);
		connectedUser->set_ip(u.ip);
	}

	// Buscar por username
	else if (username.compare("") != 0)
	{
		int destUserId = GetUsernameId(username);

		// Si el username indicado existe
		if (destUserId >= 0)
		{
			Usuario u = dict[destUserId];
			ConnectedUser* connectedUser = response->add_connectedusers();
			connectedUser->set_username(u.user);
			connectedUser->set_status(u.estado);
			connectedUser->set_ip(u.ip);
		}

		// De lo contario, enviar error
		else
		{
			printf("Server - El username especificado no existe!\n");
			SendError(cid, "Specified Username doesn't exist.");
		}
	}

	// Si nada cumple, enviar error
	else
		SendError(cid, "Server couldn't match id nor username.");


	// Alojar la respuesta
	sm.set_allocated_connecteduserresponse(response);

	// Preparar y enviar el mensaje
	string binary;
	sm.SerializeToString(&binary);

	char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

	// Enviar el mensaje serializado al cliente.
	int sent = send(socket, cstr, strlen(cstr), 0);
	if (sent == 0)
	{
		fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
		closeConnection = true;
	}
	printf("SERVER - Los usuarios conectados fueron enviados al cliente\n");
}


void Clithread::ChangeUserStatus(string newStatus)
{
	cout << "SERVER - Se actualizo el estado de " << dict[cid].estado << " a " << newStatus << endl;
	dict[cid].estado = newStatus;

	ChangeStatusResponse * response(new ChangeStatusResponse);
	response->set_userid(cid);
	response->set_status(dict[cid].estado);

	ServerMessage sm;
	sm.set_option(6);
	sm.set_allocated_changestatusresponse(response);

	string binary;
	sm.SerializeToString(&binary);

	char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

	// Enviar el mensaje serializado al cliente.
	int sent = send(socket, cstr, strlen(cstr), 0);
	if (sent == 0)
	{
		fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
		closeConnection = true;	
	}
}


void Clithread::SendBroadcastMessage(string message)
{
	// Armar el mensaje de broadcast.
	BroadcastMessage * msg(new BroadcastMessage);
	msg->set_message(message);
	msg->set_userid(cid);

	// Armar ahora el server message
	ServerMessage sm;
	sm.set_option(1);
	sm.set_allocated_broadcast(msg);

	// Preparar mensaje en string
	string binary;
	sm.SerializeToString(&binary);

	char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

	// 1. Mandar el mensaje a todos los clientes
	map<int, Usuario>::iterator itr; 
    for (itr = dict.begin(); itr != dict.end(); ++itr)
    { 
    	// Obtener el id del usuario iterado
    	int itrUserId = itr->first;
    	// Obtener el usuario del diccionario
		Usuario u = itr->second;

		// Enviar el mensaje al cliente.
		int sent = send(u.socket, cstr, strlen(cstr), 0);
		if (sent == 0)
		{
			fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
			SendError(cid, "Server couldn't reach one of the clients");
		}

    } 

	// 2. Una vez enviado el mensaje, hacerle saber al cliente que lo mandó
	BroadcastResponse * response(new BroadcastResponse);
	response->set_messagestatus("TEST");

	ServerMessage sm2;
	sm2.set_option(7);
	sm2.set_allocated_broadcastresponse(response);

	// Preparar mensaje en string
	string resp;
	sm2.SerializeToString(&resp);

	char rstr[resp.size() + 1];
    strcpy(rstr, resp.c_str());

    // Enviar el response al cliente.
	int sent = send(socket, cstr, strlen(cstr), 0);
	if (sent == 0)
	{
		fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
		closeConnection = true;
	}
}


void Clithread::SendPrivateMessage(string message, int userId, string username)
{
	int destSocket;

	// Si el usuario manda un id, buscar el usuario con ese id.
	// PREGUNTA: Debería validar si existe el id?
	if (userId > 0)
	{
		Usuario destUser = dict[userId];
		destSocket = destUser.socket;
	}
	else if (username.compare("") != 0)
	{
		int destUserId = GetUsernameId(username);

		if (destUserId >= 0)
		{
			Usuario destUser = dict[destUserId];
			destSocket = destUser.socket;
			userId = destUserId;
		}
		else
		{
			printf("Server - El username especificado no existe!\n");
			SendError(cid, "Specified username doesn't exist.");
		}
	}

	// Enviar mensaje al cliente solicitado
	DirectMessage * dm(new DirectMessage);
	dm->set_message(message);
	dm->set_userid(userId);
	dm->set_username(username);

	ServerMessage newSM;
	newSM.set_option(2);
	newSM.set_allocated_message(dm);

	string dmBinary;
	newSM.SerializeToString(&dmBinary);

	char dmstr[dmBinary.size() + 1];
	strcpy(dmstr, dmBinary.c_str());

	int dmSent = send(destSocket, dmstr, strlen(dmstr), 0);
	if (dmSent == 0)
	{
		fprintf(stderr, "ERROR al enviar mensaje de cliente a cliente\n");
		SendError(cid, "Server couldn't send message to client.");
	}
	else
	{
		// Enviar response al cliente que solicitó el mensaje
		DirectMessageResponse * response(new DirectMessageResponse);
		response->set_messagestatus("TEST");

		ServerMessage sm;
		sm.set_option(8);
		sm.set_allocated_directmessageresponse(response);

		string binary;
		sm.SerializeToString(&binary);

		char cstr[binary.size() + 1];
	    strcpy(cstr, binary.c_str());

		// Enviar el mensaje serializado al cliente.
		int sent = send(socket, cstr, strlen(cstr), 0);
		if (sent == 0)
		{
			fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
			closeConnection = true;	
		}
	}
}


void Clithread::SendError(int userId, string errorMsg)
{
	// Enviar response al cliente que solicitó el mensaje
	ErrorResponse * response(new ErrorResponse);
	response->set_errormessage(errorMsg);

	ServerMessage sm;
	sm.set_option(3);
	sm.set_allocated_error(response);

	string binary;
	sm.SerializeToString(&binary);

	char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

	// Enviar el mensaje serializado al cliente.
	int sent = send(socket, cstr, strlen(cstr), 0);
	if (sent == 0)
	{
		fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
		closeConnection = true;	
	}
}

void Clithread::AcknowledgeFromClient(int userId)
{
	// Comprobar que el user id sea el mismo y luego
	// intentar registrar al usuario
	if (userId == cid)
	{
		printf("SERVER - Se logra recibir acknowledge del cliente\n");
		if(RegisterUser(user) == 0)
		{
			// Informar al cliente que user o ip ya existen
			SendError(userId, "Username or ip has already been taken.");
			closeConnection = true;
		}
	}
	// De lo contrario, no se recibió lo que se esparaba... mandar error
	else
	{
		SendError(cid, "Client user id doesn't match the one received.");
		closeConnection = true;
	}
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
