#define MAX_NUMBERS 100

void heapify(int *arr, int n, int i, bool ascending);
void heap_sort(int *arr, int n, bool ascending);
void print_array(int *arr, int n);
void read_from_file(const char *filename, int **arr, int *size);
void parse_cli(int argc, char *argv[], int **arr, int *size, bool *asc_flag, bool *from_file, char **file_path);