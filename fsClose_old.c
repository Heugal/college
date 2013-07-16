#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

BOOT_SECT _boot;
FAT_SECT _FAT;
OPEN_LIST _olist;

ifp fsClose(ifp disk, char *file){
  unsigned char *file_buff = (char*)malloc(sizeof(char)*13);
  unsigned char *disk_buff = (char*)malloc(sizeof(char)*11);
  unsigned int i,j,c;
  unsigned short offset; 
  unsigned short clustNum;
  unsigned int pos = ftell(disk);
  unsigned int posinit = pos;
  unsigned int clustPos;
  unsigned char attr;
  int isRoot = 0;
  unsigned char dirPresent = 0;
  unsigned short firstFree = 0;
  int buff;

  if (_olist.numOpen == 0){
	printf("No files presently open.\n");
	free(file_buff);
	free(disk_buff);
	file_buff = NULL;
	disk_buff = NULL;
	return disk;
  }
  
  if (strlen(file) > 12){
	printf("File %s cannot be located.\n", file);
	free(file_buff);
	free(disk_buff);
	file_buff = NULL;
	disk_buff = NULL;
	return disk;
  }
  else{
	strncpy(file_buff, file, strlen(file));
	if ((file_buff = fnameCreate(file_buff)) == NULL){
	  printf("Invalid file name.\n");
	  free(file_buff);
	  free(disk_buff);
	  file_buff = NULL;
	  disk_buff = NULL;
	  return disk;
	}
  }

  if (pos <= _FAT.FirstDataSector*_boot.BPB_BytsPerSec){
	isRoot = 1;
	pos = (_FAT.FirstRootDirSecNum*_boot.BPB_BytsPerSec);
  }
  else
	clustNum = (pos-(_FAT.FirstDataSector*_boot.BPB_BytsPerSec))/_FAT.ClustSize;

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

  while (i <= buff){
	if (isRoot == 0)
	  fseek(disk, data_start+i, SEEK_SET);
	else
	  fseek(disk, root_start+i, SEEK_SET);
	fread(&attr, sizeof(char), 1, disk);
	if ((attr != 0x00) || (attr != 0xe5)){
	  fseek(disk, 10, SEEK_CUR);
	  fread(&attr, sizeof(char), 1, disk);
	  fseek(disk, -12, SEEK_CUR);
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
	  printf("File %s not located in directory.\n", file);
	  rewind(disk);
	  fseek(disk, posinit, SEEK_SET);
	  free(file_buff);
	  free(disk_buff);
	  file_buff = NULL;
	  disk_buff = NULL;
	  return disk;
	}
	rewind(disk);
  }

  if (dirPresent == 0){
	printf("File %s is not present in directory.\n", file);
	free(file_buff);
	free(disk_buff);
	file_buff = NULL;
	disk_buff = NULL;
	return disk;
  }
  
  int numOpen = _olist.numOpen;
  for(j=0;j<32;j++){
	if (strncmp(file_buff, _olist.openFiles[j], 11) == 0){
	  memset(_olist.openFiles[j], '\0', sizeof(_olist.openFiles[j]));
	  _olist.filePos[j] = 0;
	  _olist.numOpen--;
	  break;
	}
  }

  printf("closed file: [%s] (if blank, fclose was successful)\n", _olist.openFiles[i]);
  printf("closed file position: %d\n", _olist.filePos[i]);
  printf("number of open files: %d\n", _olist.numOpen);

  rewind(disk);
  fseek(disk, posinit, SEEK_SET);
  free(file_buff);
  free(disk_buff);
  file_buff = NULL;
  disk_buff = NULL;
  return disk;
}
