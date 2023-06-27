/*Hacer un programa que ejecute otro programa como filtro, le mande datos por su stdin, los lea de stdout y finalmente los imprima. 
Usar como programa el siguiente comando: "perl -pe '$_ = uc $_'".*/
#include <iostream>
#include <unistd.h>
#include <assert.h>

using namespace std;

const int BUFFER_SIZE = 32;

int main() {
    int pipefd[2]; // Pipe de padre a hijo
    int pipefd2[2]; // Pipe de hijo a padre
    // Crear el pipe
    if (pipe(pipefd) == -1) {
        cerr << "Error al crear el pipe" << endl; // Cerr para imprimir en la pantalla de error
        return 1;
    }
    if (pipe(pipefd2) == -1) {
        cerr << "Error al crear el pipe" << endl; // Cerr para imprimir en la pantalla de error
        return 1;
    }
    pid_t pid = fork(); // Crear el proceso hijo

    if (pid == -1) {
        cerr << "Error al crear el proceso hijo" << endl;
        return 1;
    } else if (pid == 0) {  // Proceso hijo
        close(pipefd[1]);  // Cerrar la escritura del pipe del padre al hijo
        close(pipefd2[0]); // Cerrar la lectura del pipe del hijo al padre
        
        // Redirigir stdin hacia el pipe
        int ret = 0;
        ret = dup2(pipefd[0], STDIN_FILENO); // STDIN_FILENO es para que lea de la entrada estandar
        assert(ret != -1);
        ret = dup2(pipefd2[1], STDOUT_FILENO); // STDOUT_FILENO es para que escriba en la salida estandar
        assert(ret != -1);
        close(pipefd[0]);
        close(pipefd2[1]);

        execlp("perl", "perl", "-pe", "$_ = uc $_", NULL);  // Para ejecutar otro programa como filtro
        assert(0);  // Si llega acá es porque el execlp falló
    } else {  // Proceso padre
        close(pipefd[0]);  // Cerrar la lectura del pipe del padre al hijo
        close(pipefd2[1]);  // Cerrar la escritura del pipe del hijo al padre

        char buffer[BUFFER_SIZE];
        int bytes_read;

        // Leer de stdin y escribir en el pipe
        while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) { // del stdin se lee
            write(pipefd[1], buffer, bytes_read); // Se escribe del padre al hijo
        }
        buffer[0] = 0;

        close(pipefd[1]);  // Cerrar la escritura del pipe
        
        // Leer del pipe y escribir en stdout
        while ((bytes_read = read(pipefd2[0], buffer, BUFFER_SIZE)) > 0) { // del pipe se lee (del execlp)
            write(STDOUT_FILENO, buffer, bytes_read); // Se escribe en el STDOUT
        }
        close(pipefd2[0]);
        
        exit(0);
    }
}
