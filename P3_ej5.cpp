/*Escriba un programa que mida el ancho de banda provisto por pipes. Como argumentos en la
línea de comandos, el programa debería recibir el número de bloques de datos a ser enviados y
el tamaño de cada bloque. Después de crear el pipe, el programa debería dividirse en 2
procesos, uno que escribe bloques de datos tan rápido como posible y el otro que los lee.
Después que todos los datos son leídos, el programa padre debería imprimir el tiempo
insumido y el ancho de banda (bytes/seg). Medir el ancho de banda para distintos tamaños de
bloque.*/

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <chrono>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Se requieren dos argumentos: el número de bloques y el tamaño de cada bloque.\n";
        return 1;
    }

    int num_blocks = stoi(argv[1]); // número de bloques de datos a ser enviados. stoi convierte un string a int
    int block_size = stoi(argv[2]); // tamaño de cada bloque

    int h2t[2];                     // file descriptor del pipe que va del hijo al padre
    if (pipe(h2t) == -1) {
        cerr << "Error al crear el pipe.\n";
        return 1;
    }

    pid_t pid = fork();             // crea el proceso hijo
    if (pid == -1) {
        cerr << "Error al crear el proceso hijo.\n";
        return 1;
    } else if (pid == 0) {          // proceso hijo: escribe bloques de datos en el pipe
        close(h2t[0]);              // cierra el extremo de lectura del pipe del hijo al padre
        for (int i = 0; i < num_blocks; i++) {
            char buffer[block_size];    
            if (write(h2t[1], buffer, block_size) == -1) {  // escribe en el pipe del hijo al padre 
                cerr << "Error al escribir en el pipe.\n";
            }
        }
        close(h2t[1]);              // cierra el extremo de escritura del pipe del hijo al padre
        return 0;                   // termina el proceso hijo
    } else {                        // proceso padre: lee los bloques de datos del pipe
        close(h2t[1]);              // cierra el extremo de escritura del pipe del hijo al padre
        auto start_time = chrono::high_resolution_clock::now(); // toma el tiempo actual
        while(1){
            char buffer[block_size];
            if (read(h2t[0], buffer, block_size) == 0) {   // lee del pipe del hijo al padre
                close(h2t[0]);              // cierra el extremo de lectura del pipe del hijo al padre
                break;
            }
        }
        
        auto end_time = chrono::high_resolution_clock::now();
        auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        double bandwidth = (double)(num_blocks * block_size) /((double)elapsed_time.count()/1000.0)/1000000.0; //MB/seg

        cout << "Tiempo insumido: " << elapsed_time.count() << "ms\n";
        cout << "Ancho de banda: " << bandwidth << " Mbytes/seg\n";
        return 0;

        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}
