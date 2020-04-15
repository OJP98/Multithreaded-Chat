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
#include "../../cliente/Usuario.h"

using namespace std;

Clithread::Clithread(int socketN, int cidN, struct sockaddr_in addrN)
{
	socket = socketN;
	cid = cidN;
	addr = addrN;
	len = sizeof(addr);
	closeConnection = false;
}


void Clithread::ConnectWithClient()
{
	strcpy(buffer, "SERVIDOR CONECTADO!\n");
	int sent = send(socket, buffer, len, 0);

	if (sent == 0)
	{
		fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
		EndConnection();
	}
	else
	{
		printf("SERVER - Conexión con nuevo cliente establecida.\n");

		if (RegisterUser() > 0)
		{
			ManageClient();
		}
	}
}


void Clithread::ManageClient()
{
	while (socket > 0 && !closeConnection)
	{
		// Recibir mensajes del cliente constantemente
		bzero(buffer, BUFSIZE);
		int received = recv(socket, buffer, BUFSIZE, 0);

		if (received > 0)
		{
			// Quitar el caracter de espacio \n
			if('\n' == buffer[strlen(buffer) - 1])
				buffer[strlen(buffer) - 1] = '\0';

			if (*buffer == '#') {
				printf("\nSERVER - Me pierdes cliente...\nSERVER - F en el chat amigos\n");
				closeConnection = true;
			}

			else if (strcmp(buffer, "privado") == 0)
			{
				printf("SERVER - Alguien está enviando un mensaje privado...\n");
				GetMap();
				SendPrivateMessage();
			}
			else
			{
				printf("\nCliente %d: ", socket);
				printf("%s\n", buffer);
			}
		}
		else
			ManageRecvError(received);
	}

	EndConnection();
}


void Clithread::SendPrivateMessage()
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
			printf("ALGUIEN DIGALE A ESTE PENDEJO QUE ESE USUARIO NO EXISTE!\n");
		}
	}

	else
		ManageRecvError(received);
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
             << '\t' << '\t' << u.estado << '\n'; 
    } 
    cout << endl;

}


void Clithread::AddToMap(Usuario newUser)
{
	dict.insert(pair<int, Usuario>(cid, newUser));
}


int Clithread::GetUsernameId(string username)
{
    map<int, Usuario>::iterator itr;
	for (itr = dict.begin(); itr != dict.end(); ++itr)
	{
		// Se itera todos los id's y usuarios en el diccionario
		int userId = itr->first;
		Usuario u = itr->second;

		cout << "COMPARANDO " << u.user << " CON " << username << endl;

		// Se compara el nombre del usuario con el parámetro
		if (u.user.compare(username) == 0)
			return userId;
	}
	return -1;
}


int Clithread::RegisterUser()
{
	// TODO: Hacer el three way handshake

	string username = to_string(socket);
	string ip = to_string(cid);

	if (!UsernameExists(username) && !IpExists(ip))
	{
		Usuario newUser(username, ip, socket);
		AddToMap(newUser);
		return 1;
	}
	else
	{
		printf("IP O USERNAME YA EXISTEN\n");
		EndConnection();
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
