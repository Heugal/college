#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

OPEN_LIST _olist;

// Removes the file from the open file list if the file is present there
// If there are no open files or the filename is too long, return with error status
int fsClose(char *file){
  unsigned char *file_buff = (char*)malloc(sizeof(char)*13);

  if (_olist.numOpen == 0){
	printf("No files presently open.\n");
	free(file_buff);
	file_buff = NULL;
	return -1;
  }
 
  if (strlen(file) > 12){
	printf("File %s cannot be located.\n", file);
	free(file_buff);
	file_buff = NULL;
	return -1;
  }
  else{
	strncpy(file_buff, file, strlen(file)+1);
	if ((file_buff = fnameCreate(file_buff)) == NULL){
	  printf("Invalid file name.\n");
	  free(file_buff);
	  file_buff = NULL;
	  return -1;
    }
  }

  int numOpen = _olist.numOpen;
  int i, j;
  for (j=0; j < 32; j++){
	if (strncmp(file_buff, _olist.openFiles[j], 11) == 0){
	  memset(_olist.openFiles[j], '\0', sizeof(_olist.openFiles[j]));
	  _olist.filePos[j] = 0;
	  _olist.numOpen--;
	  break;
	}
  }
  
  printf("Closed file: [%s] (if blank, fclose was successful)\n", _olist.openFiles[i]);
  printf("Closed file position: %d\n", _olist.filePos[i]);
  printf("Number of open files: %d\n", _olist.numOpen);

  free(file_buff);
  file_buff = NULL;
  return 1;
}
