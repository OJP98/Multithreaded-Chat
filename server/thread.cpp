#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <condition_variable>


#include "thread.h"

using namespace std;

Clithread::Clithread(int socketN, int tidN, struct sockaddr_in addrN)
{
	// struct connection_data *data;
	// data = (struct connection_data *) args;

	socket = socketN;
	tid = tidN;
	addr = addrN;
	len = sizeof(addr);

	closeConnection = false;
}

void Clithread::ConnectWithClient()
{
	strcpy(buffer, "SERVIDOR CONECTADO!\n");
	send(socket, buffer, len, 0);

	if (send(socket, buffer, len, 0) == 0)
	{
		fprintf(stderr, "ERROR al enviar respuesta del server al cliente.\n");
		EndConnection();
	}
	else
	{
		printf("SERVER - Conexión con nuevo cliente establecida.\n");
		ManageClient();
	}
}

void Clithread::ManageClient()
{
	while (socket > 0 && !closeConnection)
	{
		// Recibir mensajes del cliente constantemente
		int received = recv(socket, buffer, BUFSIZE, 0);

		if (received == -1)
		{
			// Mensajes
			printf("\nSERVER - ERROR, no se pudo recibir el mensaje del cliente\n");
			closeConnection = true;
		}

		else if (received == 0)
		{
			// Mensajes
			printf("\nSERVER - Conexión cerrada por parte del cliente: %d\n",socket);

			closeConnection = true;
		}

		else
		{
			printf("\nCliente %d: ", socket);
			printf("%s", buffer);
			if (*buffer == '#') {
				printf("\nSERVER: Me pierdes cliente...\nSERVER: F en el chat amigos\n");
				closeConnection = true;
			}
		}
	}

	EndConnection();
}

void Clithread::EndConnection()
{
	printf("SERVER - Terminando conexión con: %s, socket %d\n", inet_ntoa(addr.sin_addr), socket);
	close(socket);
}
