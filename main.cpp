#include <iostream>
#include <string>
#include <time.h> 
#include <vector> 

#include "Usuario.h"

using namespace std;

int main(int argc, char const *argv[])
{
	vector<Usuario> g1;
	cout<<g1.size()<<endl;
	Usuario nuevoUsuario("asd","asds");
	

	
	g1.push_back(nuevoUsuario);
	cout<<g1.size()<<endl;

	
	cout<<g1[0].Info()<<endl;
	
	return 0;
}