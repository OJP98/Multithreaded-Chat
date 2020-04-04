#ifndef USUARIO_H
#define USUARIO_H


#include <iostream>
#include <string>
#include <time.h> 

using namespace std;

class Usuario
{
public:
	Usuario(string nombreN, string ipN);
	string GetEstado();
	string Info();
	int CambioEstado(int nuevoEstado);
	time_t GetHoraUltimoMensaje();
	void SetHoraUltimoMensaje(time_t nuevaHora);
		

private:
	string nombre;
	string ip;
	int estado;
	time_t horaUltimoMensaje;
	
};

#endif