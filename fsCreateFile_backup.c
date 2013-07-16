#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

BOOT_SECT _boot;
FAT_SECT _FAT;

typedef struct{
  unsigned char DIR_Name[11];
  unsigned char DIR_Attr;
  unsigned char DIR_NTRes;
  unsigned char DIR_CrtTimeTenth;
  unsigned short DIR_CrtTime;
  unsigned short DIR_CrtDate;
  unsigned short DIR_LstAccDate;
  unsigned short DIR_FstClusHI;
  unsigned short DIR_WrtTime;
  unsigned short DIR_WrtDate;
  unsigned short DIR_FstClusLO;
  unsigned int DIR_FileSize;
}DIR_ENT;

unsigned short findFree(ifp);

ifp fsCreateFile(ifp disk, char* fileName){
  DIR_ENT _dir;

  unsigned char *file_buff = (char*)malloc(sizeof(char)*11);
  unsigned char *disk_buff = (char*)malloc(sizeof(char)*11);
  char *padding = (char*)malloc(sizeof(char)*11);
  strncpy(padding, "           ", 11);
  unsigned int i,j,c;
  int isRoot = 0;
  unsigned short 

  return disk;
}
