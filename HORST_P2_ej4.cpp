/*Hacer un programa que ejecute otro programa como filtro, le mande datos por su stdin, los lea de stdout y finalmente los imprima.
Usar como programa el siguiente comando: "perl -pe '$_ = uc $_'".*/
// Horst R. Tomás
// Laboratorio de redes
// 20/04/2023
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

const int BUFFER_SIZE = 1064;

int main()
{
    int p2h[2];  // Pipe de padre a hijo
    int h2p[2];  // Pipe de hijo a padre
    // Creando los pipes
    if (pipe(p2h) == -1) 
    {
        cerr << "Error al crear el pipe" << endl; // Cerr para imprimir en la pantalla de error
        return 1;
    }
    if (pipe(h2p) == -1)
    {
        cerr << "Error al crear el pipe" << endl; // Cerr para imprimir en la pantalla de error
        return 1;
    }
    pid_t pid = fork(); // Creando el proceso hijo

    if (pid == -1)
    {
        cerr << "Error al crear el proceso hijo" << endl;
        return 1;
    }
    else if (pid == 0)  // Proceso hijo
    {                      
        close(p2h[1]);  // Cerrar la escritura del pipe del padre al hijo
        close(h2p[0]);  // Cerrar la lectura del pipe del hijo al padre

        // Redirigiendo la entrada y salida estandar al pipe
        int ret = 0;
        ret = dup2(p2h[0], STDIN_FILENO);   // STDIN_FILENO es para que lea de la entrada estandar
        assert(ret != -1);
        ret = dup2(h2p[1], STDOUT_FILENO);  // STDOUT_FILENO es para que escriba en la salida estandar
        assert(ret != -1);  // Si ret es -1 es porque hubo un error
        close(p2h[0]);  // Una vez que se redirigió la entrada estandar, se cierra el file descriptor
        close(h2p[1]);  // Una vez que se redirigió la salida estandar, se cierra el file descriptor

        execlp("perl", "perl", "-pe", "$_ = uc $_", NULL); // execlp para ejecutar otro programa como filtro
        // Este programa es el que se ejecuta en el proceso hijo, y devuelve lo que se le mande por la entrada estandar (STDIN_FILENO) en mayúsculas
        assert(0);  // Si llega acá es porque el execlp falló
    }
    else    // Proceso padre 
    {                      
        close(p2h[0]);  // Cerrar la lectura del pipe del padre al hijo
        close(h2p[1]);  // Cerrar la escritura del pipe del hijo al padre
        bool STDIN_FLAG = true; // Flag para saber si se debe leer de la entrada estandar
        while (1)
        {
            fd_set rfds;    // Conjunto de file descriptors para (lectura) select
            int retval;     // Valor entero de retorno de select
            FD_ZERO(&rfds); // Inicializar el conjunto de file descriptors en 0
            if (STDIN_FLAG){
                FD_SET(STDIN_FILENO, &rfds);    // Si debo leer por el flag true, entonces agrego el file descriptor de la entrada estandar
            }
            FD_SET(h2p[0], &rfds);  // Agrego el file descriptor del pipe del hijo al padre en el conjunto de file descriptors
            retval = select(h2p[0] + 1, &rfds, NULL, NULL, NULL);   // Ejecuto select con el conjunto de file descriptors, y el file descriptor más grande + 1 (+1 just because)
            if (retval == -1)   // Si retval retorna -1 es porque hubo un error en select
            {   
                perror("select()");
                return 1;
            }
            char buffer[BUFFER_SIZE];   // Buffer de tipo char para leer y escribir
            if (FD_ISSET(0, &rfds))     // Si el file descriptor de la entrada estandar está en el conjunto de file descriptors
            {
                if (!(retval = read(0, buffer, BUFFER_SIZE))) // Si no se leyó nada de la entrada estandar, entonces se cierra el file descriptor
                {
                    puts("\nbye stdin");
                    close(p2h[1]);
                    STDIN_FLAG = false; // Se cambia el flag a false para que no se vuelva a leer de la entrada estandar
                }
                else{
                    write(p2h[1], buffer, retval);  // Si se leyó algo de la entrada estandar(esta en el buffer y su tamaño en retval), se escribe en el pipe del padre al hijo
                }
            }
            
            if (FD_ISSET(h2p[0], &rfds))    // Si el file descriptor de lectura del pipe del hijo al padre está en el conjunto de file descriptors
            {
                if (!(retval = read(h2p[0], buffer, BUFFER_SIZE)))  // Si no se leyó nada del pipe del hijo al padre, entonces se cierra el file descriptor
                {
                    puts("\nbye h2p");
                    close(h2p[0]);
                    break;
                }
                write(STDOUT_FILENO, buffer, retval);   // Si se leyó algo del pipe del hijo al padre(esta en el buffer y su tamaño en retval), se escribe en la salida estandar
            }
        }

    }
}
