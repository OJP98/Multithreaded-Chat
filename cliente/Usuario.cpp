#include <iostream>
#include <string>
#include <time.h>

#include "Usuario.h"

using namespace std;

Usuario::Usuario(string userN, string ipN, int socketN)
{
	user = userN;
	ip = ipN;
	socket = socketN;
	estado = "ACTIVO";
}

Usuario::Usuario()
{
	user = "0";
	ip = "0";
	socket = 0;
	estado = "ACTIVO";
}

time_t Usuario::GetHoraUltimoMensaje()
{
	return horaUltimoMensaje;
}
void Usuario::SetHoraUltimoMensaje(time_t nuevaHora)
{
	horaUltimoMensaje=nuevaHora;

}

string Usuario::GetEstado()
{

	// if(estado=='ACTIVO')
	// 	return "Activo";
	// else if (estado=='OCUPADO')
	// 	return "Ocupado";
	// else
	// 	return "Inactivo";

}

string Usuario::Info()
{
	string info;
	info="Usuario: \t"+user+" \nIp: \t\t"+ip+"\nEstado: \t"+Usuario::GetEstado()+"\n";
	return info;

}

int Usuario::CambioEstado(int nuevoEstado)
{
	if (nuevoEstado>0 &&  nuevoEstado<=3)
	{
		estado=nuevoEstado;
		return 1;
	}
	return 0;	
}