#define NAME_MAX 255
#define BUFFER_SIZE 4096
// #define BUFFER_SIZE 5    // test

void create_file(char const *file_path);
void delete_file(char const *file_path);
char path_validation(char const *file_path);
void check_max_length(const char *file_path);
void check_buffer_overflow(const char *file_path);