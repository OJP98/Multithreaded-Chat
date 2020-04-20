//g++ moveCursor.cpp -o move -lncurses
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "mensaje.pb.h"

#include <iostream>
#include <string>
#include <cstring>

#include <map>
#include <vector>

using namespace std;
using namespace chat;
using namespace google::protobuf;

#define DELAY 30000
#define BUFSIZE 1024
//#####################
//#### Clase Mensaje ##
//#####################

class Mensaje
{
public:
    Mensaje(string usuarioP,string mensajeP);
    string usuario;
    string mensaje; 
    int visto;
};
Mensaje::Mensaje(string usuarioP,string mensajeP)
{
    usuario=usuarioP;
    mensaje=mensajeP;
    visto=0;

}

//##########################
//#### Variables globales ##
//##########################

map<int,Mensaje*> mensajes;
map<string,vector<Mensaje*>> mensajesPrivados;
vector<string> usuariosConectadosLista;
int contador=0,n2 = 1;
int cantidadUsuarios=0;
int sockfd;
int userIdGlobal=0;

string nombreUsuarioBuscado;
string statusUsuarioBuscado;
int idUsuarioBuscado;
string ipUsuarioBuscado;


//########################
//#### Metodos globales ##
//########################

string getNombreChat(int id)
{
    map<string,vector<Mensaje*>>::iterator iter;
    int contador=0;

    vector<Mensaje*> chat;



    for (iter=mensajesPrivados.begin(); iter != mensajesPrivados.end(); iter++)
    {
        if(contador++==id)
        {
            return(iter->first);
        }
        
    }

}

void nuevoMensajePrivado(string bandejaUsuario)
{

    vector<Mensaje*> vect; 
    mensajesPrivados[bandejaUsuario]=vect;
}

int estadoDeChat(string nombreChat)
{
    
    vector<Mensaje*> chatActual;


    chatActual=mensajesPrivados[nombreChat];

    if(chatActual.empty())
    {
        return 1;
    }
    

    if(chatActual[chatActual.size()-1]->visto==0) return 0;
        else return 1;
    

}



//~~~~~~~~~~~~~~~~~~~~~~~~
//~~ Metodos ncurses ~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~
void makeLine(WINDOW *w,int x, int y)
{
    //Se inicializa el color
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    attron(COLOR_PAIR(1));

    wmove(w, y, 0); // se mueve el cursor
    whline(w,' ', x);//se crea la linea

    attroff(COLOR_PAIR(1));//se apaga el color
    wmove(w, y+1, 0); // se mueve el cursor
}

void HacerMenuPrincipal(int opcion)
{
    mvprintw(0, 0,"Menu Principal");

    if(opcion==1)
        {

            attron(COLOR_PAIR(1));
            mvprintw(2, 0,"Ver usuarios conectados");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(3, 0,"Cambiar estado");
            mvprintw(4, 0,"Chat Global");
            mvprintw(5, 0,"Chat privado");
            mvprintw(6, 0,"Ayuda");
            mvprintw(7, 0,"Salir");

        }
        else if(opcion==2)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            attron(COLOR_PAIR(1));
            mvprintw(3, 0,"Cambiar estado");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(4, 0,"Chat Global");
            mvprintw(5, 0,"Chat privado");
            mvprintw(6, 0,"Ayuda");
            mvprintw(7, 0,"Salir");
        }
        else if(opcion==3)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Cambiar estado");
            attron(COLOR_PAIR(1));
            mvprintw(4, 0,"Chat Global");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(5, 0,"Chat privado");
            mvprintw(6, 0,"Ayuda");
            mvprintw(7, 0,"Salir");
        }
        else if(opcion==4)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Cambiar estado");
            mvprintw(4, 0,"Chat Global");
            attron(COLOR_PAIR(1));
            mvprintw(5, 0,"Chat privado");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(6, 0,"Ayuda");
            mvprintw(7, 0,"Salir");
        }
        else if(opcion==5)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Cambiar estado");
            mvprintw(4, 0,"Chat Global");
            mvprintw(5, 0,"Chat privado");
            attron(COLOR_PAIR(1));
            mvprintw(6, 0,"Ayuda");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(7, 0,"Salir");
        }
        else if(opcion==6)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Cambiar estado");
            mvprintw(4, 0,"Chat Global");
            mvprintw(5, 0,"Chat privado");
            mvprintw(6, 0,"Ayuda");
            attron(COLOR_PAIR(1));
            mvprintw(7, 0,"Salir");
            attroff(COLOR_PAIR(1));//se apaga el color
        }
        

}

