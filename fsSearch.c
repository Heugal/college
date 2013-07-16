#include <stdlib.h>
#include <stdio.h>
#include "file_sys.h"

ifp fsSearchFAT(ifp disk, unsigned short clust_num){
  FAT_SECT _FAT;
  BOOT_SECT _BOOT;
  unsigned short ThisFATSecNum;
  unsigned short ThisFATEntOffset;
  unsigned short FATSz = _BOOT.BPB_FATSz16;
  unsigned short FATOffset = clust_num*2;

  if(clust_num >= _FAT.CountofClusters){
	printf("Invalid cluster number.\n");
  }
  else{
	ThisFATSecNum = _BOOT.BPB_RsvdSecCnt + (FATOffset/_BOOT.BPB_BytsPerSec);
	ThisFATEntOffset = FATOffset%_BOOT.BPB_BytsPerSec;
	rewind(disk);
	fseek(disk, (_BOOT.BPB_BytsPerSec*ThisFATSecNum), SEEK_SET);
	fseek(disk, ThisFATEntOffset, SEEK_CUR);
  }

  return disk;
}
