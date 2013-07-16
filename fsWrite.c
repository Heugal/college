#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

#define MAXFSIZE 16320

BOOT_SECT _boot;
FAT_SECT _FAT;

ifp fsWrite(char *buff, size_t size, int num, ifp disk){
  if (size*num >= MAXFSIZE){
	printf("Cannot write file; file too large - maximum file size is 16384 bytes.\n");
	return disk;
  }
  
  
  
  return disk;
}
