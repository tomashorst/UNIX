// Hacer un programa que liste los archivos en el directorio actual y su tamaño. En caso que existan subdirectorios,
// listar también sus contenidos de la misma forma. Utilizar opendir/readdir/closedir.

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

void listarArchivos(const char* directorio) {
    DIR *dir;
    struct dirent *ent;
    struct stat st;

    dir = opendir(directorio);
    if (dir == NULL) {
        cerr << "Error al abrir el directorio" << endl;
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        const string nombreArchivo = ent->d_name;
        const string rutaArchivo = string(directorio) + "/" + nombreArchivo;

        if (nombreArchivo == "." || nombreArchivo == "..") {
            continue;
        }

        if (stat(rutaArchivo.c_str(), &st) == -1) {
            cerr << "Error al obtener información del archivo " << rutaArchivo << endl;
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            cout << "Directorio: " << nombreArchivo << endl;
            listarArchivos(rutaArchivo.c_str());
        } else {
            cout << "Archivo: " << nombreArchivo << " (" << st.st_size << " bytes)" << endl;
        }
    }

    closedir(dir);
}

int main() {
    listarArchivos(".");
    return 0;
}