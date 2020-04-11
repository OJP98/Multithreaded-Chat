#include <string>
#include <iostream>
#include "mensaje.pb.h"
using namespace std;
using namespace chat;

void Listen(string binary)
{
	// Se deserealiza el string a un objeto Mensaje
	ClientMessage m2;
	m2.ParseFromString(binary);


	if(m2.option()==1)
	{
		// option 1: synchronize
		string nombreUsuario= m2.synchronize().username();
		string ip=m2.synchronize().ip();

		cout << "Username: " <<nombreUsuario<< endl;
		cout << "ip: " <<ip<< endl;
	}
	else if(m2.option()==2)
	{
		// IF userId 0 RETURN ALL CONNECTED USERS

  		string nombreUsuario= m2.connectedusers().username();
		int userId=m2.connectedusers().userid();

		if(userId==0)
		{
			// option 2: connectedUsers
			cout << "Opcion 2"<< endl;
			cout << "Todos los usuarios"<< endl;
		}
		else
		{
			cout << "Opcion 2"<< endl;
			cout << "Info de "<<nombreUsuario<< endl;

		}


	}
	else if(m2.option()==3)
	{
		// option 3: changeStatus

		string nuevoEstado= m2.changestatus().status();

		cout << "nuevoEstado: " <<nuevoEstado<< endl;



	}
	else if(m2.option()==4)
	{
		// option 4: broadcast
		string mensaje= m2.broadcast().message();

		cout << "Mensaje: " <<mensaje<< endl;



	}
	else if(m2.option()==5)
	{
		// option 5: directMessage
		/*
		required string message = 1;
	  	optional int32 userId = 2;
	  	optional string username = 3;
		*/

		string mensaje= m2.directmessage().message();
		int userId=m2.directmessage().userid();
		string nombreUsuario= m2.directmessage().username();

		cout << "Mensaje: " <<mensaje<< endl;
		cout << "Id Usuario: " <<userId<< endl;
		cout << "Nombre Usuario: " <<nombreUsuario<< endl;
		
	}



}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	

/*
	MyInfoSynchronize * miInfo(new MyInfoSynchronize);

	miInfo->set_username("usuario1234");
	miInfo->set_ip("127.0.0.1");
	ClientMessage m;
	m.set_option(1);
	m.set_allocated_synchronize(miInfo);
*/

/*
	connectedUserRequest * miInfo(new connectedUserRequest);

	// IF userId 0 RETURN ALL CONNECTED USERS
	miInfo->set_userid(1);
	miInfo->set_username("127.0.0.1");

	ClientMessage m;
	m.set_option(2);
	m.set_allocated_connectedusers(miInfo);
*/


/*
	ChangeStatusRequest * miInfo(new ChangeStatusRequest);

	miInfo->set_status("usuario1234");
	ClientMessage m;
	m.set_option(3);
	m.set_allocated_changestatus(miInfo);

*/


/*
	BroadcastRequest * miInfo(new BroadcastRequest);

	miInfo->set_message("usuario1234");
	ClientMessage m;
	m.set_option(4);
	m.set_allocated_broadcast(miInfo);
*/


/*
	DirectMessageRequest * miInfo(new DirectMessageRequest);

	miInfo->set_message("usuario1234");
	miInfo->set_userid(5);
	miInfo->set_username("usuario1234");
	ClientMessage m;
	m.set_option(5);
	m.set_allocated_directmessage(miInfo);

*/

	// Se serializa el message a string
	string binary;
	m.SerializeToString(&binary);

	//Servidor
	Listen(binary);
	

	google::protobuf::ShutdownProtobufLibrary();
	return 1;
}