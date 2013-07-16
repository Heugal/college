#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

PWD _cDir;

void fsPrintDir(void){
  int i;
  for(i;i<256;i++)
	printf("%c", _cDir.currDir[i]);
  printf("\n");
}
