/*Hacer el programa anterior utilizando el system call select para leer de stdin y del pipe.*/
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
using namespace std;
#define BUFFER_SIZE 1064 // Tamaño del buffer porque sí

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "Error: no se pudo crear el pipe\n";
        return 1;
    }

    fd_set readfds;                     // Conjunto de file descriptors a leer con select
    int maxfd;                          // El file descriptor más grande

    FD_ZERO(&readfds);                  // Inicialización
    FD_SET(STDIN_FILENO, &readfds);     // Agregar stdin al conjunto para leer 
    FD_SET(pipefd[0], &readfds);        // Agregar el file descriptor del pipe al conjunto para leer

    if (STDIN_FILENO > pipefd[0]) {     // Si la entrada estándar es mayor que el file descriptor del pipe
        maxfd = STDIN_FILENO;           // Actualizo el máximo con la entrada estándar
    } else {
        maxfd = pipefd[0];              // Sino el máximo es el file descriptor del pipe
    }

    cout << "Ingrese texto: ";
    string input;
    getline(cin, input);

    if (fork() == 0) {                  // Proceso hijo
        close(pipefd[0]);               
        dup2(pipefd[1], STDOUT_FILENO); // Redirigir la salida estándar al pipe para escribir
        execlp("perl", "perl", "-pe", "$_ = uc $_", nullptr); // Ejecutar el programa anterior
    } else {
        close(pipefd[1]);

        char buffer[BUFFER_SIZE];
        int nready;
        do {
            nready = select(maxfd + 1, &readfds, NULL, NULL, NULL); // Esperar a que haya algo para leer
            if (nready == -1) {
                cerr << "Error: select falló\n";
                return 1;
            }

            if (FD_ISSET(STDIN_FILENO, &readfds)) {                     // Si hay algo para leer en stdin se escribe en el pipe
                getline(cin, input);
                cout << "Texto ingresado: " << input << endl;
                return 0;
            }

            if (FD_ISSET(pipefd[0], &readfds)) {                        // Si hay algo para leer en el pipe se lee y se imprime
                int nbytes = read(pipefd[0], buffer, BUFFER_SIZE);      // Leer del pipe y guardar en el buffer
                if (nbytes == -1) {
                    cerr << "Error: no se pudo leer del pipe\n";
                    return 1;
                } else if (nbytes == 0) {
                    cout << "El proceso hijo ha cerrado el pipe\n";
                } else {
                    buffer[nbytes] = '\0';
                    cout << "Texto convertido: " << buffer << endl;
                }
            }

        } while (nready > 0);

        close(pipefd[0]);
    }

    return 0;
}
