#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
using namespace std;

/*Hacer un programa que ejecute otro programa como filtro, le mande datos por su stdin, los lea de
stdout y finalmente los imprima. Utilizando el system call select para leer de stdin y del pipe.*/

//Para correr otro programa como filtro debo utilizar un fork() y un execve()
//Todo programa tiene 3 streams abiertos, uno para input, otro para output y otro para imprimir mensajes de error o diagnosticos
//stdin, stdout, stderr
//Debo mandarle datos al programa por stdin y que los lea por stdout. Debo utilizar un pipe para esto y un system call select
//para leer de stdin y del pipe

int main(int argc, char *argv[])
{
    int fd[2];      //file descriptor
    int pid;        //pid del hijo
    int status;     //status del hijo
    int n;          //cantidad de bytes leidos
    char buf[10];   //buffer para leer de stdin


    //Debo utilizar system call select para leer de stdin y del pipe

    if (pipe(fd) == -1) //creo el pipe
    {
        perror("pipe");
        exit(1);
    }

    if ((pid = fork()) == -1) //creo el fork
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0) //hijo
    {
        close(fd[1]); //cierro el file descriptor de escritura
        dup2(fd[0], 0); //duplico el file descriptor de lectura en el file descriptor de stdin
        close(fd[0]); //cierro el file descriptor de lectura
        execlp("wc", "wc", "-l", NULL); //ejecuto el programa wc
        perror("execlp");
        exit(1);
    }

    else //padre
    {
        close(fd[0]); //cierro el file descriptor de lectura
        while ((n = read(0, buf, 10)) > 0) //leo de stdin
        {
            write(fd[1], buf, n); //escribo en el pipe
            //Termina cuando se presiona Ctrl+D. 
        }
        close(fd[1]); //cierro el file descriptor de escritura
        wait(&status); //espero a que el hijo termine
    }
    return 0;
}

