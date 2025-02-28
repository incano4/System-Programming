#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// #define MAX_NUMBERS 100

// void heapify(int *arr, int n, int i, bool asc_flag);
// void heap_sort(int *arr, int n, bool asc_flag);
// void printArray(int *arr, int n);
// void readfrom_file(const char *filename, int *arr, int *size);
// void parse_cli(int argc, char *argv[], int *arr, int *size, bool *asc_flag, bool *from_file, char **file_path);
#include "main.h"





void heapify(int *arr, int n, int i, bool asc_flag) {
    int root = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;


    // n - размер. находим наибольший элемент среди тройки эл-в

    // Compare left child
    if (left < n && ((asc_flag && arr[left] > arr[root]) || (!asc_flag && arr[left] < arr[root]))) {
        root = left;
    }

    // Compare right child
    if (right < n && ((asc_flag && arr[right] > arr[root]) || (!asc_flag && arr[right] < arr[root]))) {
        root = right;
    }

    // If root is not smaller/larger
    if (root != i) {
        int temp = arr[i];
        arr[i] = arr[root];
        arr[root] = temp;

        // Recursively heapify the affected sub-tree
        heapify(arr, n, root, asc_flag);
    }
}

void heap_sort(int *arr, int n, bool asc_flag) {
    // i - старшая вершина с потомками
    // тут строим сортирующее дерево
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i, asc_flag);
    }

    // сорт. элементы, постепенно искл. их по одному
    for (int i = n - 1; i > 0; i--) {
        // mv root to end
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        // heapify on reduced heap
        heapify(arr, i, 0, asc_flag);
    }
}








void print_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


void read_from_file(const char *filename, int **arr, int *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);

        // perror("Failed to open file");
        exit(1);
    }

    int count = 0;
    int value;
    while (fscanf(file, "%d", &value) == 1) {
        count++;
    }

        // printf("count = %d \n", count);

    rewind(file);

    *arr = malloc(count * sizeof(int));
    if (!*arr) {
        fprintf(stderr, "Failed to allocate memory \n");
        fclose(file);
        exit(1);
    }

    for ((*size) = 0; (*size) < count; (*size)++)
        fscanf(file, "%d", &(*arr)[(*size)]);

    fclose(file);
}





// void read_from_file(const char *filename, int **arr, int *size) {
//     FILE *file = fopen(filename, "r");
//     if (!file) {
//         fprintf(stderr, "Failed to open file: %s\n", filename);

//         // perror("Failed to open file");
//         exit(1);
//     }

//     int i;
//     while (fscanf(file, "%d", &i)) (*size)++;

//     rewind(file);

//     *arr = malloc((*size) * sizeof(int));
//     if (!*arr) {
//         fprintf(stderr, "Failed to allocate memory \n");
//         fclose(file);
//         exit(1);
//     }

//     for (i = 0; i < (*size); i++)
//         fscanf(file, "%d", &(*arr[i]);

//     fclose(file);
// }


void parse_cli(int argc, char *argv[], int **arr, int *size, bool *asc_flag, bool *from_file, char **file_path) {
    *asc_flag = true;
    *from_file = false;
    int count = 0;



    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--min") == 0 || strcmp(argv[i], "--max") == 0 || strcmp(argv[i], "-f") == 0) {
            if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
                i++; // Skip the file path argument
            }
        } else {
            count++;
        }
    }


    (*arr) = (int*)malloc(count * sizeof(int));

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--max") == 0) {
            *asc_flag = true;
        } else if (strcmp(argv[i], "--min") == 0) {
            *asc_flag = false;
        } else if (strcmp(argv[i], "-f") == 0) {
            if ((i + 1 < argc) && (count == 0)) {
                *from_file = true;
                *file_path = argv[++i];
            } else if (count != 0){
                fprintf(stderr, "Usage: [--min | --max] [-f <file> | numbers...]\n");
                exit(1);
            } else {
                fprintf(stderr, "Error: Missing file path after -f\n");
                exit(1);
            }
        } else {
            (*arr)[(*size)++] = atoi(argv[i]);
            // arr[(*size)++] = atoi(argv[i]);


        }
    }
}

int main(int argc, char *argv[]) {
    // int arr[MAX_NUMBERS];
    int *arr;

    // подумать, как лучше определять массив. будто бы лучше с помощью malloc
    int size = 0;
    bool asc_flag;
    bool from_file;
    char *file_path;


    // если ввели только 2 слова в консоль, то что-то определенно не так, выводим справку по использованию проги
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [--min | --max] [-f <file> | numbers...]\n", argv[0]);
        return 1;
    }

// -------------------------------------------------------------------
    // если видим ключ -f, то автоматически уходим в обработку файла, несмотря на то, что там еще могут быть цифры в консоли
    // надо это пофиксить

    // блен еще флаги будто наоборот себя ведут, надо пофиксить
// -------------------------------------------------------------------


    // разбираем, что прилетает из командной строки
    parse_cli(argc, argv, &arr, &size, &asc_flag, &from_file, &file_path);

    if (from_file) {
        read_from_file(file_path, &arr, &size);
            // print_array(arr, size);

    }

    heap_sort(arr, size, asc_flag);

    print_array(arr, size);

    free(arr);
    return 0;
}
