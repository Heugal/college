#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

#define MAXFSIZE 16320

BOOT_SECT _boot;
FAT_SECT _FAT;

ifp fsRead(char *buff, size_t size, int num, ifp disk){
  if (size*num >= MAXFSIZE){
	printf("Cannot read memory; file too large - maximum file size is 16384KB.\n");
	return disk;
  }
  
  
  
  return disk;
}
