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

unsigned short findFree(ifp);
unsigned int uppercase(char *);

ifp fsCreateDir(ifp disk, char *dirName){
  DIR_ENT _dir;

  //counter starts at 64 because the first entry is '.' and 2nd entry
  //is '..' for every directory
  unsigned char *dir_buff = (char*)malloc(sizeof(char)*11);
  unsigned char *disk_buff = (char*)malloc(sizeof(char)*11);
  char *padding = "           ";
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
  char *check;
  //put the directory name into a buffer so the name can
  //be compared in the first 32 bytes of the entry
  if ((strlen(dirName) > 11) || (check = strchr(dirName, '.') != NULL)){
	printf("Invalid directory name.\n");
	free(dir_buff);
	free(disk_buff);
	dir_buff = NULL;
	disk_buff = NULL;
	return disk;
  }
  else{
	offset = 11 - strlen(dirName);
	snprintf(dir_buff, 12, "%s%*.*s", dirName, offset, offset, padding);
	uppercase(dir_buff);
	offset = 0;
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
	  //if entry is a directory
	  if ((attr&0x10) == 0x10){
		fread(disk_buff, sizeof(char), strlen(dir_buff), disk);
		if (strncmp(dir_buff, disk_buff, strlen(dir_buff)) == 0){
		  dirPresent = 1;
		  break;
		}
	  }
	}
	i += 32;
	if (i == buff){
	  printf("Too many directories present in current directory.\n");
	  rewind(disk);
	  fseek(disk, posinit, SEEK_SET);
	  free(dir_buff);
	  free(disk_buff);
	  dir_buff = NULL;
	  disk_buff = NULL;
	  return disk;
	}
	rewind(disk);
  }
  
  if (dirPresent == 0){
	firstFree = findFree(disk);
    if (firstFree == 0x0000){
	  printf("Not enough memory on disk to create directory\n");
	  free(dir_buff);
	  free(disk_buff);
	  dir_buff = NULL;
	  disk_buff = NULL;
	  return disk;
	}
	fseek(disk, -1, SEEK_CUR);
    fwrite(&firstFree, sizeof(firstFree), 1, disk);
	for(j=0;j<strlen(dir_buff);j++)
	  _dir.DIR_Name[j] = dir_buff[j];
	for(j;j<11;j++)
	  _dir.DIR_Name[j] = ' ';
    _dir.DIR_Attr = 0x10;
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
	fwrite(&_dir, sizeof(_dir), 1, disk);
        
	_dir.DIR_Name[1] = '.';
	_dir.DIR_FstClusLO = clustNum;
	fwrite(&_dir, sizeof(_dir), 1, disk);
    fwrite(&cluster, sizeof(cluster), 1, disk);
	rewind(disk);
	fseek(disk, posinit, SEEK_SET);
  }
  else{
	printf("Directory %s already present in current directory.\n", dirName);
	free(dir_buff);
	free(disk_buff);
	dir_buff = NULL;
	disk_buff = NULL;
	return disk;
  }
  free(dir_buff);
  free(disk_buff);
  dir_buff = NULL;
  disk_buff = NULL;
  return disk;
}

unsigned short findFree(ifp disk){
  unsigned short fat;
  unsigned int i=3;
  unsigned short pos = 0x0000;
  rewind(disk);
  fseek(disk, _boot.BPB_BytsPerSec+4, SEEK_SET);
  while (i<_FAT.CountofClusters){
    fat = fgetc(disk);
    if (fat == 0x0000){
	  pos = ftell(disk);
	  pos = (pos-_boot.BPB_BytsPerSec)/2;
	  return pos;
    }
    else{
	  i++;
	  fseek(disk, 1, SEEK_CUR);
    }
  }

  return pos;
}

unsigned int uppercase(char *sPtr){
  while (*sPtr != '\0'){
	*sPtr = toupper((unsigned char) *sPtr);
	++sPtr;
  }
}
