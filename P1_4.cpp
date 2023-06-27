// Hacer un programa que genere un file con agujeros grandes utilizando lseek. Recuperar la
// iformación del file con fstat e imprimir los campos de la estructura stat. Comparar con lo que
// muestran los comandos ls y du sobre el archivo.

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
    int fd = open("P1_ej4_archivo.txt", O_CREAT | O_WRONLY, 0664);
    if (fd == -1) {
        std::cout << "Error creando archivo.txt" << std::endl;
    }

    lseek(fd, 1000000, SEEK_SET); // Agujero de 1MB 
    write(fd, "Boca", 4);

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