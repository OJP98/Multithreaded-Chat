#include <string>
#include <iostream>
#include "mensaje.pb.h"
using namespace std;
using namespace chat;

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// Se crea instacia tipo MyInfoSynchronize y se setean los valores deseables
	MyInfoSynchronize * miInfo(new MyInfoSynchronize);
	miInfo->set_username("usuario1234");
	miInfo->set_ip("127.0.0.1");

	// Se crea instancia de Mensaje, se setea los valores deseados
	ClientMessage m;
	m.set_option(1);
	m.set_allocated_synchronize(miInfo);

	// Se serializa el message a string
	string binary;
	m.SerializeToString(&binary);

	/* SE DESEMPAQUETA Y SE LEE */

	// A partir de aquí abajo, ya es como lo que tendría que hacer el servidor.

	// Se deserealiza el string a un objeto Mensaje
	ClientMessage m2;
	m2.ParseFromString(binary);

	// Se puede accesar a los valores de la siguiente manera:
	cout << "Option: " << m2.option() << endl;
	cout << "Username: " << m2.synchronize().username() << endl;
	cout << "ip: " << m2.synchronize().ip() << endl;

	google::protobuf::ShutdownProtobufLibrary();
	return 1;
}