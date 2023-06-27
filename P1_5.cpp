/*Diseñe un programa que escriba archivos que contengan 10 bloques de 4 bytes cada uno separados
entre ellos por “huecos” de 32, 512, 8KB y 1MB (cada archivo con un tamaño de huecos distinto).
Recuperar la información de los archivos con fstat y analizar los campos st_size y st_blocks.
Explique sus conclusiones.*/

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <dirent.h>
using namespace std;

int main() {
    int fd = open("P1_e5_archivo.txt", O_CREAT | O_WRONLY, 0664);
    if (fd == -1) {
        std::cout << "Error creando archivo.txt" << std::endl;
    }

    lseek(fd, 1000000, SEEK_SET); // Agujero de 1MB 
    write(fd, "Boca", 4);   // No puede faltar

    struct stat st;
    if (fstat(fd, &st) == -1) {             
        cerr << "Error al obtener información del archivo" << endl;
        return 1;
    }

    cout << "Tamaño del archivo: " << st.st_size << endl;
    cout << "Bloques asignados: " << st.st_blocks << endl;
    cout << "Tamaño de bloque: " << st.st_blksize << endl;

    return 0;
}