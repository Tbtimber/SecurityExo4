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


#define FILE_NAME "login.log"
#define LINE_REGEX "[0-9]{2}-[0-9]{2}-[0-9]{4} [0-9]{2}:[0-9]{2};[a-zA-Z]+;[a-zA-Z0-9]+;[0-9]+"
#define BUFFER_SIZE 128


typedef struct {
	char *name;
	int conectionNbr;
} user;

//Mail du prof : anthony.dechy@advens.fr

int check_file();
int readFiles(int fd);
int check_line(char *line);
int parse_line(char *line, user *newUser);
user  *add_to_LOG(user userTested, user *users, int *usersSize);
void display_userBank(user *bank, int size);

void display_userBank(user *bank, int size) {
	if(bank != NULL) {
		for(int i=0; i<size; i++) {
			printf("%s:%d\n", bank[i].name,bank[i].conectionNbr);
		}
	}
}

user *add_to_LOG(user userTested, user *users, int *usersSize) {
	int found = 0;
	int i = 0;
	while (i < *usersSize && found == 0 && users != NULL) {
		if(strcmp(userTested.name, users[i].name) == 0) {
			found = 1;
			users[i].conectionNbr++;
		}
		i++;
	}
	if(found == 0) {
		users = realloc(users, ((*usersSize) + 1)*sizeof(user));
		if(users != NULL) {
			users[*usersSize].name = userTested.name;
			users[*usersSize].conectionNbr = 1;
			(*usersSize)++;
			return users;
		} else {
			*usersSize = 0;
			return NULL;
		}
	}
	return users;
}
int check_line(char *line){
  regex_t regex;
  int reti;

  reti = regcomp(&regex, LINE_REGEX, 0);

  if(reti){
    fprintf(stderr, "Regex not valid\n");
    regfree(&regex);
    printf("return 0\n");
    return 0;
  }

  reti = regexec(&regex, line, 0, NULL, 0);
   if (reti == 0){
	   printf("return 1\n");
     return 1;
   }
   regfree(&regex);
  // printf("Not Match\n");
   return 1;
}

int parse_line(char *line, user *newUser) {
  int i = 0;
  char *token = strtok(line, ";");
  newUser->conectionNbr = 1;
  while (token) {
    switch (i) {
      case 1:
        newUser->name = token;
        break;
      default:
        break;
    }
    token = strtok(NULL, ";");
    if(token) {
      i++;
    }
  }
  return 0;
}



int readFiles(int fd) {
	char buf[BUFFER_SIZE];
	int c;
	int i = 0;
	user us= {"default",0};
	int nbUser = 0;
	user *userBank = NULL;
	//use getchar and check end of line !
	if (dup2(fd, STDIN_FILENO) < 0) {
		return -1;
	}
	while (((c = getchar()) != EOF) && !feof(stdin) && !ferror(stdin)) {
		if(c == '\n') {
			buf[i] = '\0';
			//printf("%s", buf);
			if(check_line(buf)) {
				parse_line(buf,&us);
				userBank = add_to_LOG(us, userBank, &nbUser);
				//printf("Name : %s, connection : %d\n", us.name, us.conectionNbr);
			}
			i = 0;
		}
		if(i < BUFSIZ-1) {
			buf[i++] = c;
		}
	}

	display_userBank(userBank, nbUser);
	return -1;
}

int check_file() {
	struct stat sb;
	return stat(FILE_NAME, &sb) == 0 && S_ISREG(sb.st_mode);
}

int main(void) {
	if (!check_file()) {
		printf("FILE_NAME is not a file name !");
		return EXIT_FAILURE;
	}

	int fd = open(FILE_NAME, O_RDONLY);

	if (fd < 0) {
		return EXIT_FAILURE;
	}
	readFiles(fd);

	if (close(fd) < 0) {
		printf("Close file failed !\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
