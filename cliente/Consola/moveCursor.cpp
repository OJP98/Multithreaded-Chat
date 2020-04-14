//g++ moveCursor.cpp -o move -lncurses
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>


#include <iostream>
#include <string>
#include <cstring>

#include <map>

using namespace std;

#define DELAY 30000
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


int main() {
    char buf[100] = {0}, *s = buf;

    int ch, cnt = 0, n = 1;

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

 
    while (n != 0) {
        erase();

        int x,y;
        getmaxyx(stdscr,y,x);

        /*
        mvprintw(0, 0,"Escriba 0 para cerrar!");
        mvprintw(1, 0, "Contador: %d", cnt++);
        mvprintw(3, 0, "Ultimo Numero: %d", n);
        */

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
                nuevoMensaje("TU TATA",string(buf));

                *s = 0;
                sscanf(buf, "%d", &n);
                s = buf;
                *s = 0;
            }
            else if (ch == 27) {
                //si presiona [ESC]
                n=0;
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
        usleep(DELAY);
    }
 
    delwin(w);
    endwin();
    return 0;
}