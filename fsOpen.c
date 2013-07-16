#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

BOOT_SECT _boot;
FAT_SECT _FAT;
OPEN_LIST _olist;

ifp fsOpen(ifp disk, char *file, char *fattr){
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

  if (_olist.numOpen >= 32){
	printf("Cannot open file; currently too many open files.\n");
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
	strncpy(file_buff, file, strlen(file)+1);
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
	i = 64;
  }
  else{
	root_start = _FAT.FirstRootDirSecNum*_boot.BPB_BytsPerSec;
	clustNum = 0x0001;
	buff = (_FAT.RootDirSectors*_boot.BPB_BytsPerSec);
	i=0;
  }
  
  int postest = 0;
  unsigned int firstFreeEnt = 0;
  while (i <= buff){
	if (isRoot == 0)
	  fseek(disk, data_start+i, SEEK_SET);
	else
	  fseek(disk, root_start+i, SEEK_SET);
	fread(&attr, sizeof(char), 1, disk);
    // we're checking if the file we want to located is in the current directory
    // 0x00 indicates an empty space, 0xe5 indicates a previous deleted file
	if ((attr != 0x00) && (attr != 0xe5)){
	  fseek(disk, 10, SEEK_CUR);
	  fread(&attr, sizeof(char), 1, disk);
	  fseek(disk, -12, SEEK_CUR);
      // the directory attribute 0x10 indicates that the file is a subdirectory
      // so we make sure that we want to open a file and not a subdirectory
	  if ((attr&0x10) != 0x10){
		fread(disk_buff, strlen(file_buff), 1, disk);
		if (strncmp(file_buff, disk_buff, 11) == 0){
		  dirPresent = 1;
		  firstFreeEnt = i;
		  break;
		}
	  }
	}
	else{
	  if (firstFreeEnt == 0){
		firstFreeEnt = i;
	  }
	}
	i += 32;
	if (i == buff){
	  if (firstFreeEnt != 0){
	    rewind(disk);
	    if (isRoot == 0)
		fseek(disk, data_start+firstFreeEnt, SEEK_SET);
	    else
		fseek(disk, root_start+firstFreeEnt, SEEK_SET);
	    break;
	  }
	  else{
	    printf("File %s is not located in the current directory and no space in directory to create it.\n", file);
		free(file_buff);
		free(disk_buff);
		file_buff = NULL;
		disk_buff = NULL;
	    return disk;
	  }
	}
	rewind(disk);
  }
  //if dirPresent = 1, file is found in directory
  //otherwise if there is room in the directory,
  //a "filename".txt handle will be created
  if (dirPresent == 0)
    disk = fsCreateFile(disk, file);
  
  rewind(disk);
  if (isRoot == 0)
	fseek(disk, data_start+firstFreeEnt, SEEK_SET);
  else
	fseek(disk, root_start+firstFreeEnt, SEEK_SET);
  if (strncmp(fattr, "r", 1) == 0){
	fseek(disk, 11, SEEK_CUR);
	fread(&attr, sizeof(char), 1, disk);
	attr = attr|0x01;
	fseek(disk, -1, SEEK_CUR);
	fwrite(&attr, sizeof(char), 1, disk);
	fseek(disk, -11, SEEK_CUR);
  }
  postest = ftell(disk);
  fseek(disk, 25, SEEK_CUR);
  postest = ftell(disk);
  fread(&offset, sizeof(short), 1, disk);
  rewind(disk);
  fseek(disk, (_FAT.FirstDataSector*_boot.BPB_BytsPerSec)+(offset*_FAT.ClustSize)+64, SEEK_CUR);
  i=0;
  postest = ftell(disk);
  for (i;i<32;i++){
	//if the open file entry is empty
	if (_olist.openFiles[i][0] == '\0'){
	  strcpy(_olist.openFiles[i], file_buff);
	  _olist.filePos[i] = postest;
	  _olist.numOpen++;
	  break;
	}
  }

  printf("Opened file: %s\n", _olist.openFiles[i]);
  printf("open file position: %d\n", _olist.filePos[i]);
  printf("number of open files: %d\n", _olist.numOpen);
  free(file_buff);
  free(disk_buff);
  file_buff = NULL;
  disk_buff = NULL;  

  return disk;
}
