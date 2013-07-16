#include <stdio.h>
#include <stdlib.h>
#include "file_sys.h"

// Closes the disk, still keeps the written data
// Use this if you want to switch file systems and still keep data
// Use fsUnmount if you want to switch and delete all written data
ifp fsCloseDisk(ifp disk){
  fclose(disk);
  return disk;
}