void printMensajes(int max,int posInicial)
{
    map<int, Mensaje*>::iterator iter;

    iter = mensajes.begin();
    int ajuste=(mensajes.size()-max);

    for (int i = 0; i < ajuste; ++i)
    {
        iter++;
    }
    
    int contadorLinea=0;
    for (iter; iter != mensajes.end(); iter++)
    {
        
        string nombreIter=iter->second->usuario;
        string mensajeIter=iter->second->mensaje;

        char nombreChar[nombreIter.size() + 1];
        strcpy(nombreChar, nombreIter.c_str());

        char mensajeChar[mensajeIter.size() + 1];
        strcpy(mensajeChar, mensajeIter.c_str());    

        mvprintw(posInicial+(contadorLinea++), 0,"%s \t: %s",nombreChar,mensajeChar);
        
    }

}

void printBandejaPrivada(int posInicial, int posBandejaPrivada)
{

    map<string,vector<Mensaje*>>::iterator iter;
    int contadorLinea=0;

    for (iter=mensajesPrivados.begin(); iter != mensajesPrivados.end(); iter++)
    {
        string nombreIter=iter->first;

        char nombreChar[nombreIter.size() + 1];
        strcpy(nombreChar, nombreIter.c_str());

        int estadoActual=estadoDeChat(nombreIter);
        
        if (posBandejaPrivada==contadorLinea)
        {
            attron(COLOR_PAIR(1));
            if(estadoActual) mvprintw(posInicial+(contadorLinea++), 0,"%s",nombreChar);
            else mvprintw(posInicial+(contadorLinea++), 0,"%s (*)",nombreChar);
            attroff(COLOR_PAIR(1));//se apaga el color
        }
        else
        {
            if(estadoActual) mvprintw(posInicial+(contadorLinea++), 0,"%s",nombreChar);
            else mvprintw(posInicial+(contadorLinea++), 0,"%s (*)",nombreChar);
            
        }
        

    }

}

void printChatPrivado(int id,int max,int posInicial)
{

    map<string,vector<Mensaje*>>::iterator iter;
    int contador=0;

    vector<Mensaje*> chat;



    for (iter=mensajesPrivados.begin(); iter != mensajesPrivados.end(); iter++)
    {
        if(contador++==id)
        {
            chat=mensajesPrivados.at(iter->first);
            break;
        }
        
    }

    if(!chat.empty())
    {
        chat[chat.size()-1]->visto=1;
        int ajuste=(chat.size()-max);
        if(ajuste<0) ajuste=0;
        contador=0;
        for (int i=(0+ajuste); i < chat.size(); i++) 
        {
            string nombreIter=chat[i]->usuario;
            string mensajeIter=chat[i]->mensaje;

            char nombreChar[nombreIter.size() + 1];
            strcpy(nombreChar, nombreIter.c_str());

            char mensajeChar[mensajeIter.size() + 1];
            strcpy(mensajeChar, mensajeIter.c_str());

            mvprintw(posInicial+(contador++), 0,"%s \t: %s",nombreChar,mensajeChar);
        }

    }

    

}

void printCambioDeEstado(int posCambiodeEstado)
{
    mvprintw(0, 0,"Cambio de Estado");

    if(posCambiodeEstado==0)
    {
        attron(COLOR_PAIR(1));
        mvprintw(2, 0,"Activo"); 
        attroff(COLOR_PAIR(1));
        mvprintw(3, 0,"Ocupado"); 
        mvprintw(4, 0,"Inactivo");
    }
    else if(posCambiodeEstado==1)
    {
        mvprintw(2, 0,"Activo"); 
        attron(COLOR_PAIR(1));
        mvprintw(3, 0,"Ocupado"); 
        attroff(COLOR_PAIR(1));
        mvprintw(4, 0,"Inactivo");
    }
    else if(posCambiodeEstado==2)
    {
        mvprintw(2, 0,"Activo"); 
        mvprintw(3, 0,"Ocupado"); 
        attron(COLOR_PAIR(1));
        mvprintw(4, 0,"Inactivo");
        attroff(COLOR_PAIR(1));
    }

      

}

