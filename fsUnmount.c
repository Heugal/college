#include <stdio.h>
#include <stdlib.h>
#include "file_sys.h"

// Clears the entire file system of all written data
// Use this if you want to switch file systems AND clear written data
// Use fsCloseDisk.c if you want to switch file systems and keep written data
ifp fsUnmount(ifp disk, char *diskName){
  rewind(disk);
  freopen(diskName, "w", disk);
  fclose(disk);
  return disk;
}
