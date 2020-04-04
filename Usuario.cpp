#include <iostream>
#include <string>
#include <time.h> 

#include "Usuario.h"

using namespace std;

Usuario::Usuario(string nombreN, string ipN)
{
	nombre=nombreN;
	ip=ipN;
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

	if(estado==1)
		return "Activo";
	else if (estado==2)
		return "Ocupado";
	else
		return "Inactivo";

}

string Usuario::Info()
{
	string info;
	info="Usuario: \t"+nombre+" \nIp: \t\t"+ip+"\nEstado: \t"+Usuario::GetEstado()+"\n";
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