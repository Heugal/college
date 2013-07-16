#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

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

BOOT_SECT _boot;
FAT_SECT _FAT;

char *fnameCreate(char *);
int isPrsent(char *);

ifp fsCreateFile(ifp disk, char *fileName){
  DIR_ENT _dir;

  //counter starts at 64 because the first entry is '.' and 2nd entry
  //is '..' for every directory
  unsigned char *file_buff = (char*)malloc(sizeof(char)*13);
  unsigned char *disk_buff = (char*)malloc(sizeof(char)*11);
  unsigned int i,j,c;
  int isRoot = 0;
  unsigned short offset;
  unsigned short clustNum;
  unsigned int pos = ftell(disk);
  unsigned int posinit = pos;
  unsigned int clustPos;
  unsigned char attr;
  unsigned char dirPresent = 0;
  unsigned short firstFree = 0;
  int buff;
  //put the directory name into a buffer so the name can
  //be compared in the first 32 bytes of the entry
  if (strlen(fileName) > 12){
	printf("File name is too long.\n");
    free(file_buff);
    free(disk_buff);
    file_buff = NULL;
	disk_buff = NULL;
	return disk;
  }
  else{
	strncpy(file_buff, fileName, strlen(fileName)+1);
	if ((file_buff = fnameCreate(file_buff)) == NULL){
	  printf("Invalid file name.\n");
	  free(file_buff);
	  free(disk_buff);
	  file_buff = NULL;
	  disk_buff = NULL;
	  return disk;
	}
  }

  //find the position of the current directory. if the current directory
  //is the root directory, position is moved to the first root directory 
  //sector. otherwise the clutser number is set
  if (pos<=(_FAT.FirstDataSector*_boot.BPB_BytsPerSec)){
	isRoot = 1;
	pos = (_FAT.FirstRootDirSecNum*_boot.BPB_BytsPerSec);
  }
  else
	clustNum= (pos-(_FAT.FirstDataSector*_boot.BPB_BytsPerSec))/_FAT.ClustSize;

  unsigned char cluster[_FAT.ClustSize-64];
  for (j=0;j<_FAT.ClustSize-64;j++)
	cluster[j] = 0;

  rewind(disk);
  unsigned int data_start = 0;
  unsigned int root_start = 0;
  if (isRoot == 0){
	data_start = (_FAT.FirstDataSector*_boot.BPB_BytsPerSec)+(clustNum*_FAT.ClustSize);
        clustPos = ftell(disk);
	buff = _FAT.ClustSize;
	i=64;
  }
  else{
	root_start = _FAT.FirstRootDirSecNum*_boot.BPB_BytsPerSec;
	clustNum = 0x0001;
	buff = (_FAT.RootDirSectors*_boot.BPB_BytsPerSec);
	i=0;
  }

  while (i<= buff){
	if (isRoot == 0)
	  fseek(disk, data_start+i, SEEK_SET);
	else
	  fseek(disk, root_start+i, SEEK_SET);
	fread(&attr, sizeof(char), 1, disk);
	if ((attr == 0x00) || (attr == 0xe5))
	  break;
	else{
	  fseek(disk, 10, SEEK_CUR);
	  fread(&attr, sizeof(char), 1, disk);
	  fseek(disk, -12, SEEK_CUR);
	  //if entry is a file
	  if ((attr&0x10) != 0x10){
		fread(disk_buff, strlen(file_buff), 1, disk);
		if (strncmp(file_buff, disk_buff, 11) == 0){
		  dirPresent = 1;
		  break;
		}
	  }
	}
	i += 32;
	if (i == buff){
	  printf("Too many directories present in current directory.\n");
	  free(file_buff);
	  free(disk_buff);
	  file_buff = NULL;
	  disk_buff = NULL;
	  return disk;
	}
	rewind(disk);
  }
  
  if (dirPresent == 0){
	firstFree = findFree(disk);
    if (firstFree == 0x0000){
	  printf("Not enough memory on disk to create directory\n");
	  free(file_buff);
	  free(disk_buff);
	  file_buff = NULL;
	  disk_buff = NULL;
	  return disk;
	}
	fseek(disk, -1, SEEK_CUR);
    fwrite(&firstFree, sizeof(firstFree), 1, disk);
	for(j=0;j<11;j++)
	  _dir.DIR_Name[j] = file_buff[j];
    _dir.DIR_Attr = 0x00;
    _dir.DIR_NTRes = 0x00;
    _dir.DIR_CrtTimeTenth = 0x00;
    _dir.DIR_CrtTime = getTime();
    _dir.DIR_CrtDate = getDate();
    _dir.DIR_LstAccDate = getDate();
    _dir.DIR_FstClusHI = 0x0000;
    _dir.DIR_WrtTime = getTime();
    _dir.DIR_WrtDate = getDate();
    _dir.DIR_FstClusLO = firstFree;
    _dir.DIR_FileSize = 0x00000000;

    rewind(disk);
	if (isRoot == 0)
	  fseek(disk, (_FAT.FirstDataSector*_boot.BPB_BytsPerSec)+(clustNum*_FAT.ClustSize), SEEK_SET);
	else
	  fseek(disk, (_FAT.FirstRootDirSecNum*_boot.BPB_BytsPerSec), SEEK_SET);
	fseek(disk, i, SEEK_CUR);
	fwrite(&_dir, sizeof(_dir), 1, disk);
	rewind(disk);
	fseek(disk, (_FAT.FirstDataSector*_boot.BPB_BytsPerSec)+(firstFree*_FAT.ClustSize), SEEK_SET);

	_dir.DIR_Name[0] = '.';
	for(j=1;j<11;j++)
	  _dir.DIR_Name[j] = ' ';
	_dir.DIR_Attr = 0x10;
	fwrite(&_dir, sizeof(_dir), 1, disk);
        
	_dir.DIR_Name[1] = '.';
	_dir.DIR_FstClusLO = clustNum;
	fwrite(&_dir, sizeof(_dir), 1, disk);
    fwrite(&cluster, sizeof(cluster), 1, disk);
	rewind(disk);
	fseek(disk, posinit, SEEK_SET);
  }
  else{
	printf("File %s already present in current directory.\n", fileName);
    free(file_buff);
	free(disk_buff);
	file_buff = NULL;
	disk_buff = NULL;
	return disk;
  }
  free(file_buff);
  free(disk_buff);
  file_buff = NULL;
  disk_buff = NULL;
  return disk;
}

