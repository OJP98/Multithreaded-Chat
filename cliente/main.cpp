#include <iostream>
#include <string>
#include <time.h> 
#include <vector> 
#include "mensaje.pb.h"

#include "Usuario.h"

using namespace std;

int main(int argc, char const *argv[])
{
	vector<Usuario> g1;
	Usuario nuevoUsuario("TheJP","100.102.120");

	cout<<"Cambio de estado "<<nuevoUsuario.CambioEstado(5)<<endl;

	time_t tiempo1;
	time(&tiempo1);
	nuevoUsuario.SetHoraUltimoMensaje(tiempo1);
	
	cout<<"Hora "<<nuevoUsuario.GetHoraUltimoMensaje()<<endl;
	
	g1.push_back(nuevoUsuario);
	
	cout<<g1[0].Info()<<endl;
	
	return 0;
}