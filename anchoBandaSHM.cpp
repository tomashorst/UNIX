#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <string>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include "MxQueueBW.h"

int nBlock;
int sizeBlock;
int shmfd;

void hijo();
void padre(int);

Queue* pCola;

int main(int argc, char *argv[]){
    shm_unlink("/mem");
    if (argc != 3){
        printf("Primer argumento cantidad, segundo tamaño.");
        return 1;
    }
    nBlock = atoi(argv[1]);
    if (nBlock == 0){
        printf("Falta argumento de cantidad de paquetes");
        return 0;
    }

    sizeBlock = atoi(argv[2]);
    if(sizeBlock == 0){
        printf("Falta argumento de tamaño de paquetes");
        return 0;
    }

    int SIZE = sizeof(Queue) + sizeof(char) * sizeBlock * 100;

    //Shared Memory
    shmfd = shm_open("/mem", O_RDWR | O_CREAT, 0640);
    if (shmfd == -1){
        printf("Error de shared memory\n");
        return -1;
    } 
    int ret = ftruncate(shmfd, SIZE);
    if(ret == -1){
        printf("Error de truncate\n");
        return -1;
    }

    pCola = (Queue *) mmap(NULL, SIZE,
     PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    
    if(pCola == MAP_FAILED){
        printf("Error de MMAP\n");
        return -1;
    }

    *pCola = *new Queue(100, sizeBlock);

    //Forkeo un hijo
    pid_t pid = fork();

    switch( pid ) {
        case 0:
            hijo();
            break;
            
        case -1:
            perror("error en el fork:");
            break;
            
        default:
            padre(pid);
            colaDestructor(pCola);
            shm_unlink("/mem");
            munmap(pCola, SIZE);
            break;
    }

    return 0;
}

void hijo(){
    char* Block = new char[sizeBlock];

    for(int i = 0; i < nBlock; i++){
        pCola -> put(Block);
    }

    delete[] Block;
}

void padre(int pid){
    clock_t t_inicial = clock();

    char* BlockContainer = new char [sizeBlock];
    for(int i = 0; i < nBlock; i++)
        pCola -> get(BlockContainer);
    delete[] BlockContainer;

    clock_t t_final = clock();
    double t = ((double)(t_final - t_inicial)) / CLOCKS_PER_SEC;

    printf("Tiempo = %lf \n", t);
    printf("Ancho de banda = %lf MBps\n", 1.0*nBlock*sizeBlock/t/1000000);
}


