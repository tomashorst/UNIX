/*Repita nuevamente para colas implementadas en memoria compartida.*/

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <chrono>
#include <sys/shm.h>
#include <fcntl.h>
#include "Queue.h"
#include <sys/mman.h>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Se requieren dos argumentos: el número de bloques y el tamaño de cada bloque.\n";
        return 1;
    }

    int num_blocks = stoi(argv[1]); // número de bloques de datos a ser enviados. stoi convierte un string a int
    int block_size = stoi(argv[2]); // tamaño de cada bloque
    int num_queue = 100;            // tamaño de la cola

    Queue* pCola = Queue::create("pCola",num_queue, block_size);    // crea la cola en memoria compartida ya inicializada
    assert(pCola != NULL);

    pid_t pid = fork();             // crea el proceso hijo
    if (pid == -1) {
        cerr << "Error al crear el proceso hijo.\n";
        return 1;
    } else if (pid == 0) {          // proceso hijo: escribe mediante put() los bloques de datos de la cola
        for (int i = 0; i < num_blocks; i++) {
            char buffer[block_size];   
            pCola->put(buffer);
        }
        return 0;                   // termina el proceso hijo
    } else {                        // proceso padre: lee mediante get() los bloques de datos de la cola
        auto start_time = chrono::high_resolution_clock::now(); // toma el tiempo actual
        for (int i = 0; i < num_blocks; i++) {
            char buffer[block_size];
            pCola->get(buffer);
        }
        
        auto end_time = chrono::high_resolution_clock::now();
        auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        double bandwidth = (double)(num_blocks * block_size) /((double)elapsed_time.count()/1000.0)/1000000.0; //MB/seg

        cout << "Tiempo insumido: " << elapsed_time.count() << "ms\n";
        cout << "Ancho de banda: " << bandwidth << " Mbytes/seg\n";
        return 0;
    }

    return 0;
}

// que el padrea, antes del fork, cree la cola, la inicialice, el padre hace put y el hijo get
// Factor 2 de + veloz aprox