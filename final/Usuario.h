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
	string estado;
	int socket;
	string ip;
	time_t horaUltimoMensaje;
	bool reallyInactive;
	
};

#endif