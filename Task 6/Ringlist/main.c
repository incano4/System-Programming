#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char* dynstring(char c) {
    int l = 0, size = 1;
    char *string = (char*) malloc(size * sizeof(char));
    char ch;
    while ( (ch = getchar()) != c) {
        string[l++] = ch;
        if (l >= size) {
            size *= 2;
            string = (char*) realloc(string, size * sizeof(char));
        }
    }
    string[l] = '\0';
    return string;
}

// start realization of ringlist
typedef struct n{
    char* login;
    char* pass;
    struct n* next;
    struct n* prev;
} node;

typedef struct{  
    node* cur_node;
    int size;
} ringlist;

node* create_node (char sym){
    node* new_node = (node*)malloc(sizeof(node));
    printf("Input login: ");
    new_node->login = dynstring(sym);
    printf("Input password: ");
    new_node->pass = dynstring(sym);
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
}

ringlist* createringlist(void){
    ringlist* new_ringlist = (ringlist*)malloc(sizeof(ringlist));
    new_ringlist->cur_node = NULL;
    new_ringlist->size = 0;
    return new_ringlist;
}

void ringlistPush(char sym, ringlist* ringlist){
    node* tmp = create_node(sym);
    // в случ. создания 1й ноды она указывает сама на себя
    if(!ringlist->size){
        ringlist->cur_node = tmp;
        ringlist->cur_node->next = ringlist->cur_node->prev = tmp;
    } else {
        tmp->next = ringlist->cur_node->next;
        tmp->prev = ringlist->cur_node;
        ringlist->cur_node->next->prev = tmp;
        ringlist->cur_node->next = tmp;
    }
    ++ringlist->size;
}

void ringlistPop(ringlist* ringlist, char** login, char** pass){
    if (!ringlist->size){
        printf("Error:ringlist is empty\n");
        (*login) = NULL;
        (*pass) = NULL;
        return;
    }
    *(login) = ringlist->cur_node->login;
    *(pass) = ringlist->cur_node->pass;
    node* tmp = ringlist->cur_node;
    if (tmp->next != tmp) {
        tmp->next->prev = tmp->prev;
        tmp->prev->next = tmp->next;
    } else {
        ringlist->cur_node = NULL;
    }
    free(tmp);
    --ringlist->size;
}

bool ringlistIsEmpty(ringlist* ringlist){
    return (ringlist->size == 0);
}

char* ringlistSearch(ringlist* ringlist, char* search_login){
    node* cur_node = ringlist->cur_node;
    do {
        if (!strcmp(search_login, cur_node->login)) return cur_node->pass;
        cur_node = cur_node->next;
    } while (cur_node != ringlist->cur_node);
    return NULL;
}

int ringlistSize(ringlist* ringlist){
    return ringlist->size;
}

void freeringlist(ringlist* ringlist) {
    if (ringlist->size) {
        node* cur_node = ringlist->cur_node;
        while (!ringlistIsEmpty(ringlist)){
            node* new_curnode = ringlist->cur_node->next;
            char* login;
            char* pass;
            ringlistPop(ringlist, &login, &pass);
            free(login);
            free(pass);
            ringlist->cur_node = new_curnode;
        }
    } 
    free(ringlist);
}


// end realization of ringlist

void help(void){
    printf("Commands for work with ringlist:\n1 - push in ringlist passwod + login value.\n2 - pop from ringlist login/password value.\n3 - size of ringlist.\n4 - ringlist is empty.\n5 - find password from login.\n6 - next node.\n7 - previous node.\n0 - exit from programm.\n");
}

int main(void) {
    ringlist* main_ringlist = createringlist();
    help();
    int input = 1;
    while(input){
        printf("Input commands: ");
        scanf("%d",&input);
        switch(input){
            case 1:
                getchar();
                ringlistPush('\n', main_ringlist);
                break;
            case 2:
                node* new_curnode = main_ringlist->cur_node->next;
                char* login;
                char* pass;
                ringlistPop(main_ringlist, &login, &pass);
                if (login != NULL) {
                    printf("Login from ringlist: %s\n", login);
                    printf("Password from ringlist: %s\n", pass);
                }
                free(login);
                free(pass);
                main_ringlist->cur_node = new_curnode;
                break;
            case 3:
                printf("Size of ringlist: %d\n",ringlistSize(main_ringlist));
                break;
            case 4:
                printf("ringlist is ");
                if (!ringlistIsEmpty(main_ringlist)){
                    printf("not empty\n");
                } else {
                    printf("empty\n");
                }
                break;
            case 5:
                getchar();
                printf("Input login: ");
                char* search_login = dynstring('\n');
                char* search_pass = ringlistSearch(main_ringlist, search_login);
                if (search_pass == NULL) {
                    printf("Search login is not exist\n");
                }  else {
		            printf("Password for login \"%s\" is \"%s\"\n", search_login, search_pass);
                }
                break;
            case 6:
                if (ringlistIsEmpty(main_ringlist)) {
                    printf("ERROR: Ringlist is empty\n"); 
                } else {
                    main_ringlist->cur_node = main_ringlist->cur_node->next; 
                    printf("Current login: \"%s\", password: \"%s\"\n", main_ringlist->cur_node->login, main_ringlist->cur_node->pass);
                }
                break;
            case 7:
                if (ringlistIsEmpty(main_ringlist)) {
                    printf("ERROR: Ringlist is empty\n"); 
                } else {
                    main_ringlist->cur_node = main_ringlist->cur_node->prev; 
                    printf("Current login: \"%s\", password: \"%s\"\n", main_ringlist->cur_node->login, main_ringlist->cur_node->pass);
                }
                break;

            case 0:
                freeringlist(main_ringlist);
                return 0;
            default:
                help();
        }
    }
    return 0;
}