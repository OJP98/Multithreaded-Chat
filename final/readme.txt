########################################################

ANTES DE INICIAR: Instalar ncurses y google protobuf.

Instalación de ncurses Ubuntu:
sudo apt-get install libncurses5-dev libncursesw5-dev

Instalación de de ncurses en otros sabores populares de linux:
https://www.cyberciti.biz/faq/linux-install-ncurses-library-headers-on-debian-ubuntu-centos-fedora/

########################################################

PARA COMPILAR:
Simplemente correr "make" en la terminal, dentro del directorio respectivo.

########################################################

Para correr el servidor:
./server <puerto>

Para correr el cliente:
./ClienteFinal <usuario> <ip-servidor> <puerto-servidor> <ip-usuario>

ejemplo cliente:
./ClienteFinal client1 127.0.0.1 4200 0.0.0.1

# NOTA: Se agregó el campo ip-usuario por motivos de testing. Esto es ya que se tienen que probar múltiples terminales en una misma máquina.

########################################################


