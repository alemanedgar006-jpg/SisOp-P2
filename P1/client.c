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

int main(int argc, char *argv[]) {

    int cliente;
    struct sockaddr_in servidor;
    char buffer[TAM];
    ssize_t bytes;

    // Comprobar argumentos
    if (argc != 2) {
        printf("Uso: Cliente IP_Servidor\n");
        exit(1);
    }

    // 1. Crear socket
    cliente = socket(AF_INET, SOCK_STREAM, 0);

    if (cliente == -1) {
        perror("Error al crear socket");
        exit(1);
    }

    // 2. Configurar direccion del servidor
    memset(&servidor, 0, sizeof(servidor));

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);

    servidor.sin_addr.s_addr = inet_addr(argv[1]);

    // 3. Conectar con el servidor
    if (connect(cliente,
                (struct sockaddr *)&servidor,
                sizeof(servidor)) == -1) {

        perror("Error en connect");
        close(cliente);
        exit(1);
    }

    printf("Conectado al servidor.\n");
    printf("Contenido del archivo Google.html:\n\n");

    // 4. Recibir archivo
    while ((bytes = recv(cliente, buffer, TAM, 0)) > 0) {

        // Mostrar los datos recibidos
        fwrite(buffer, 1, bytes, stdout);
    }

    if (bytes == -1) {
        perror("Error en recv");
    }

    printf("\n\nFin de la transferencia.\n");

    // 5. Cerrar socket
    close(cliente);

    return 0;
}

