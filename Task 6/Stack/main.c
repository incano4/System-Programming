#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char* get_dynamic_string(char c) {
    int l = 0;
    int size = 10;
    char *string = (char*) malloc(size * sizeof(char));
    char ch;
    while ((ch = getchar()) != c) {
        string[(l)++] = ch;
        if (l >= size) {
            size *= 2;
            string = (char*) realloc(string, size * sizeof(char));
        }
    }
    string[l] = '\0';
    return string;
}

// одна ячейка стека
typedef struct n {
    char* login;
    char* password;
    struct n* next;
} node;

typedef struct {  
    node* stack_pt;
    int size;
} stack;

node* create_node (char sym) {
    node* new_node = (node*)malloc(sizeof(node));
    printf("Input login: ");
    new_node->login = get_dynamic_string(sym);
    printf("Input password: ");
    new_node->password = get_dynamic_string(sym);
    new_node->next = NULL;
    return new_node;
}

stack* createStack(void) {
    stack* new_stack = (stack*)malloc(sizeof(stack));
    new_stack->stack_pt = NULL;
    new_stack->size = 0;
    return new_stack;
}

void stackPush(stack* stack, char val) {
    node* tmp = create_node(val);
    tmp->next = stack->stack_pt;
    stack->stack_pt = tmp;
    ++stack->size;
}

void stackPop(stack* stack, char** login, char** password) {
    if (!stack->size){
        printf("Error:stack is empty\n");
        (*login) = NULL;
        (*password) = NULL;
        return;
    }
    (*login) = stack->stack_pt->login;
    (*password) = stack->stack_pt->password;
    node* tmp = stack->stack_pt;
    stack->stack_pt = stack->stack_pt->next;
    free(tmp);
    --stack->size;
}

bool stackIsEmpty(stack* stack) {
    return (stack->size == 0);
}

// bool stackSearch(stack* stack, char* str) {
//     if (stack->size) {
//         node* curNode = stack->stack_pt;
//         while (curNode != NULL) {
//             if (!strcmp(str, curNode->login)) return curNode;
//             curNode = curNode->next;
//         }
//         return NULL;
//     } else return NULL;
// }

char* stackSearch(stack* stack, char* search_login) {
    char* login;
    char* password;
    while(!stackIsEmpty(stack)) {
        stackPop(stack, &login, &password);
        if (strcmp(login, search_login) == 0) {
            //puts("Login is exist\n");
            return password;
        }
            
    }  
    return NULL;
}

int stackSize(stack* stack) {
    return stack->size;
}

void freestack(stack* stack) {
    if (stack->size) {
        node* curNode = stack->stack_pt;
        while (curNode != NULL) {
            node* delNode = curNode;
            curNode = curNode->next;
            free(delNode->login);
            free(delNode->password);
            free(delNode);
        }
        free(curNode);
    } 
    free(stack);
}


// end realization of stack

void help(void) {
    printf("Commands for work with stack:\n1 - push in stack login/password.\n2 - pop from stack login/password.\n3 - size of stack.\n4 - stack is empty.\n5 - search in stack.\n0 - exit from programm.\n");
}

int main(void) {
    stack* main_stack = createStack();
    help();
    int input = 1;
    while(input){
        printf("Input commands: ");
        scanf("%d",&input);
        switch(input){
            case 0: break;
            case 1:
                getchar();
                puts("Input account data");
                stackPush(main_stack, '\n');
                break;
            case 2:
                char* login;
                char* password;
                stackPop(main_stack, &login, &password);
                printf("Login from stack: %s\n", login);
                printf("Password from stack: %s\n", password);
                free(login);
                free(password);
                break;
            case 3:
                printf("Size of stack: %d\n",stackSize(main_stack));
                break;
            case 4:
                printf("stack is ");
                if (!stackIsEmpty(main_stack)){
                    puts("not empty");
                } else {
                    puts("empty");
                }
                break;
            case 5:
                // убираем /n 
                getchar();
                printf("Input search login: ");
                char* search_login = get_dynamic_string('\n');
                char* search_pass = stackSearch(main_stack, search_login);
                if (search_pass == NULL) {
                    printf("Search login \"%s\" does not exist\n", search_login);
                } else {
                    printf("Login: \"%s\", Password: \"%s\"\n", search_login, search_pass);
                    free(search_pass);
                }
                free(search_login);
                break;
            default:
                help();
        }
    }
    return 0;
}