void printAyuda()
{
    mvprintw(0, 0,"Ayuda");

    mvprintw(2, 0,  "Generales:");
    mvprintw(3, 0,  "Para navegar se utilizan las flechas del teclado, para seleccionar una opcion se utiliza la tecla Enter y regresar se utiliza la tecla ESC.");

    mvprintw(6, 0,  "Que encontraras en cada opcion del Menu:");
    
    mvprintw(8, 0,  "Ver usuarios conectados:");
    mvprintw(9, 0,  "Se visualiza la información de los usuarios que están registrados en el servidor, para ver información extra, seleccione un usuario.");

    mvprintw(12, 0,  "Cambiar estado:");
    mvprintw(13, 0,  "Esta opción permite poder cambiar el estado del usuario (Activo, Ocupado, Inactivo).");

    mvprintw(16, 0,  "Chat global:");
    mvprintw(17, 0,  "Chat con todos los usuarios conectados.");

    mvprintw(19, 0,  "Chat privado:");
    mvprintw(20, 0,  "Chat privado con un solo usuario.");

    mvprintw(22, 0,  "Hecho con    por Oscar Juarez, Jose Cifuentes y Luis Esturban.");
    start_color();
    init_pair(2, COLOR_WHITE, COLOR_RED);
    attron(COLOR_PAIR(2));
    mvprintw(22, 10,  "<3");
    attroff(COLOR_PAIR(2));
    mvprintw(23,0,  "");
    
}

void printUsuariosConectados(int posicionUsuariosConectados)
{

    for (int i = 0; i < usuariosConectadosLista.size(); ++i)
    {
        string nombreIter=usuariosConectadosLista[i];
        char nombreChar[nombreIter.size() + 1];
        strcpy(nombreChar, nombreIter.c_str());


        if(i==posicionUsuariosConectados)
        {
            attron(COLOR_PAIR(1));
            mvprintw(2+i, 0,"%s",nombreChar);
            attroff(COLOR_PAIR(1));
        }
        else
        {
            mvprintw(2+i, 0,"%s",nombreChar);
        }

    }



}

void actualizarBandejaPrivada()
{

    connectedUserRequest * usuariosConectados(new connectedUserRequest);

    // IF userId 0 RETURN ALL CONNECTED USERS
    usuariosConectados->set_userid(0);

    ClientMessage m;
    m.set_option(2);
    m.set_allocated_connectedusers(usuariosConectados);

    // Se serializa el message a string
    string binary3;
    m.SerializeToString(&binary3);

    char cstr3[binary3.size() + 1];
    strcpy(cstr3, binary3.c_str());

    send(sockfd, cstr3, strlen(cstr3), 0);

}
void getInfoDeUsuario(int posicionUsuariosConectados)
{
    connectedUserRequest * usuariosConectados(new connectedUserRequest);

    // IF userId 0 RETURN ALL CONNECTED USERS
    usuariosConectados->set_username(usuariosConectadosLista[posicionUsuariosConectados]);

    ClientMessage m;
    m.set_option(2);
    m.set_allocated_connectedusers(usuariosConectados);

    // Se serializa el message a string
    string binary3;
    m.SerializeToString(&binary3);

    char cstr3[binary3.size() + 1];
    strcpy(cstr3, binary3.c_str());

    send(sockfd, cstr3, strlen(cstr3), 0);

}
void printInfoDeUsuario()
{

    string nombreIter=nombreUsuarioBuscado;
    char nombreChar[nombreIter.size() + 1];
    strcpy(nombreChar, nombreIter.c_str());
    mvprintw(2, 0,"Nombre de usuario\t: %s",nombreChar);

    string statusIter=statusUsuarioBuscado;
    char statusChar[statusIter.size() + 1];
    strcpy(statusChar, statusIter.c_str());
    mvprintw(3, 0,"Status de usuario\t: %s",statusChar);

    mvprintw(4, 0,"Id de Usuario\t\t: %d",idUsuarioBuscado);

    string ipIter=ipUsuarioBuscado;
    char ipChar[ipIter.size() + 1];
    strcpy(ipChar, ipIter.c_str());
    mvprintw(5, 0,"Status de usuario\t: %s",ipChar);
            
    refresh();
        


}

