// Usando las funciones los comandos “ln ...” y “ln –s ... ” verificar como se afecta el reference count de un ínodo.
// Verificar con la función stat(2).

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
    // Crear el archivo de prueba con open
    int fd = open("p1_ej6__.txt", O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        cout << "Error al crear el archivo" << endl;
        return 1;
    }
    close(fd);
    
    // Obtener el número de referencia actual del archivo utilizando la función stat
    struct stat st;
    stat("p1_ej6__.txt", &st);
    cout << "Current reference count: " << st.st_nlink << endl;
    
    return 0;
}