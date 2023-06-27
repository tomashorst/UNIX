//Hacer un programa que abra archivos con flags O_EXCL y O_CREAT. Verificar el comportamiento esperado cuando ambos flags existen y el archivo existe previamente.

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
using namespace std;

int main(int argc, char *argv[])
{
    int fd;
    int flags = O_CREAT | O_EXCL;
    int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    if ((fd = open("archivo.txt", flags, mode)) == -1)   //Crea el archivo si no existe
    {
        perror("open");
        exit(1);
    }

    return 0;
}