//~~~~~~~~~~~~~~~~~~~~~~~~
//~~ Metodos Otros ~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~
void mensajeglobal(string msj,int sockfd){

	//sleep(5);
    BroadcastRequest * messageRequest(new BroadcastRequest);
	messageRequest->set_message(msj);

	ClientMessage m5;
	m5.set_option(4);
	m5.set_allocated_broadcast(messageRequest);

	string binary4;
    m5.SerializeToString(&binary4);
    char cstr4[binary4.size() + 1];
    strcpy(cstr4, binary4.c_str());
    send(sockfd, cstr4, strlen(cstr4), 0);
    
}

void mensajeglobalStatus(string msj,int sockfd){

	//sleep(5);
    BroadcastRequest * messageRequest(new BroadcastRequest);
	messageRequest->set_message(msj);

	ClientMessage m5;
	m5.set_option(4);
	m5.set_allocated_broadcast(messageRequest);

	string binary4;
    m5.SerializeToString(&binary4);
    char cstr4[binary4.size() + 1];
    strcpy(cstr4, binary4.c_str());
    send(sockfd, cstr4, strlen(cstr4), 0);
    
}

void mensajeprivado(string msj, string usrName ,int sockfd){

	//sleep(5);
    DirectMessageRequest * dm(new DirectMessageRequest);

	dm->set_message(msj);
	dm->set_username(usrName);
	ClientMessage m7;
	m7.set_option(5);
	m7.set_allocated_directmessage(dm);

	string binary7;
    m7.SerializeToString(&binary7);
    char cstr7[binary7.size() + 1];
    strcpy(cstr7, binary7.c_str());
    send(sockfd, cstr7, strlen(cstr7), 0);
    
	
}



void mensajeprivadoStatus(string msj, string usrName ,int sockfd){

	//sleep(5);
    DirectMessageRequest * dm(new DirectMessageRequest);

	dm->set_message(msj);
	dm->set_username(usrName);
	ClientMessage m7;
	m7.set_option(5);
	m7.set_allocated_directmessage(dm);

	string binary7;
    m7.SerializeToString(&binary7);
    char cstr7[binary7.size() + 1];
    strcpy(cstr7, binary7.c_str());
    send(sockfd, cstr7, strlen(cstr7), 0);
    char buffer7[BUFSIZE];
    bzero(buffer7, BUFSIZE);
    recv(sockfd, buffer7, BUFSIZE, 0);
    string ret(buffer7, BUFSIZE);
    ServerMessage sm7;
    sm7.ParseFromString(buffer7);
    int option7 = sm7.option();
    if (option7 == 8)
    {
    	string mensaje= sm7.directmessageresponse().messagestatus();
    }else if (option7 == 3)
    {
        string errorMensaje= sm7.error().errormessage();
        cout << "El error es: " <<errorMensaje<< endl;
    }
	
}

//~~~~~~~~~~~~~~~~~~~~~~~~
//~~ Metodos mensaje ~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~
void nuevoMensaje(string usuario,string mensaje,int sockfd)
{
    
    mensajes[contador++]=new Mensaje(usuario,mensaje);
}

void enviarNuevoMensaje(string usuario,string mensaje,int sockfd)
{
    mensajeglobal(mensaje,sockfd);
    nuevoMensaje(usuario,mensaje,sockfd);
}

void nuevoMensajePrivado(string bandejaUsuario, string usuario,string mensaje)
{
    //mensajeprivado(mensaje,bandejaUsuario,sockfd);
    if(mensajesPrivados.find(bandejaUsuario)==mensajesPrivados.end())
    {
        vector<Mensaje*> vect; 
        vect.push_back(new Mensaje(usuario,mensaje));
        mensajesPrivados[bandejaUsuario]=vect;
    }
    else
    {
           
        mensajesPrivados.at(bandejaUsuario).push_back(new Mensaje(usuario,mensaje));
    }
 
}

