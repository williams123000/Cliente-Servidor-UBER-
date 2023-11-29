#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>


#define PORT 8080
/**
 * Uso del cliente: ./cliente [estado|viaje] IP_SERVIDOR
 */
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso: %s [estado|viaje] IP_SERVIDOR\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};

    // Crear el socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == -1)
    {
        perror("Error en la creación del socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    /* htons convierte un entero largo en octetos cuyo orden entienden
     * las funciones de los sockets
     */
    server_addr.sin_port = htons(PORT);
    /* inet_addr convierte de cadena de caracteres (formato puntado
     * "128.112.123.1") a
     * octetos.
     */
    server_addr.sin_addr.s_addr = inet_addr(argv[2]);

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error en la conexión al servidor");
        exit(EXIT_FAILURE);
    }

    // Enviar la solicitud al servidor
    send(client_socket, argv[1], strlen(argv[1]),0);

    if (strcmp(argv[1], "viaje_terminado") == 0)
    {
        // Solicitar al usuario que ingrese la placa del automóvil
        int placas_auto;
        printf("Ingrese la placa del automóvil utilizado: ");
        scanf("%d", &placas_auto);
        send(client_socket, &placas_auto, sizeof(placas_auto),0);
    }
    
    // Recibir y mostrar la respuesta del servidor
    recv(client_socket, buffer, sizeof(buffer),0);
    printf("Respuesta del servidor:\n%s\n", buffer);

    // Cerrar el socket
    close(client_socket);

    return 0;
}
