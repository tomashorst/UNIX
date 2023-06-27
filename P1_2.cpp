//Hacer un programa que utilice umask y luego open para crear varios archivos con distintos permisos por default.

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
using namespace std;


int main() {
    // Establecemos la máscara de permisos
    umask(002);

    // Creamos el primer archivo con permisos de lectura y escritura para el usuario y grupo y solo lectura para otros
    int fd1 = open("archivo1.txt", O_CREAT | O_WRONLY, 0664);
    if (fd1 == -1) {
        std::cout << "Error creando archivo1.txt" << std::endl;
    }

    // Creamos el segundo archivo con permisos 640 de lectura y escritura para el usuario y solo lectura para el grupo y otros
    int fd2 = open("archivo2.txt", O_CREAT | O_WRONLY, 0640);
    if (fd2 == -1) {
        std::cout << "Error creando archivo2.txt" << std::endl;
    }

    // Creamos el tercer archivo con permisos 600 de lectura y escritura para el usuario
    int fd3 = open("archivo3.txt", O_CREAT | O_WRONLY, 0600);
    if (fd3 == -1) {
        std::cout << "Error creando archivo3.txt" << std::endl;
    }

    return 0;
}
 