char *fnameCreate(char *fileName){
  char *fcopy = (char*)malloc(sizeof(char)*13);
  char *dir_buff = (char*)malloc(sizeof(char)*11);
  char *fname = (char*)malloc(sizeof(char)*9);
  char *basecase = "txt";
  char *fext = (char*)malloc(sizeof(char)*4);
  char *padding = "           ";
  int offset, i=0;
  char *remove = ".";
  char *token = (char*)malloc(sizeof(char)*11);
 
  // Checks to see if file system wants to create a directory for "." or ".."
  // which are already created, so it returns null
  if (strncmp(remove, fileName, 1) == 0){
	return NULL;
  }
  strncpy(fcopy, fileName, strlen(fileName)+1);
  i = isPresent(fileName);
  token = strtok(fcopy, ".");
  strncpy(fname, token, strlen(token)+1);
  if (i != 0){
    token = strtok(NULL, ".");
    strncpy(fext, token, strlen(token)+1);
  }
  else{
    strncpy(fext, basecase, strlen(basecase)+1);
  }
  offset = 11-strlen(fname)-strlen(fext);
  snprintf(dir_buff, 12, "%s%*.*s%s", fname, offset, offset, padding, fext);
  uppercase(dir_buff);

  free(fcopy);
  free(fname);
  free(fext);
  fcopy = NULL;
  fname = NULL;
  fext = NULL; 

  return dir_buff;
}

// checks to see if filename contains a '.' indicating if it is a file or a directory
int isPresent(char *sPtr){
  int i=0;
  while (*sPtr != '\0'){
	if (*sPtr == '.')
	  i = 1;
	++sPtr;
  }
  return i;
}
