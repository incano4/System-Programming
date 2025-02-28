#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5000
#define BUFFER_SIZE 4096
#define KEY 0x5A

int start_server_socket(int socket_type);
int start_client_socket(int socket_type);
void xor_key_encryption_decryption(char *message, size_t length);
void reverse_string(char *message, size_t length);


int main(int argc, char *argv[]) {

    // проверка на tcp/udp
    int socket_type;
    if (argc != 2) {
        printf("Usage: %s --tcp | --udp\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--tcp") == 0) {
        socket_type = SOCK_STREAM;
    } else if (strcmp(argv[1], "--udp") == 0) {
        socket_type = SOCK_DGRAM;
    } else {
        printf("Invalid argument. Use --tcp or --udp\n");
        return 1;
    }


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

    if ((socket_fd = socket(AF_INET, socket_type, 0)) == -1) { // AF_INET - IPv4; Socket type - SOCK_STREAM (tcp) or SOCK_DGRAM (udp)
        puts("Could not create server socket\n");
        return 1;
    }

    socklen_t addrlen = sizeof(server_address);
    memset(&server_address, 0, addrlen); // обнуляем то, что находится по адресу, лежащему в server_address (т.е. структуру)
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;

    // см в comments для объясняния
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if ((bind(socket_fd, (struct sockaddr *)&server_address, addrlen)) == -1) {
            puts("Binding error");
            close(socket_fd);
            return 1;
        }

        socklen_t client_len = sizeof(client_address);
        int client_fd;

    if (socket_type == SOCK_STREAM) { // используем TCP
        if (listen(socket_fd, 3) == -1) { // 3 - максимальное количество соеднинений, ожидающих подключение. остальные отклоняются
            puts("Listening error");
            close(socket_fd);
            return 1;
        } else {
            printf("Listening on port %d\n", PORT);
        }

        if ((client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_len)) == -1) {
            puts("Accept failed");
            close(socket_fd);
            return 1;
        }

        // получаем сообщение
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        xor_key_encryption_decryption(buffer, strlen(buffer)); // расшифровываем
        printf("Message from client: %s\n", buffer);
        reverse_string(buffer, strlen(buffer));  
        xor_key_encryption_decryption(buffer, strlen(buffer)); // засшифровываем
        send(client_fd, buffer, strlen(buffer), 0);

        close(client_fd);

    } else { // используем UDP
        recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, &client_len);
        xor_key_encryption_decryption(buffer, strlen(buffer));
        printf("Message from client: %s\n", buffer);
        reverse_string(buffer, strlen(buffer));  
        xor_key_encryption_decryption(buffer, strlen(buffer));
        sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_address, client_len);

    }

    close(socket_fd);
    return 0;
}

int start_client_socket(int socket_type) {
    // подготавливаем сообщение
    char message[BUFFER_SIZE];
    printf("Enter client message: ");
    fgets(message, BUFFER_SIZE, stdin);
    message[strcspn(message, "\n")] = 0;
    xor_key_encryption_decryption(message, strlen(message));

    int client_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_address;

    if ((client_fd = socket(AF_INET, socket_type, 0)) == -1) { // AF_INET - IPv4; Socket type - SOCK_STREAM (tcp) or SOCK_DGRAM (udp)
        puts("Could not create client socket\n");
        return 1;
    }

    socklen_t addrlen = sizeof(server_address);
    memset(&server_address, 0, addrlen);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;


    if (socket_type == SOCK_STREAM) { // используем TCP
        if ((connect(client_fd, (struct sockaddr *)&server_address, addrlen)) == -1) {
            puts("Connection error\n");
            close(client_fd);
            return 1;
        } // если соединение установлено и по TCP
        send(client_fd, message, strlen(message), 0);

    } else { // используем UDP
        if ((connect(client_fd, (struct sockaddr *)&server_address, addrlen)) == -1) {
            puts("Connection error\n");
            close(client_fd);
            return 1;
        }
        sendto(client_fd, message, strlen(message), 0, (struct sockaddr *)&server_address, addrlen);
    }
    
    recv(client_fd, message, BUFFER_SIZE, 0);
    xor_key_encryption_decryption(message, strlen(message));
    printf("Server reply: %s\n", message);

    close(client_fd);
    return 0;

}

void xor_key_encryption_decryption(char *message, size_t length) {
    for (size_t i = 0; i < length; i++) {
        message[i] ^= KEY;
    }
}

void reverse_string(char *message, size_t length) {
    for (size_t i = 0; i < length / 2; i++) {
        char tmp = message[i];
        message[i] = message[length - i - 1];
        message[length - i - 1] = tmp;
    }
}