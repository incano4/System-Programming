// будет использован один recvfrom, чтобы получить данные клиента
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5000
// #define CLIENT_PORT 5001
#define BUFFER_SIZE 4096

int start_server_socket(int socket_type);
int start_client_socket(int socket_type);
void reverse_string(char *message, size_t length);


int main(int argc, char *argv[]) {

    int socket_type = SOCK_DGRAM;

	pid_t pid = fork();
    if (pid < 0) {
        puts("Fork error");
        return 1;
    } else if (pid == 0 ) {
        puts("Fork success");
        sleep (1);
        start_client_socket(socket_type);

    } else {
        start_server_socket(socket_type);
    }
    return 0;
}

int start_server_socket(int socket_type) {
    int socket_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_address, client_address;

    if ((socket_fd = socket(AF_INET, socket_type, 0)) == -1) { // AF_INET - IPv4; Socket type - SOCK_DGRAM (udp)
        puts("Could not create server socket\n");
        return 1;
    }

    socklen_t addrlen = sizeof(server_address);
    memset(&server_address, 0, addrlen); // обнуляем то, что находится по адресу, лежащему в server_address (т.е. структуру)
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;

    if ((bind(socket_fd, (struct sockaddr *)&server_address, addrlen)) == -1) {
            puts("Binding error");
            close(socket_fd);
            return 1;
        }

    socklen_t client_len = sizeof(client_address);
    int client_fd;
    
    recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, &client_len);
    printf("Message from client: %s\n", buffer);\
    reverse_string(buffer, strlen(buffer));


    if ((connect(socket_fd, (struct sockaddr *)&client_address, client_len)) == -1) {
        return 1;
    }

    send(socket_fd, buffer, strlen(buffer), 0);
    close(socket_fd);
    return 0;
}

int start_client_socket(int socket_type) {

    // подготавливаем сообщение
    char message[BUFFER_SIZE];
    printf("Enter client message: ");
    fgets(message, BUFFER_SIZE, stdin);
    message[strcspn(message, "\n")] = 0;

    int client_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_address;

    if ((client_fd = socket(AF_INET, socket_type, 0)) == -1) { // AF_INET - IPv4; Socket type - SOCK_DGRAM (udp)
        puts("Could not create client socket\n");
        return 1;
    }

    socklen_t addrlen = sizeof(server_address);
    memset(&server_address, 0, addrlen);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;


    // используем UDP + connect
    if ((connect(client_fd, (struct sockaddr *)&server_address, addrlen)) == -1) {
        puts("Connection error\n");
        close(client_fd);
        return 1;
    }

    send(client_fd, message, strlen(message), 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, message, BUFFER_SIZE, 0);

    printf("Server reply: %s\n", message);

    close(client_fd);
    return 0;

}

void reverse_string(char *message, size_t length) {
    for (size_t i = 0; i < length / 2; i++) {
        char tmp = message[i];
        message[i] = message[length - i - 1];
        message[length - i - 1] = tmp;
    }
}