void enviarMensajePrivado(string bandejaUsuario, string usuario,string mensaje)
{
    mensajeprivado(mensaje,bandejaUsuario,sockfd);
    if(bandejaUsuario!=usuario)
        nuevoMensajePrivado(bandejaUsuario, usuario,mensaje);
}

void cambiarEstado(int estado,int sockfd)
{
    string posiblesEstados[]={"ACTIVO","OCUPADO","INACTIVO"};
    
    ChangeStatusRequest * statusRequest(new ChangeStatusRequest);
    statusRequest->set_status(posiblesEstados[estado]);

    ClientMessage m3;
    m3.set_option(3);
    m3.set_allocated_changestatus(statusRequest);
    // Se serializa el message a string
    string binary3;
    m3.SerializeToString(&binary3);

    char cstr3[binary3.size() + 1];
    strcpy(cstr3, binary3.c_str());

    send(sockfd, cstr3, strlen(cstr3), 0);

    

    char buffer2[BUFSIZE];
    // Esperar respuesta del servidor
    bzero(buffer2, BUFSIZE);
    recv(sockfd, buffer2, BUFSIZE, 0);

    string ret(buffer2, BUFSIZE);
    ServerMessage sm2;
    sm2.ParseFromString(buffer2);
    int option2 = sm2.option();

    // MANEJAR MY INFO RESPONSE
    if (option2 == 6)
    {
        string userStatus = sm2.changestatusresponse().status();
        mvprintw(6, 0,"Estado Actualizado");
        refresh();
        sleep(1);
    }else if (option2 == 3)
    {
        string errorMensaje= sm2.error().errormessage();
        cout << "El error es: " <<errorMensaje<< endl;
    }

}

