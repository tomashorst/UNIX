/*Hacer un programa que abra 2 pipes, forkee un hijo, lea de stdin, lo mande al hijo por un pipe, lea lo
que el hijo le devuelve y lo imprima por pantalla. El hijo debe leer lo que le envía el padre, pasarlo a
mayúsculas y reenviárselo al padre.*/

#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;

const int BUFFER_SIZE = 32;

int main() {
    int pipefd[2], pipefd2[2]; // Los file descriptors de los pipes

    if (pipe(pipefd) == -1 || pipe(pipefd2) == -1) {
        cerr << "Error al crear los pipes" << endl;
        return 1;
    }

    pid_t pid = fork(); // Crear el proceso hijo

    if (pid == -1) {
        cerr << "Error al crear el proceso hijo" << endl;
        return 1;
    } else if (pid == 0) {  // Proceso hijo
        close(pipefd[1]);   // Cerrar la escritura del primer pipe
        close(pipefd2[0]);  // Cerrar la lectura del segundo pipe

        char buffer[BUFFER_SIZE];
        int bytes_read;

        while ((bytes_read = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            // Convertir a mayúsculas
            for (int i = 0; i < bytes_read; i++) {
                buffer[i] = toupper(buffer[i]);
            }
            // Enviar de vuelta al padre
            write(pipefd2[1], buffer, bytes_read);
        }

        close(pipefd[0]);  // Cerrar la lectura del primer pipe
        close(pipefd2[1]); // Cerrar la escritura del segundo pipe
        exit(0);
    } else {  // Proceso padre
        close(pipefd[0]);   // Cerrar la lectura del primer pipe
        close(pipefd2[1]);  // Cerrar la escritura del segundo pipe

        char buffer[BUFFER_SIZE];
        int bytes_read;

        while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
            // Enviar al hijo
            write(pipefd[1], buffer, bytes_read);
            // Leer lo que el hijo envía de vuelta
            bytes_read = read(pipefd2[0], buffer, BUFFER_SIZE);
            // Imprimir en pantalla
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        close(pipefd[1]);
        close(pipefd2[0]);
        exit(0);
    }
}
