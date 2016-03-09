/*
 ============================================================================
 Name        : TD1_EX4.c
 Author      : Matthieu et Romain
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>
#include <limits.h>


#define FILE_NAME "login.log"
#define LINE_REGEX ".*[[:space:]].*;.*;.*;[[:digit:]]"
#define BUFFER_SIZE 128

typedef struct {
    char *name;
    char *serverName;
    unsigned int conectionNbr;
    unsigned int timespend;
} user;

//Mail du prof : anthony.dechy@advens.fr

int check_file();
int readFiles(int fd);
int check_line(char *line);
int parse_line(char *line, user *newUser);
user *add_to_LOG(user userTested, user *users, int *usersSize);
void display_userBank(user *bank, int size);
int copy_users(user *oldUsers, user *newUsers, int newSize);
void delete_user_bank(user *users, int usersSize);

void display_userBank(user *bank, int size) {
    if (bank != NULL) {
        for (int i = 0; i < size; i++) {
            fprintf(stdout, "%s:%s:%d:%d\n", bank[i].name, bank[i].serverName, bank[i].conectionNbr, bank[i].timespend);
        }
    }
}

user *add_to_LOG(user userTested, user *users, int *usersSize) {
    int found = 0;
    int i = 0;
    size_t sizeName = 0;
    size_t sizeServerName = 0;
    user *tmpUsers = NULL;
    while (i < *usersSize && found == 0 && users != NULL) {
        sizeName = strlen(users[i].name);
        sizeServerName = strlen(users[i].serverName);
        if ((strncmp(userTested.name, users[i].name, sizeName) == 0) && (strncmp(userTested.serverName, users[i].serverName, sizeServerName) == 0)) {
            found = 1;
            users[i].conectionNbr++;
            users[i].timespend += userTested.timespend;
        }
        i++;
    }
    if (found == 0) {
        tmpUsers = malloc(((*usersSize) + 1) * sizeof (user));
        if (tmpUsers != NULL) {
            if ((*usersSize) > 0) {
                copy_users(users, tmpUsers, (*usersSize));
                free(users);
            }
            users = tmpUsers;
            sizeName = strlen(userTested.name);
            sizeServerName = strlen(userTested.serverName);
            users[*usersSize].name = malloc((sizeName + 1) * sizeof (char));
            if ((users[*usersSize].name == NULL) || (strncpy(users[*usersSize].name, userTested.name, sizeName) == NULL)) {
                delete_user_bank(users, *usersSize);
                return NULL;
            }
            users[*usersSize].name[sizeName] = '\0';
            users[*usersSize].conectionNbr = 1;
            users[*usersSize].serverName = malloc((sizeServerName + 1) * sizeof (char));
            if ((users[*usersSize].serverName == NULL) || (strncpy(users[*usersSize].serverName, userTested.serverName, sizeServerName) == NULL)) {
                delete_user_bank(users, *usersSize);
                return NULL;
            }
            users[*usersSize].serverName[sizeServerName] = '\0';
            users[*usersSize].timespend = userTested.timespend;
            (*usersSize)++;
            return users;
        } else {
            *usersSize = 0;
            return NULL;
        }
    }
    return users;
}

int copy_users(user *oldUsers, user *newUsers, int newSize) {
    if ((oldUsers == NULL) || (newUsers == NULL) || (newSize <= 0)) {
        return 0;
    }
    size_t sizeName = 0;
    size_t sizeServerName = 0;
    for (int i = 0; i < newSize; i++) {
        sizeName = strlen(oldUsers[i].name);
        newUsers[i].name = oldUsers[i].name;
        newUsers[i].conectionNbr = oldUsers[i].conectionNbr;
        newUsers[i].serverName = oldUsers[i].serverName;
        newUsers[i].timespend = oldUsers[i].timespend;
    }
    return 1;
}

void delete_user_bank(user *users, int usersSize) {
    for (int i = 0; i < usersSize; i++) {
        free(users[i].name);
        users[i].name = NULL;
        free(users[i].serverName);
        users[i].serverName = NULL;
    }
}

int check_line(char *line) {
    regex_t regex;
    int reti = 0;

    reti = regcomp(&regex, LINE_REGEX, 0);

    if (reti) {
        fprintf(stderr, "Regex not valid\n");
        regfree(&regex);
        return 0;
    }

    reti = regexec(&regex, line, 0, NULL, 0);
    if (reti == 0) {
        regfree(&regex);
        return 1;
    }
    regfree(&regex);
    return 0;
}

int parse_line(char *line, user *newUser) {
    int i = 0;
    int size = 0;
    unsigned long result;
    size = strlen(line);
    if (size <= 0) {
        return 0;
    }
    char *tempLine = calloc(size + 1, sizeof (char));
    strncpy(tempLine, line, size);
    tempLine[size] = '\0';
    char *token = strtok(tempLine, ";");
    if (token == NULL) {
        free(tempLine);
        tempLine = NULL;
        return 0;
    }
    newUser->conectionNbr = 1;
    while (token) {
        switch (i) {
            case 1:
                size = strlen(token);
                if (size <= 0) {
                    free(tempLine);
                    tempLine = NULL;
                    free(token);
                    token = NULL;
                    return 0;
                }
                newUser->name = malloc(size + 1 * sizeof (char));
                if (newUser->name == NULL) {
                    free(tempLine);
                    tempLine = NULL;
                    free(token);
                    token = NULL;
                    return 0;
                }
                if (strncpy(newUser->name, token, size) == NULL) {
                    free(tempLine);
                    tempLine = NULL;
                    free(token);
                    token = NULL;
                    free(newUser->name);
                    newUser->name = NULL;
                    return 0;
                }
                newUser->name[size] = '\0';
                break;
            case 2:
                size = strlen(token);
                if (size <= 0) {
                    free(tempLine);
                    tempLine = NULL;
                    free(token);
                    token = NULL;
                    free(newUser->name);
                    newUser->name = NULL;
                    return 0;
                }
                newUser->serverName = malloc(size + 1 * sizeof (char));
                if (newUser->serverName == NULL) {
                    free(tempLine);
                    tempLine = NULL;
                    free(token);
                    token = NULL;
                    free(newUser->name);
                    newUser->name = NULL;
                    return 0;
                }
                if (strncpy(newUser->serverName, token, size) == NULL) {
                    free(tempLine);
                    tempLine = NULL;
                    free(token);
                    token = NULL;
                    free(newUser->serverName);
                    newUser->serverName = NULL;
                    free(newUser->name);
                    newUser->name = NULL;
                    return 0;
                }
                newUser->serverName[size] = '\0';
                break;
            case 3:
                result = strtoul(token, NULL, 10);
                if (result > INT_MAX) {
                    free(tempLine);
                    tempLine = NULL;
                    free(newUser->serverName);
                    newUser->serverName = NULL;
                    free(newUser->name);
                    newUser->name = NULL;
                    return 0;
                }
                newUser->timespend = (int) result;
                break;
            default:
                break;
        }
        token = strtok(NULL, ";");
        if (token == NULL) {
            free(tempLine);
            tempLine = NULL;
        }
        if (token) {
            i++;
        }
    }
    free(tempLine);
    tempLine = NULL;
    free(token);
    token = NULL;
    return 1;
}

int readFiles(int fd) {
    char buf[BUFFER_SIZE];
    int c = 0;
    int i = 0;
    user us = {"default", "default", 0, 0};
    int nbUser = 0;
    int lineTreated = 0;
    user *userBank = NULL;
    if (dup2(fd, STDIN_FILENO) < 0) {
        return 0;
    }
    while ((c = getchar()) && !ferror(stdin) && !lineTreated) {
        if ((c == '\n') || (c == EOF)) {
            buf[i] = '\0';
            if (check_line(buf)) {
                if (parse_line(buf, &us)) {
                    userBank = add_to_LOG(us, userBank, &nbUser);
                    if (userBank == NULL) {
                        free(us.name);
                        free(us.serverName);
                        us.name = NULL;
                        us.serverName = NULL;
                        return 0;
                    } else {
                        free(us.name);
                        us.name = NULL;
                        free(us.serverName);
                        us.serverName = NULL;
                    }
                } else {
                    fprintf(stderr, "Parse failed\n");
                }
            } else {
                fprintf(stderr, "Line not macth with pattern \n");
            }
            i = 0;
            if (c == EOF) {
                lineTreated = 1;
            }
        }
        if (i < BUFFER_SIZE - 1) {
            buf[i++] = c;
        }
    }

    display_userBank(userBank, nbUser);
    delete_user_bank(userBank, nbUser);
    free(userBank);
    userBank = NULL;
    return 1;
}

int check_file() {
    struct stat sb;
    return stat(FILE_NAME, &sb) == 0 && S_ISREG(sb.st_mode);
}

int main(void) {
    if (!check_file()) {
        fprintf(stderr, "FILE_NAME is not a file name !");
        return EXIT_FAILURE;
    }

    int fd = open(FILE_NAME, O_RDONLY);

    if (fd < 0) {
        return EXIT_FAILURE;
    }
    if (!readFiles(fd)) {
        fprintf(stderr, "Process failed !! \n");
    }

    if (close(fd) < 0) {
        fprintf(stderr, "Close file failed !\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
