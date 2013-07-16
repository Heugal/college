#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_sys.h"

int main(int argc, char **argv){
  ifp disk;
  char *drive = (char*)malloc(sizeof(char)*32);
  unsigned char _BOOT_SECT[512];
  char *dirName = "filesys";
  
  printf("Please enter the name of the disk: \n");
  scanf("%s", drive);

  disk = fsMount(disk, drive);

  disk = fsCreateDir(disk, dirName);
  disk = fsCreateDir(disk, dirName);
  disk = fsCreateDir(disk, "newFolder");
  
  disk = fsCreateFile(disk, "data.txt");
  disk = fsCreateFile(disk, "data.txt");
  disk = fsCreateFile(disk, "superdat.bat");

  disk = fsChangeDir(disk, "newFolder");
  fsPrintDir();
  disk = fsCreateFile(disk, "stuff.dat");
  disk = fsCreateDir(disk, "moredir");
  disk = fsChangeDir(disk, "moredir");
  fsPrintDir();
  //disk = fsCreateFile(disk, "apple");
  disk = fsOpen(disk, "README.txt", "r");
  disk = fsChangeDir(disk, "..");
  if (fsClose("README.txt") == -1){
	printf("Error closing file.\n");
  }
  fsPrintDir();
  
  disk = fsCloseDisk(disk);
  //disk = fsUnmount(disk, drive);

  return 0;
}
