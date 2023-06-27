/*Hacer un programa que ejecute otro programa (por ejemplo 'ls -l') e imprima el output del programa
ejecutado. Hint: pipe, fork, close, dup, exec, read, printf a stdout.*/

#include <iostream>
#include <unistd.h>
using namespace std;

const int BUFFER_SIZE = 32;

int main() {
    int pipefd[2];

    // Crear el pipe
    if (pipe(pipefd) == -1) {
        cerr << "Error al crear el pipe" << endl;
        return 1;
    }

    pid_t pid = fork(); // Crear el proceso hijo

    if (pid == -1) {
        cerr << "Error al crear el proceso hijo" << endl;
        return 1;
    } else if (pid == 0) {  // Proceso hijo
        close(pipefd[0]);   // Cerrar la lectura del pipe del hijo
        // Redirigir stdout hacia el pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Ejecutar el programa
        execlp("ls", "ls", "-l", NULL);
    } else {  // Proceso padre
        close(pipefd[1]);  // Cerrar la escritura del pipe del padre

        char buffer[BUFFER_SIZE];
        int bytes_read;

        while ((bytes_read = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            // Imprimir en pantalla
            write(STDOUT_FILENO, buffer, bytes_read);   // STDOUT_FILENO es para que imprima en la pantalla
        }

        close(pipefd[0]);
        exit(0);
    }
}
