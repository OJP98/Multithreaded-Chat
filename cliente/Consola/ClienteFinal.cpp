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
};
Mensaje::Mensaje(string usuarioP,string mensajeP)
{
    usuario=usuarioP;
    mensaje=mensajeP;

}

//##########################
//#### Variables globales ##
//##########################

map<int,Mensaje*> mensajes;
int contador=0;

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
        /*
        //--------
        string a = "Hello World!";
        char cstr[a.size() + 1];
        strcpy(cstr, a.c_str());    
        mvprintw(2, 0, "String: %s", cstr);
        //-------
        */

        string nombreIter=iter->second->usuario;
        string mensajeIter=iter->second->mensaje;

        char nombreChar[nombreIter.size() + 1];
        strcpy(nombreChar, nombreIter.c_str());

        char mensajeChar[mensajeIter.size() + 1];
        strcpy(mensajeChar, mensajeIter.c_str());    

        mvprintw(posInicial+(contadorLinea++), 0,"%s \t: %s",nombreChar,mensajeChar);
        
    }

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

void *spammer(void *arg){
	int i=1;
	while(i<100000){
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////


	pthread_t hilo1;
	pthread_create(&hilo1,NULL,spammer,NULL);
    char buf[100] = {0}, *s = buf;

    int ch, cnt = 0, n2 = 1;

    int numPantalla=1;

    int opcionMenuPrincipal=1;//Opcion de menuPrincipal

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
                    else if(opcionMenuPrincipal==7) n=0;
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
                    nuevoMensaje(argv[1],string(buf));

                    *s = 0;
                    sscanf(buf, "%d", &n2);
                    s = buf;
                    *s = 0;
                }
                else if (ch == 27) {
                    //si presiona [ESC]
                    numPantalla=1;
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