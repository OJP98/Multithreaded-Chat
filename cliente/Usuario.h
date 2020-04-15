#ifndef USUARIO_H
#define USUARIO_H

#include <iostream>
#include <string>
#include <time.h> 

using namespace std;

class Usuario
{
public:
	Usuario(string nombreN, string ipN, int socketN);
	Usuario();
	string GetEstado();
	string Info();
	int CambioEstado(int nuevoEstado);
	time_t GetHoraUltimoMensaje();
	void SetHoraUltimoMensaje(time_t nuevaHora);
	string user;
	int estado;
	int socket;
	string ip;
		

private:
	time_t horaUltimoMensaje;
	
};

#endif