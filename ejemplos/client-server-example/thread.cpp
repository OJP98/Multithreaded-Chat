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

using namespace std;

Clithread::Clithread(
	int socketN,
	int tidN,
	struct sockaddr_in addrN,
	int clientCountN
	)
{
	socket = socketN;
	tid = tidN;
	addr = addrN;
	len = sizeof(addr);

	clientCount=&clientCountN;

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
		AddToMap();
		ManageClient();
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
		// value = m.at(key)
		if (dict.find(atoi(buffer)) != dict.end()) {
			// Buscar en el diccionario por socket relacionado a ip del cliente
			int cli_socket = dict[atoi(buffer)];
			bzero(buffer, BUFSIZE);
			strcpy(buffer, "MENSAJE PRIVADO DE PARTE DE ALGUIEN...\n");
			send(cli_socket, buffer, BUFSIZE, 0);
		}
	}

	else
		ManageRecvError(received);
}


void Clithread::EndConnection()
{
	printf("SERVER - Terminando conexión con: %s, socket %d\n", inet_ntoa(addr.sin_addr), socket);
	// Eliminar usuario del map
	dict.erase(tid);
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


void Clithread::GetMap()
{
	printf("EL MAP AL CONECTAR CON CLIENTE ES:\n");
	map<int, int>::iterator itr; 
	cout << "\tTID\tSOCKET\n"; 
    for (itr = dict.begin(); itr != dict.end(); ++itr) { 
        cout << '\t' << itr->first 
             << '\t' << itr->second << '\n'; 
    } 
    cout << endl;

}


void Clithread::AddToMap()
{
	dict.insert(pair<int, int>(tid, socket));
}