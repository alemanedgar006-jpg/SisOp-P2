#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 9999
#define TAM 1024

int main() {

    int servidor, cliente;
    struct sockaddr_in direccion;
    socklen_t longitud = sizeof(direccion);
    pid_t pid;

    servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor == -1) {
        perror("Error al crear socket");
        exit(1);
    }

    memset(&direccion, 0, sizeof(direccion));

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);
    direccion.sin_addr.s_addr = INADDR_ANY;

    int opcion = 1;
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR,
               &opcion, sizeof(opcion));

    if (bind(servidor, (struct sockaddr *)&direccion,
             sizeof(direccion)) == -1) {
        perror("Error en bind");
        close(servidor);
        exit(1);
    }
    if (listen(servidor, 5) == -1) {
        perror("Error en listen");
        close(servidor);
        exit(1);
    }

    printf("Servidor escuchando en el puerto %d...\n", PUERTO);
    while (1) {

        cliente = accept(servidor,
                         (struct sockaddr *)&direccion,
                         &longitud);

        if (cliente == -1) {
            perror("Error en accept");
            continue;
        }

        printf("Cliente conectado.\n");

        pid = fork();

        if (pid == -1) {
            perror("Error en fork");
            close(cliente);
            continue;
        }

        if (pid == 0) {


            close(servidor);

            FILE *archivo = fopen("Google.html", "rb");

            if (archivo == NULL) {
                perror("Error al abrir Google.html");
                close(cliente);
                exit(1);
            }

            char buffer[TAM];
            size_t bytes;

            while ((bytes = fread(buffer, 1, TAM, archivo)) > 0) {

                size_t enviados = 0;

                while (enviados < bytes) {

                    ssize_t n = send(cliente,
                                     buffer + enviados,
                                     bytes - enviados,
                                     0);

                    if (n <= 0) {
                        perror("Error al enviar");
                        fclose(archivo);
                        close(cliente);
                        exit(1);
                    }

                    enviados += n;
                }
            }

            printf("Archivo enviado correctamente.\n");

            fclose(archivo);
            close(cliente);
            exit(0);

        } else {


            close(cliente);
        }
    }

    close(servidor);

    return 0;
}

