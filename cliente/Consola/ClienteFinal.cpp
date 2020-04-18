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

#include <iostream>
#include <string>
#include <cstring>

#include <map>
#include <vector>

using namespace std;

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
int contador=0,n2 = 1;


//########################
//#### Metodos globales ##
//########################

//~~~~~~~~~~~~~~~~~~~~~~~~
//~~ Metodos mensaje ~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~
void nuevoMensaje(string usuario,string mensaje)
{
    mensajes[contador++]=new Mensaje(usuario,mensaje);
}

void nuevoMensajePrivado(string bandejaUsuario, string usuario,string mensaje)
{
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
            mvprintw(3, 0,"Ver info de usuario");
            mvprintw(4, 0,"Cambiar estado");
            mvprintw(5, 0,"Chat Global");
            mvprintw(6, 0,"Chat privado");
            mvprintw(7, 0,"Ayuda");
            mvprintw(8, 0,"Salir");

        }
        else if(opcion==2)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            attron(COLOR_PAIR(1));
            mvprintw(3, 0,"Ver info de usuario");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(4, 0,"Cambiar estado");
            mvprintw(5, 0,"Chat Global");
            mvprintw(6, 0,"Chat privado");
            mvprintw(7, 0,"Ayuda");
            mvprintw(8, 0,"Salir");
        }
        else if(opcion==3)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Ver info de usuario");
            attron(COLOR_PAIR(1));
            mvprintw(4, 0,"Cambiar estado");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(5, 0,"Chat Global");
            mvprintw(6, 0,"Chat privado");
            mvprintw(7, 0,"Ayuda");
            mvprintw(8, 0,"Salir");
        }
        else if(opcion==4)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Ver info de usuario");
            mvprintw(4, 0,"Cambiar estado");
            attron(COLOR_PAIR(1));
            mvprintw(5, 0,"Chat Global");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(6, 0,"Chat privado");
            mvprintw(7, 0,"Ayuda");
            mvprintw(8, 0,"Salir");
        }
        else if(opcion==5)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Ver info de usuario");
            mvprintw(4, 0,"Cambiar estado");
            mvprintw(5, 0,"Chat Global");
            attron(COLOR_PAIR(1));
            mvprintw(6, 0,"Chat privado");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(7, 0,"Ayuda");
            mvprintw(8, 0,"Salir");
        }
        else if(opcion==6)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Ver info de usuario");
            mvprintw(4, 0,"Cambiar estado");
            mvprintw(5, 0,"Chat Global");
            mvprintw(6, 0,"Chat privado");
            attron(COLOR_PAIR(1));
            mvprintw(7, 0,"Ayuda");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(8, 0,"Salir");
        }
        else if(opcion==7)
        {
            mvprintw(2, 0,"Ver usuarios conectados");
            mvprintw(3, 0,"Ver info de usuario");
            mvprintw(4, 0,"Cambiar estado");
            mvprintw(5, 0,"Chat Global");
            mvprintw(6, 0,"Chat privado");
            mvprintw(7, 0,"Ayuda");
            attron(COLOR_PAIR(1));
            mvprintw(8, 0,"Salir");
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

//~~~~~~~~~~~~~~~~~~~~~~~~
//~~ Metodos Otros ~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~
void *spammer(void *arg){
	int i=1;
	while(n2 != 0){
		nuevoMensaje("TU NANA","Hola Mundo");
		sleep(5);
		i++;
	}
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]) {
	/////////////////////////////////////////////////////////////////
    /*
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	bool salir = false;
	char buffer[BUFSIZE];

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

	printf("Esperando confirmación del servidor...\n");
	recv(sockfd, buffer, BUFSIZE, 0);
	printf("Conexión aceptada!\n\n");
    */
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//~~~~~~~~~~~~~~~~~~~~~~~~
//~~ Cosas Temporales ~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~
    nuevoMensajePrivado("Jose","Julian","Que onda");
    nuevoMensajePrivado("Julian","Julian","Que onda");
    nuevoMensajePrivado("Julian","Jose","Como vas?");
    nuevoMensajePrivado("Gustavo");
/////////////////////////////////////////////////////////////////////////////////////////////////////////
	pthread_t hilo1;
	pthread_create(&hilo1,NULL,spammer,NULL);
    char buf[100] = {0}, *s = buf;

    int ch, cnt = 0;

    int numPantalla=1;

    int opcionMenuPrincipal=1;//Opcion de menuPrincipal

    int posBandejaPrivada=0; //opcion de bandeja privada

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
           
            //mvprintw(9, 0,"%d",opcionMenuPrincipal);

            
            refresh();
            if ((ch = getch()) != ERR) 
            {
                if (ch == 27) {
                    //si presiona [ESC]
                    n2=0;
                }
                else if(ch==KEY_UP)
                {
                    if(opcionMenuPrincipal--<=1) opcionMenuPrincipal=7;

                }
                else if(ch==KEY_DOWN)
                {
                    if(opcionMenuPrincipal++>=7) opcionMenuPrincipal=1;
                    
                }
                else if (ch=='\n') 
                {
                    if(opcionMenuPrincipal==4)
                    {
                        opcionMenuPrincipal=1;
                        numPantalla=2;
                    }
                    else if(opcionMenuPrincipal==5)
                    {
                        opcionMenuPrincipal=1;
                        numPantalla=5;
                    }
                    else if(opcionMenuPrincipal==7) n2=0;
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
                        nuevoMensaje(argv[1],string(buf));

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
                        nuevoMensajePrivado(getNombreChat(posBandejaPrivada),"TU TATA",string(buf));
                                        

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

        
        usleep(DELAY);
    }
 
    delwin(w);
    endwin();
    pthread_join(hilo1,NULL);
    return 0;
}