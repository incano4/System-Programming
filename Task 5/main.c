#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "main.h"


void check_buffer_overflow(const char *file_path) {
    char buffer[BUFFER_SIZE];
    if (strlen(file_path) >= BUFFER_SIZE) {
        // printf("Error: path is too long. Buffer owerflow.\n");
        fprintf(stderr, "Error: path is too long. Buffer owerflow.\n");
        exit(7);
    }

    // Копируем путь в буфер (длина проверена)
    // Имеет смысл, если бы мы предпочли статический буфер, а не выделять память динамически)
    strncpy(buffer, file_path, BUFFER_SIZE - 1);
    buffer[BUFFER_SIZE - 1] = '\0';
}


void check_max_length(char const *file_path) {
    if (strlen(file_path) >= NAME_MAX) {
        fprintf(stderr, "Error: name too long\n");
        exit(36);
    }
}


char path_validation(char const *file_path) {
    size_t length = strlen(file_path);
    char last_char = file_path[length - 1];

    switch(last_char) {
    case '/':
        // printf("Unable to create file %s. Filename shouldn't ends with '/'\n", file_path);
        fprintf(stderr, "Unable to create file %s. Filename shouldn't ends with '/'\n", file_path);
        exit(21);
        break;

    default: // по умолчанию будем запрещать использование спецсимволов
        const char *invalid_chars = "?~*&?[]";
        if (strpbrk(file_path, invalid_chars)) {
            // printf("Invalid characters in path: %s\n", file_path);
            fprintf(stderr, "Invalid characters in path: %s\n", file_path);
            exit(1);
        } else {
            create_file(file_path);
        }
        break;
    }
}


void create_file(char const *file_path) {
    // File Pointer declared
    FILE *file;
    file = fopen(file_path, "w");

    if (file) {
        fclose(file);
        printf("File %s created\n", file_path);
    } else {
        // printf("An error occurred while creating a file: %s\n", file_path);
        fprintf(stderr, "An error occurred while creating a file: %s\n", file_path);
        exit(132);
    }
}


void delete_file(char const *file_path) {
    if (remove(file_path) == 0) {
        printf("The file was deleted successfully.\n");
    } else {
        // printf("The file was not deleted.\n");
        fprintf(stderr, "File was not deleted: %s\n", file_path);
        exit(2);
    }
}


int main(int argc, char *argv[]) {
    int opt;
    int create_flag = 0, delete_flag = 0;
    char *file_path = NULL;


// argc и argv являются счетчиком и массивом аргументов, которые передаются функции main() при запуске программы.
// Элемент argv, начинающийся с "-" (и не являющийся "-" или "--"), считается опцией.
// Больше информации: https://www.opennet.ru/man.shtml?topic=getopt&category=3&russian=0

// c: - ключ -c требует аргумента (-c path).
// cd: - ключ -c не требует аргумента, ключ -d требует
// c:d: - ключи -c и -d требуют аргумента.
// оставляем 'cd', тк иначе optind будет принимать неверное значение, тк будем считать, 
// что первый аргумент мы уже обработали

    while((opt = getopt(argc, argv, "cd")) != -1) {

        switch(opt) {
            case 'c':
                create_flag = 1;
                break;
            case 'd':
                delete_flag = 1; 
                break;
            default:  
                printf("unknown option \n");
                break;
        }
    }

// / 
    if ((create_flag || delete_flag) && argc == 2) {
        // printf("No files are specified\n");
        fprintf(stderr, "No files are specified\n");
        return 22;
    } 
    else if (!create_flag && !delete_flag) {
        // printf("Action is not specified. Use '-c' or '-d' option.\n");
        fprintf(stderr, "Action is not specified. Use '-c' or '-d' option.\n");
        return 1;
    }

    if (create_flag && delete_flag) {
        // printf("You can't use 'delete' and 'create' option at same time \n");
        fprintf(stderr, "You can't use 'delete' and 'create' option at same time \n");
        return 1;
    }

    // optind - первый элемент массива argv
    for (int i = optind; i < argc; i++) {
        check_buffer_overflow(argv[i]);
        char *file_path = malloc(strlen(argv[i]) + 1); // +1 для добавления символа конца строки
        // попробовать генерить с random
        // mal 2048*char_size

        strcpy(file_path, argv[i]); // сохраняем путь в выделенную память
        check_max_length(file_path);
        printf("Обрабатывается файл: %s\n", file_path);
        
        if (create_flag) {
            printf("Creating files.. \n");
            path_validation(file_path);
            free(file_path);
        }

        else if (delete_flag) {
            const char *invalid_chars = "~&?[]";
            if (strpbrk(file_path, invalid_chars)) {
                // printf("Invalid characters in path: %s\n", file_path);
                fprintf(stderr, "Invalid characters in path: %s\n", file_path);
                return 1;
                free(file_path);
            } else {
                printf("Removing files.. \n");
                delete_file(file_path);
                free(file_path);
            }
        }
    }
}