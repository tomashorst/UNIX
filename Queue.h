#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>

struct Queue{
    int putter; // Puntero de escritura
    int getter; // Puntero de lectura
    int size_queue;  // Tamaño de la cola
    int block_size;  // Tamaño de cada bloque

    sem_t *semlock; // Semáforo de acceso a la cola
    sem_t *semread; // Semáforo de lectura
    sem_t *semwrite; // Semáforo de escritura
    char data[0]; // Datos de la cola
    
    static Queue* create(const char* name, int size_queue_, int block_size_){
        shm_unlink(name);             // elimina la memoria compartida si existe
        int SIZE = sizeof(Queue) + sizeof(char) * block_size_ * size_queue_; // tamaño de la memoria compartida es:
        // el tamaño de la estructura Queue + el tamaño de los bloques de datos * sique_queue_
        // Tamaño de cola que sea chico, y que se vaya reciclando...
        int shmfd;                    // file descriptor de la memoria compartida
        shmfd = shm_open(name, O_RDWR | O_CREAT, 0640);   // crea la memoria compartida, si ya existe la abre
        if (shmfd == -1){
            printf("Error de shared memory\n");
            return NULL;
        } 
        int ret = ftruncate(shmfd, SIZE);   // trunca la memoria compartida al tamaño SIZE para que sea del tamaño necesario
        if(ret == -1){
            printf("Error de truncate\n");
            return NULL;
        }
        Queue * pCola_ = (Queue *) mmap(NULL, SIZE,
        PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0); // mapea la memoria compartida en el espacio de direcciones del proceso
        // sino se mapea, el hijo no podrá acceder a la memoria compartida
        
        if(pCola_ == MAP_FAILED){
            printf("Error de MMAP\n");
            return NULL;
        }
        pCola_->init(size_queue_, block_size_); // inicializa la cola en la memoria compartida
        return pCola_;
    }

    void init(int _size, int _block_size){ // inicializa la cola en la memoria compartida
        putter = 0;
        getter = 0;
        size_queue   = _size;
        block_size = _block_size;
        semlock  = sem_open("/SemLock10" , O_CREAT, 0640, 1);   // crea los semáforos de acceso a la cola
        assert(semlock != SEM_FAILED);
        semread  = sem_open("/SemRead10" , O_CREAT, 0640, 0);   // crea los semáforos de lectura
        assert(semread != SEM_FAILED);
        semwrite = sem_open("/SemWrite10", O_CREAT, 0640, size_queue); // crea los semáforos de escritura
        assert(semwrite != SEM_FAILED);
        printf("Naciendo\n");
    }

    void put(char* Block){  // pone un bloque de datos en la cola
        sem_wait(semwrite); // espera a que haya lugar para escribir
        sem_wait(semlock);  // espera a que no haya otro proceso escribiendo 
        memcpy(data + block_size*(putter%size_queue), Block, block_size);  // copia el bloque de datos en la cola
        putter ++;          // incrementa el puntero de escritura 
        sem_post(semlock);  // libera el semáforo de acceso a la cola
        sem_post(semread);  // incrementa el semáforo de lectura
    }

    void get(char *value){  // obtiene un bloque de datos de la cola
        sem_wait(semread);  // espera a que haya un bloque de datos para leer
        sem_wait(semlock);  // espera a que no haya otro proceso escribiendo
        memcpy(value, data + block_size*(getter%size_queue), block_size); // copia el bloque de datos de la cola
        getter ++;          // incrementa el puntero de lectura
        sem_post(semlock);  // libera el semáforo de acceso a la cola
        sem_post(semwrite); // incrementa el semáforo de escritura
    }

    int dixr(){                 // devuelve la cantidad de bloques de datos en la cola
        return putter - getter; // la diferencia entre el puntero de escritura y el de lectura
    }
};

void colaDestructor(Queue *pQ){ // destruye la cola
        int check;
        printf("Muriendo\n");
        check = sem_close(pQ -> semlock);   // cierra los semáforos de acceso a la cola
        assert(check != -1);
        check = sem_close(pQ -> semread);   // cierra los semáforos de lectura
        assert(check != -1);
        check = sem_close(pQ -> semwrite);  // cierra los semáforos de escritura
        assert(check != -1);

        check = sem_unlink("/SemLock10");   // elimina los semáforos de acceso a la cola
        assert(check != -1);
        check = sem_unlink("/SemRead10");   // elimina los semáforos de lectura
        assert(check != -1);
        check = sem_unlink("/SemWrite10");  // elimina los semáforos de escritura
        assert(check != -1);
}