void *escucha(void *arg){
    while(n2 != 0){

        //sleep(5);

        char buffer2[BUFSIZE];
        // Esperar respuesta del servidor
        bzero(buffer2, BUFSIZE);
        recv(sockfd, buffer2, BUFSIZE, 0);

        string ret(buffer2, BUFSIZE);
        ServerMessage sm2;
        sm2.ParseFromString(buffer2);
        int option2 = sm2.option();



    

        //broadcast
        if(option2 == 1)
        {
            mvprintw(6, 0,"Entra");
            refresh();
            sleep(10);

            if(!(sm2.broadcast().userid()==userIdGlobal))
            {
                //nuevoMensaje(string usuario,string mensaje,int sockfd)
                nuevoMensaje("Prueba",sm2.broadcast().message(),sockfd);
            }
            

        }
        //Mensaje privado
        
        else if (option2 == 2)
        {
            string mensaje= sm2.message().message();
            int userId=sm2.message().userid();
            string nombreUsuario= sm2.message().username();
            nuevoMensajePrivado(nombreUsuario, nombreUsuario,mensaje);

        }



        //Manejo de errores
        else if (option2 == 3)
        {
            string errorMensaje= sm2.error().errormessage();
            cout << "El error es: " <<errorMensaje<< endl;
        }
        // MANEJAR MY INFO RESPONSE
        else if (option2 == 5)
        {
            cantidadUsuarios=sm2.connecteduserresponse().connectedusers().size();

            if(cantidadUsuarios==1)
            {
                nombreUsuarioBuscado=sm2.connecteduserresponse().connectedusers(0).username();
                statusUsuarioBuscado=sm2.connecteduserresponse().connectedusers(0).status();
                idUsuarioBuscado=sm2.connecteduserresponse().connectedusers(0).userid();
                ipUsuarioBuscado=sm2.connecteduserresponse().connectedusers(0).ip();

            }


            for(int i=0;i<cantidadUsuarios;i++)
            {
                string nombreIter=sm2.connecteduserresponse().connectedusers(i).username();
                
                if(mensajesPrivados.find(nombreIter)==mensajesPrivados.end())
                {
                    nuevoMensajePrivado(nombreIter);
                    usuariosConectadosLista.push_back(nombreIter);
                }

                
                

            }
            
        }



    }
    pthread_exit(NULL);
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {

    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    bool salir = false;
    char buffer[BUFSIZE];

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR al abrir socket");

    
    // Obtener host desde consola
    server = gethostbyname(argv[2]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no existe la dirección del host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server -> h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR, no se pudo conectar al servidor");

    /* THREE-WAY-HANDSHAKE */





    //          1. SOLICITAR SINCRONIZACION DE INFO

    // Se crea instacia tipo MyInfoSynchronize y se setean los valores deseables
    MyInfoSynchronize * miInfo(new MyInfoSynchronize);
    miInfo->set_username(argv[1]);
    miInfo->set_ip(argv[4]);

    // Se crea instancia de Mensaje, se setea los valores deseados
    ClientMessage m;
    m.set_option(1);
    m.set_allocated_synchronize(miInfo);

    // Se serializa el message a string
    string binary;
    m.SerializeToString(&binary);

    char cstr[binary.size() + 1];
    strcpy(cstr, binary.c_str());

    send(sockfd, cstr, strlen(cstr), 0);

    printf("Esperando confirmación del servidor...\n");

    //          2. ESPERAR RESPUESTA DE SINCRONIZACIÓN

    // Recibir mensajes del cliente conectado
    bzero(buffer, BUFSIZE);
    recv(sockfd, buffer, BUFSIZE, 0);

    if (1)
    {

        string ret(buffer, BUFSIZE);
    }
    ServerMessage sm;
    sm.ParseFromString(buffer);
    int option = sm.option();

    // MANEJAR MY INFO RESPONSE
    if (option == 4)
    {
        userIdGlobal = sm.myinforesponse().userid();
        cout << "MY ID ES: " << userIdGlobal << endl;


        //          3. HACER SABER AL SERVIDOR QUE ESTAMOS LISTOS

        // Se crea instancia tipo MyInfoAcknowledge
        MyInfoAcknowledge * ack(new MyInfoAcknowledge);
        ack->set_userid(userIdGlobal);

        // Se crea instancia de Mensaje, se setea los valores deseados
        ClientMessage m2;
        m2.set_option(6);
        m2.set_allocated_acknowledge(ack);

        // Se serializa el message a string
        string binary2;
        m2.SerializeToString(&binary2);

        char cstr2[binary2.size() + 1];
        strcpy(cstr2, binary2.c_str());

        send(sockfd, cstr2, strlen(cstr2), 0);

    }
    
//////////////////////////////////////////////////////////////////////////////////////////////

/*
    mensajeglobal("hola",sockfd);
    mensajeglobalStatus("hola",sockfd);
    mensajeprivado("hola","usuarioPrueba",sockfd);
    mensajeprivadoStatus("hola","usuarioPrueba",sockfd);
    */


//~~~~~~~~~~~~~~~~~~~~~~~~
//~~ Cosas Temporales ~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~
    /*
    nuevoMensajePrivado("Jose","Julian","Que onda");
    nuevoMensajePrivado("Julian","Julian","Que onda");
    nuevoMensajePrivado("Julian","Jose","Como vas?");
    nuevoMensajePrivado("Gustavo");
    */
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    pthread_t hilo1;
    pthread_create(&hilo1,NULL,escucha,NULL);
    char buf[100] = {0}, *s = buf;

    int ch, cnt = 0;

    int numPantalla=1;

    int opcionMenuPrincipal=1;//Opcion de menuPrincipal

    int posBandejaPrivada=0; //opcion de bandeja privada

    int posCambioDeEstado=0;

    int posUsuariosConectados=0;

    WINDOW *w;
 
    if ((w = initscr()) == NULL) {
        fprintf(stderr, "Error: initscr()\n");
        exit(EXIT_FAILURE);
    }

    //habilita la utilizacion de teclas especiales
    keypad(stdscr, TRUE); 

    //deshabilita el echo automatico
    noecho();


    cbreak();      // disable line-buffering


    timeout(100);  // wait 100 milliseconds for input


    while (n2 != 0) {
        erase();

        int x,y;
        getmaxyx(stdscr,y,x);


        if(numPantalla==1)
        {
            //menuPrincipal

            start_color();
            init_pair(1, COLOR_BLACK, COLOR_GREEN);

            HacerMenuPrincipal(opcionMenuPrincipal);        
           
            mvprintw(9, 0,"%d",opcionMenuPrincipal);

            
            refresh();
            if ((ch = getch()) != ERR) 
            {
                if (ch == 27) {
                    //si presiona [ESC]
                    n2=0;
                }
                else if(ch==KEY_UP)
                {
                    if(opcionMenuPrincipal--<=1) opcionMenuPrincipal=6;

                }
                else if(ch==KEY_DOWN)
                {
                    if(opcionMenuPrincipal++>=6) opcionMenuPrincipal=1;
                    
                }
                else if (ch=='\n') 
                {
                    if(opcionMenuPrincipal==1)
                    {
                        opcionMenuPrincipal=1;
                        numPantalla=9;
                        actualizarBandejaPrivada();
                        
                    }
                    else if(opcionMenuPrincipal==2)
                    {
                        opcionMenuPrincipal=1;
                        numPantalla=7;
                    }
                    else if(opcionMenuPrincipal==3)
                    {
                        opcionMenuPrincipal=1;
                        numPantalla=2;
                    }
                    else if(opcionMenuPrincipal==4)
                    {
                        opcionMenuPrincipal=1;
                        numPantalla=5;
                        actualizarBandejaPrivada();
                    }
                    else if(opcionMenuPrincipal==5)
                    {
                        opcionMenuPrincipal=1;
                        numPantalla=8;
                    }
                    else if(opcionMenuPrincipal==6) n2=0;
                }

            }



        }
        else if(numPantalla==2)
        {
            mvprintw(0, 0,"CHAT GENERAL 2020");

            //int max,int posInicial
            //printMensajes(4,2);
            printMensajes(y-7,2);
            

            makeLine(w,x,y-5);
            mvprintw(y-4, 0, "> %s", buf);

            refresh();
             
            // getch (with cbreak and timeout as above)
            // waits 100ms and returns ERR if it doesn't read anything.
            if ((ch = getch()) != ERR) {
                if (ch == '\n') {
                    //esto se ejecuta al presionar enter
                    if(strlen(buf)>0)
                        enviarNuevoMensaje(argv[1],string(buf),sockfd);

                    *s = 0;
                    sscanf(buf, "%d", &n2);
                    s = buf;
                    *s = 0;
                }
                else if (ch == 27) {
                    //si presiona [ESC]
                    numPantalla=1;
                    *s = 0;
                    sscanf(buf, "%d", &n2);
                    s = buf;
                    *s = 0;
                }
                else if (ch == KEY_BACKSPACE) {
                    if (s > buf)
                        *--s = 0;
                }
                else if (s - buf < (long)sizeof buf - 1) {
                    *s++ = ch;
                    *s = 0;
                }
            }

        }
        else if(numPantalla==5)
        {
            //Bandeja de entrada de mensajes privados
            mvprintw(0, 0,"Bandeja de mensajes privados");
            printBandejaPrivada(2,posBandejaPrivada);
            mvprintw(9, 0,"%d",posBandejaPrivada);
            refresh();

            if ((ch = getch()) != ERR) 
            {
                if (ch == 27) {
                    //si presiona [ESC]
                    numPantalla=1;
                }
                else if(ch==KEY_UP)
                {
                    if(posBandejaPrivada--<=0) posBandejaPrivada=mensajesPrivados.size()-1;

                }
                else if(ch==KEY_DOWN)
                {
                    if(posBandejaPrivada++>=(mensajesPrivados.size()-1)) posBandejaPrivada=0;
                    
                }
                else if (ch=='\n') 
                {
                    numPantalla=6;
                   
                }

            }
        }
        else if(numPantalla==6)
        {
            //Chat Privado
            string nombreChat=getNombreChat(posBandejaPrivada);
            char nombreChatChar[nombreChat.size() + 1];
            strcpy(nombreChatChar, nombreChat.c_str());
            mvprintw(0, 0,"Chat Privado %s",nombreChatChar);          

            makeLine(w,x,y-5);
            //(int id,int max,int posInicial)
            printChatPrivado(posBandejaPrivada,y-7,2);
            mvprintw(y-4, 0, "> %s", buf);
            refresh();



            if ((ch = getch()) != ERR) {
                if (ch == '\n') {
                    //esto se ejecuta al presionar enter
                    if(strlen(buf)>0)
                        enviarMensajePrivado(getNombreChat(posBandejaPrivada),argv[1],string(buf));

                                        

                    *s = 0;
                    sscanf(buf, "%d", &n2);
                    s = buf;
                    *s = 0;
                }
                else if (ch == 27) {
                    //si presiona [ESC]
                    numPantalla=5;
                    *s = 0;
                    sscanf(buf, "%d", &n2);
                    s = buf;
                    *s = 0;
                }
                else if (ch == KEY_BACKSPACE) {
                    if (s > buf)
                        *--s = 0;
                }
                else if (s - buf < (long)sizeof buf - 1) {
                    *s++ = ch;
                    *s = 0;
                }
            }

        }
        else if(numPantalla==7)
        {
            //Cambio de estado
            //int posInicial, int posBandejaPrivada
            printCambioDeEstado(posCambioDeEstado);
            
            refresh();

            if ((ch = getch()) != ERR) {
                if (ch == '\n') {
                    //esto se ejecuta al presionar enter   
                    cambiarEstado(posCambioDeEstado,sockfd);
                    numPantalla=1;
                    posCambioDeEstado=0;

                    *s = 0;
                    sscanf(buf, "%d", &n2);
                    s = buf;
                    *s = 0;
                }
                else if (ch == 27) {
                    //si presiona [ESC]
                    numPantalla=1;
                    *s = 0;
                    sscanf(buf, "%d", &n2);
                    s = buf;
                    *s = 0;
                }
                else if(ch==KEY_UP)
                {
                    if(posCambioDeEstado--<=0) posCambioDeEstado=2;

                }
                else if(ch==KEY_DOWN)
                {
                    if(posCambioDeEstado++>=2) posCambioDeEstado=0;
                    
                }
                else if (ch == KEY_BACKSPACE) {
                    if (s > buf)
                        *--s = 0;
                }
                else if (s - buf < (long)sizeof buf - 1) {
                    *s++ = ch;
                    *s = 0;
                }
            }

        }
        else if(numPantalla==8)
        {
            printAyuda();
            refresh();
            //ayuda
            if ((ch = getch()) != ERR) 
            {
                if (ch == 27) {
                    //si presiona [ESC]
                    numPantalla=1;
                    *s = 0;
                    sscanf(buf, "%d", &n2);
                    s = buf;
                    *s = 0;
                }
            }
        

        }
        else if(numPantalla==9)
        {
            //Usuarios conectados
            mvprintw(0, 0,"Usuarios Conectados: %d",usuariosConectadosLista.size());
            printUsuariosConectados(posUsuariosConectados);
            mvprintw(9, 0,"%d",posUsuariosConectados);
            refresh();
            if ((ch = getch()) != ERR) 
            {
                if (ch == 27) {
                    //si presiona [ESC]
                    numPantalla=1;
                    posUsuariosConectados=0;
                }
                else if(ch==KEY_UP)
                {
                    if(posUsuariosConectados--<=0) posUsuariosConectados=cantidadUsuarios-1;

                }
                else if(ch==KEY_DOWN)
                {
                    if(posUsuariosConectados++>=(cantidadUsuarios-1)) posUsuariosConectados=0;
                    
                }
                else if (ch=='\n') 
                {
                    numPantalla=10;
                    getInfoDeUsuario(posUsuariosConectados);
                    
                }

            }
        }
        else if(numPantalla==10)
        {
            mvprintw(0, 0,"Informacion personal");
            printInfoDeUsuario();
            refresh();

            if ((ch = getch()) != ERR) 
            {
                if (ch == 27) {
                    //si presiona [ESC]
                    numPantalla=1;
                    posUsuariosConectados=0;
                }


            }


        }

        
        usleep(DELAY);
    }
 
    delwin(w);
    endwin();
    pthread_cancel(hilo1);
    //pthread_join(hilo1,NULL);
    return 0;
}