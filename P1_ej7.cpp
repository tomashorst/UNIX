/*Hacer un programa que obtenga memoria en un loop mientras pueda con new y otro que lo haga
con mmap anónimo. Cuánta memoria se puede obtener con cada método de allocación? Ver los
mapas de memoria de los procesos antes que terminen.*/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <cstdlib>
#include <string> 

using namespace std;

int main() {
    size_t size = 1; //  10 bytes
    int count = 0;
    int max = 1;
    while (max < 3) {
        // Obtener memoria con new
        void *mem = nullptr;
        try {
            mem = new char[size];
        } catch (const bad_alloc& e) {
            cout << "Error al obtener memoria con new: " << e.what() << endl;
            break;
        }
        memset(mem, 0, size);
        count++;
        cout << "Obtenida memoria #" << count << " con new" << endl;

        // Obtener memoria con mmap anónimo
        void *map = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (map == MAP_FAILED) {
            cout << "Error al obtener memoria con mmap: " << strerror(errno) << endl;
            break;
        }
        memset(map, 0, size);
        count++;
        cout << "Obtenida memoria #" << count << " con mmap" << endl;

        // Esperar un segundo
        sleep(1); 
        max++;
    }

    // Ver el mapa de memoria del proceso, con pmap, antes de terminar
    system(("pmap " + std::to_string(getpid())).c_str());

    return 0;
}
