#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void signal_handler(int signal_number) {
    if (signal_number == SIGCHLD) {
        return; // ignore SIGCHLD
    }

    const char *signal_name = strsignal(signal_number); // получили строку с именем сигнала
    if (signal_name != NULL) {
        printf("Caught signal %d (%s)\n", signal_number, signal_name);
    } else {
        printf("Caught signal %d (Unknown)\n", signal_number);
    }
}

int main() {

    struct sigaction sa;
    sa.sa_handler = signal_handler; 
    sigemptyset(&sa.sa_mask); // маска (набор сигналов), которые будут блокироваться во время выполнения обработчика
    // зануляем, чтобы никакие сигналы не блокировались
    sa.sa_flags = 0; // флаги, указывающие на особенности обработки сигнала. 0 = флаги не используем

    // trap signal
    for (int i = 1; i <= 64; i++) {
        sigaction(i, &sa, NULL);
    }

    printf("Process running with PID: %d\n", getpid());

    while (1) {
        sleep(1);
    }

    return 0;
}
