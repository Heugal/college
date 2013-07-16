#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

BOOT_SECT _boot;
FAT_SECT _FAT;
PWD _cDir;

ifp fsChangeDir(ifp disk, char *dirName){
  unsigned char *dir_buff = (char*)malloc(sizeof(char)*11);
  unsigned char *disk_buff = (char*)malloc(sizeof(char)*11);
  unsigned char *padding = "           ";
  unsigned short offset;
  unsigned int pos = ftell(disk);
  unsigned int posinit = pos;
  unsigned int isRoot = 0;
  unsigned short clustNum;
  unsigned int clustPos;
  unsigned char attr;
  unsigned char dirPresent = 0;
  unsigned short firstFree = 0;
  int buff;
  unsigned int data_start;
  unsigned int root_start;
  int i=0,j,c;

  if (strlen(dirName) > 11){
	printf("Directory %s is not located in current directory.\n", dirName);
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
   
  if (pos <= (_FAT.FirstDataSector*_boot.BPB_BytsPerSec)){
	isRoot = 1;
	pos = (_FAT.FirstRootDirSecNum*_boot.BPB_BytsPerSec);
	root_start = pos;
	clustNum = 0x0001;
	buff = (_FAT.RootDirSectors*_boot.BPB_BytsPerSec);
  }
  else{
	clustNum = (pos-(_FAT.FirstDataSector*_boot.BPB_BytsPerSec))/_FAT.ClustSize;
	buff = _FAT.ClustSize;
	data_start = (_FAT.FirstDataSector*_boot.BPB_BytsPerSec)+(clustNum*_FAT.ClustSize);
  }	
  
  int postest =0;
  while (i <= buff){
	if (isRoot == 0)
	  fseek(disk, data_start+i, SEEK_SET);
	else
	  fseek(disk, root_start+i, SEEK_SET);
	fseek(disk, 11, SEEK_CUR);
	fread(&attr, sizeof(char), 1, disk);
	fseek(disk, -12, SEEK_CUR);
	if ((attr&0x10) == 0x10){
	  fread(disk_buff, strlen(dir_buff), 1, disk);
	  if (strncmp(dir_buff, disk_buff, strlen(dirName)) == 0){
	    dirPresent = 1;
	    if (isRoot == 1)
	      pos = root_start+i;
	    else
	      pos = data_start+i;
	    rewind(disk);
	    fseek(disk, pos, SEEK_SET);
	    break;
	  }
	}
	i += 32;
	if (i == buff){
	  printf("Directory %s is not located in current directory.\n", dirName);
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
	printf("Directory %s is not located in current directory.\n", dirName);
	rewind(disk);
	fseek(disk, posinit, SEEK_SET);
	free(dir_buff);
	free(disk_buff);
	dir_buff = NULL;
	disk_buff = NULL;
	return disk;
  }
  fseek(disk, 26, SEEK_CUR);
  fread(&offset, sizeof(short), 1, disk);
  rewind(disk);
  fseek(disk, (_FAT.FirstDataSector*_boot.BPB_BytsPerSec)+(offset*_FAT.ClustSize), SEEK_CUR);
  
  i=0;
  int copyDone = 0;
  //if directory to change to == "..", remove current directory from current directory listing
  //otherwise add current directory name
  if (strncmp(dir_buff, "..", 2) == 0){
	for (j=2;j<255;j++){
	  if (_cDir.currDir[j] == '\0'){
		for (c=j;c>1;c--){
		  _cDir.depth--;
		  if (_cDir.currDir[c] == '/'){
			if (c > 1){
			  _cDir.currDir[c] = '\0';
			  copyDone = 1;
			  break;
			}
		    else
			  copyDone = 1;
			  break;
		  }			
		  else
			_cDir.currDir[c] = '\0';
		}
	  }
	  if (copyDone == 1)
		break;
	}
  }
  else{
	for (j=2;j<256;j++){
	  if (_cDir.currDir[j] == '\0'){
		if (j+strlen(dirName)+1 >= 256)
		  break;
		else{
		  _cDir.depth++;
		  c=j;
		  if (j != 1){
		    _cDir.currDir[c] = '/';
			c++;
		  }
		  for(c;c<255;c++){
			if (dirName[i] != '\0'){
			  _cDir.currDir[c] = dirName[i];
			  i++;
			}
			else{
			  copyDone = 1;
			  break;
			}
		  }
		}
	  }
	  if (copyDone == 1)
		break;
	}
  }

  free(dir_buff);
  free(disk_buff);
  dir_buff = NULL;
  disk_buff = NULL;
  return disk;
}
