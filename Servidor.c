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
#define NUM_AUTOS 5

bool autos_disponibles[NUM_AUTOS] = {true, true, true, true, false}; // Ejemplo de autos disponibles
int viajes_realizados = 0;
int ganancia_total = 0;
/**
 * Imprime la dirección IP del cliente.
 */
void print_client_ip(struct sockaddr_in client_addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN);
    printf("Cliente conectado desde la IP: %s\n", ip);
}
/**
 * Maneja la solicitud del cliente.
 */
void handle_request(int client_socket)
{
    char buffer[1024] = {0};
    int read_result;
    while ((read_result = read(client_socket, buffer, sizeof(buffer))) > 0)
    {

        if (strcmp(buffer, "estado") == 0)
        {
            // Servicio "estado"
            char response[1024];
            sprintf(response, "Viajes realizados: %d\nGanancia total: $%d", viajes_realizados, ganancia_total);
            send(client_socket, response, sizeof(response),0);
        }
        else if (strcmp(buffer, "viaje") == 0)
        {
            // Servicio "viaje"
            int auto_disponible = -1;
            for (int i = 0; i < NUM_AUTOS; ++i)
            {
                if (autos_disponibles[i])
                {
                    auto_disponible = i;
                    break;
                }
            }

            if (auto_disponible != -1)
            {
                // Auto disponible
                int costo_viaje = rand() % 199 + 10; // Costo aleatorio entre $10 y $199
                ganancia_total += costo_viaje;
                viajes_realizados++;
                char response[1024];
                sprintf(response, "Placas del auto: %d\nCosto del viaje: $%d", auto_disponible, costo_viaje);
                send(client_socket, response, sizeof(response),0);
                autos_disponibles[auto_disponible] = false; // Marcar el auto como ocupado
            }
            else
            {
                // No hay conductores disponibles
                char response[] = "No hay conductores";
                send(client_socket, response, sizeof(response),0);
            }
        }
        else if (strcmp(buffer, "viaje_terminado") == 0)
        {
            // Servicio "viaje_terminado"
            int placas_auto;
            read(client_socket, &placas_auto, sizeof(placas_auto));
            // Marcar el auto como disponible nuevamente
            autos_disponibles[placas_auto] = true; 
            printf("Viaje del auto con placas %d terminado. Auto disponible nuevamente.\n", placas_auto);
            // Avisar al cliente que el viaje ha terminado
            char response[] = "Viaje terminado";
            send(client_socket, response, sizeof(response),0);
        }
        else
        {
            // Solicitud desconocida
            printf("Solicitud desconocida: %s\n", buffer);
        }
        // Limpiar el búfer
        memset(buffer, 0, sizeof(buffer));
    }
    if (read_result == 0)
    {
        printf("Cliente desconectado.\n");
    }
    else
    {
        perror("Error al leer del cliente");
    }
    close(client_socket);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Crear el socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0); // Protocolo TCP

    if (server_socket == -1)
    {
        perror("Error en la creación del socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Vincular el socket al puerto
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error en el enlace al puerto");
        exit(EXIT_FAILURE);
    }

    // Escuchar las conexiones entrantes
    if (listen(server_socket, 5) == -1)
    {
        perror("Error al escuchar las conexiones");
        exit(EXIT_FAILURE);
    }

    printf("Servidor esperando conexiones...\n");

    while (1)
    {
        // Aceptar la conexión entrante
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1)
        {
            perror("Error al aceptar la conexión");
            exit(EXIT_FAILURE);
        }

        print_client_ip(client_addr);

        // Manejar la solicitud del cliente
        handle_request(client_socket);
    }

    return 0;
}
