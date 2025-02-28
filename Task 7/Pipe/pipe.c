#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define FIFO_PATH "/tmp/named_pipe"
#define BUFFER_SIZE 4096

void unnamed_pipe();
void named_pipe();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s -u (unnamed) | -n (named)\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-u") == 0) {
        unnamed_pipe();
    } else if (strcmp(argv[1], "-n") == 0) {
        named_pipe();
    } else {
        fprintf(stderr, "Invalid option. Use -u for unnamed or -n for named pipe.\n");
        return 1;
    }
    return 0;
}


void unnamed_pipe() {
    int fd[2];  // fd[0] = r, fd[1] = w. определены вызовами pipe
    char buffer[BUFFER_SIZE];
    const char *message = "Msg from unnamed pipe";

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        close(fd[1]);  // Закрываем запись
        read(fd[0], buffer, BUFFER_SIZE);
        close(fd[0]);
        printf("Child received: %s\n", buffer);
        exit(0);
    } else {
        close(fd[0]);  
        write(fd[1], message, strlen(message) + 1);
        close(fd[1]);
        wait(NULL); // позволяет дождаться завершения хотя бы одного child proc чтобы тот не стал процессом-зомби
    }
}


void named_pipe() {
    char buffer[BUFFER_SIZE];
    const char *message = "Msg from named pipe";

    if (mkfifo(FIFO_PATH, 0666) == -1) { // созд. FIFO
        perror("mkfifo");
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // child proc
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd == -1) {
            perror("open (child)");
            exit(1);
        }
        read(fd, buffer, BUFFER_SIZE);
        close(fd);
        printf("Child received: %s\n", buffer);
        unlink(FIFO_PATH);
        exit(0);
    } else {  // PP
        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd == -1) {
            perror("open (parent)");
            exit(1);
        }
        write(fd, message, strlen(message) + 1); // +1 на конец строки
        close(fd);
        wait(NULL); // для завершения child proc
    }
}
  