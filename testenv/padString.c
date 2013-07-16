#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int uppercase(char *);

void main(int argc, char **argv){
  char *dir_buff = (char*)malloc(sizeof(char)*11);
  char *fname = (char*)malloc(sizeof(char)*8);
  char *fext = (char*)malloc(sizeof(char)*3);
  char *padding = (char*)malloc(sizeof(char)*11);
  strncpy(padding, "           ", 11);
  int offset;
  int i=0, j;
  char *remove = ".";
  char * token = (char*)malloc(sizeof(char)*11);
  if (strlen(argv[1]) > 12){
	printf("directory name is too large.\n");
	exit(0);
  }
  else{
	printf("length of directory name: %d\n", strlen(argv[1]));
	i = strcspn(argv[1], remove);
	token = strtok(argv[1], ".");
	strncpy(fname, token, strlen(token));
	token = strtok(NULL, ".");
	strncpy(fext, token, strlen(token));
	offset = 11-strlen(fname)-strlen(fext);
	printf("padded with %d spaces\n", offset);
	snprintf(dir_buff, 12, "%s%*.*s%s", fname, offset, offset, padding, fext);
	offset = 0;
	uppercase(dir_buff);
	printf("[%s]\n", fname);
	printf("[%s]\n", fext);
	printf("[%s]\n", dir_buff);
  }
}

int uppercase(char *sPtr){
  while (*sPtr != '\0'){
	*sPtr = toupper((unsigned char) *sPtr);
	++sPtr;
  }
}
