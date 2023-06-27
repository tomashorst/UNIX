// Usando las funciones link y symlink verificar como se afecta el reference count de un ínodo.
// Verificar con la función stat(2). Hacer lo mismo con los comandos “ln ...” y “ln –s ... ” verificando con el comando stat.

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
    int fd = open("p1_ej6.txt", O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        cout << "Error al crear el archivo" << endl;
        return 1;
    }
    close(fd);
    
    // Obtener el número de referencia actual del archivo utilizando la función stat
    struct stat st;
    stat("p1_ej6.txt", &st);
    cout << "Current reference count: " << st.st_nlink << endl;
    
    // Crear un nuevo enlace al archivo utilizando la función link. El inodo no cambia porque esto es un enlace duro
    link("p1_ej6.txt", "p1_ej6_2.txt");  // Sólo me agrega un enlace, no cambia el inodo
    
    // Obtener el nuevo número de referencia del archivo utilizando la función stat
    cout << "New reference count after hard link: " << st.st_nlink << endl;
    
    // Crear un nuevo enlace simbólico al archivo utilizando la función symlink. Enlace directo sería
    symlink("p1_ej6.txt", "p1_ej6_symlink.txt");

    // Obtener el nuevo número de referencia del archivo utilizando la función stat
    cout << "New reference count after symbolic link: " << st.st_nlink << endl;

    // Crear otro más
    symlink("p1_ej6.txt", "p1_ej6_mysymlink2.txt");
    
    // Obtener el nuevo número de referencia del archivo utilizando la función stat
    cout << "New reference count after symbolic link 2: " << st.st_nlink << endl;
    
    return 0;
}