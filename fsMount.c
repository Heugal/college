#include <stdio.h>
#include <stdlib.h>
#include "file_sys.h"
#include <string.h>

struct DSKSZTOSECPERCLUS{
  int diskSize;
  char secPerClusVal;
};

typedef struct{
  unsigned char ROOT_Name[11];
  unsigned char ROOT_Attr;
  unsigned char ROOT_NTRes;
  unsigned char ROOT_CrtTimeTenth;
  unsigned short ROOT_CrtTime;
  unsigned short ROOT_CrtDate;
  unsigned short ROOT_LstAccDate;
  unsigned short ROOT_FstClusHI;
  unsigned short ROOT_WrtTime;
  unsigned short ROOT_WrtDate;
  unsigned short ROOT_FstClusLO;
  unsigned int ROOT_FileSize;
}ROOT_DIR;

BOOT_SECT _boot;
FAT_SECT _FAT;
OPEN_LIST _olist;
PWD _currDir;

void mountVolID(BOOT_SECT *, int);
void mountFAT(BOOT_SECT *, FAT_SECT *);
void mountRoot(ROOT_DIR *);

ifp fsMount(ifp disk, char* diskName){
  ROOT_DIR _root;
  int len;
  unsigned short i;

  if ((!(disk = fopen(diskName, "rb+"))) && diskName != NULL){
	printf("Cannot locate/open specified file.\n");
	exit(0);
  }

  fseek(disk, 0L, SEEK_END);
  len = ftell(disk);
  fseek(disk, 0L, SEEK_SET);
  
  mountVolID(&_boot, len);
  mountFAT(&_boot, &_FAT);

  printf("\n-----------------------------------------------------\n");
  printf("                  BOOT SECTOR INFO\n");
  printf("-----------------------------------------------------\n");
  printf("Bytes per sector: %d bytes\n", _boot.BPB_BytsPerSec);
  printf("Sectors per cluster: %d sectors\n", _boot.BPB_SecPerClus);
  printf("Number of reserved sectors: %d sectors\n", _boot.BPB_RsvdSecCnt);
  printf("Number of FATs: %d FATs\n", _boot.BPB_NumFATs);
  printf("Root entry count: %d\n", _boot.BPB_RootEntCnt);
  printf("Total number of sectors: %d sectors\n", _boot.BPB_TotSec16);
  printf("Total size of FAT: %d sectors\n", _boot.BPB_FATSz16);
  printf("Size of boot sector: %d bytes\n", sizeof(_boot));

  printf("\n-----------------------------------------------------\n");
  printf("                  FAT SECTOR INFO\n");
  printf("-----------------------------------------------------\n");
  printf("Number of root directory sectors: %d sectors\n",_FAT.RootDirSectors);
  printf("First Data Sector location: sector %d\n", _FAT.FirstDataSector);
  printf("Number of sectors in data region: %d sectors\n", _FAT.DataSec);
  printf("Number of clusters in the data region: %d clusters\n", _FAT.CountofClusters); 
  printf("Size of cluster: %d bytes\n", _FAT.ClustSize);


  const unsigned int root_num = (_FAT.RootDirSectors*_boot.BPB_BytsPerSec)-32;
  const unsigned int FAT_num = (_boot.BPB_FATSz16*_boot.BPB_BytsPerSec)/2;
  unsigned char root_dir[root_num];
  unsigned short FAT[FAT_num];
  unsigned int j, sect;
  for (j=0;j<FAT_num;j++)
	FAT[j] = 0x0000;
  for (j=0;j<root_num;j++)
	root_dir[j] = 0x00;
  
  memset(_olist.openFiles, '\0', sizeof(_olist.openFiles));
  memset(_olist.filePos, 0, sizeof(_olist.filePos));
  _olist.numOpen = 0;
  
  _currDir.currDir[0] = '/';
  for (j=2;j<256;j++)
	_currDir.currDir[j] = '\0';
  _currDir.currDir[1] = '/';
  _currDir.depth = 0;

  FAT[0] = 0xfff8;
  FAT[1] = 0xffff;
  FAT[_FAT.CountofClusters-1] = 0xffff;

  if(_FAT.CountofClusters < 4085){
	printf("Volume is FAT12\n");
	_FAT.FAType = 12;
  }
  else if (_FAT.CountofClusters < 65526){
	printf("Volume is FAT16\n");
	_FAT.FAType = 16;
  }
  else{
	printf("Volume is FAT32\n");
	printf("FAT32 not supported.\n");
	_FAT.FAType = 32;
	exit(0);
  }
  printf("First sector of root directory location: sector %d\n\n", _FAT.FirstRootDirSecNum);	
 
  fwrite(&_boot, sizeof(_boot), 1, disk);
  fwrite(&FAT, sizeof(FAT), 1, disk);

   mountRoot(&_root);
   fwrite(&_root, sizeof(_root), 1, disk);
   fwrite(&root_dir, sizeof(root_dir), 1, disk);
  
  return disk;
}

void mountVolID(BOOT_SECT *boot, int len){
  struct DSKSZTOSECPERCLUS DskTableFAT16[] = {
	{2100, 0},
	{8170, 2},
	{65536,4}
  };
  int i = 0;
  int RootDirSectors, temp1, temp2;
  boot->BPB_jmpBoot[0] = 0xe9;
  boot->BPB_jmpBoot[1] = 0xe0;
  boot->BPB_jmpBoot[2] = 0x90;
  boot->BS_OEMName[0] = 'M';
  boot->BS_OEMName[1] = 'S';
  boot->BS_OEMName[2] = 'W';
  boot->BS_OEMName[3] = 'I';
  boot->BS_OEMName[4] = 'N';
  boot->BS_OEMName[5] = '4';
  boot->BS_OEMName[6] = '.';
  boot->BS_OEMName[7] = '1';
  boot->BPB_BytsPerSec = 512;
  printf("Length of file: %d\n", len);
  while(i < 3){
	if (DskTableFAT16[i].diskSize*boot->BPB_BytsPerSec < len)
	  i++;
	else{
	  boot->BPB_SecPerClus = DskTableFAT16[i].secPerClusVal;
	  break;
	}
  }
  boot->BPB_RsvdSecCnt = 1;
  boot->BPB_NumFATs = 1;
  boot->BPB_RootEntCnt = 512;
  boot->BPB_TotSec16 = len/512;
  boot->BPB_Media = 0xf8;

  RootDirSectors = ((boot->BPB_RootEntCnt*32)+(boot->BPB_BytsPerSec-1))/boot->BPB_BytsPerSec;
  temp1 = DskTableFAT16[i].diskSize - (boot->BPB_RsvdSecCnt + RootDirSectors);
  temp2 = (256*boot->BPB_SecPerClus) + boot->BPB_NumFATs;
  boot->BPB_FATSz16 = (temp1 + (temp2 - 1))/temp2; 
  
  boot->BPB_SecPerTrk = 0x00;
  boot->BPB_NumHeads = 0x02;
  boot->BPB_HiddSec = 0x00;
  boot->BPB_TotSec32 = 0x00;
  boot->BS_DrvNum = 0x80;
  boot->BS_Reserved1 = 0x00;
  boot->BS_BootSig = 0x29;
  boot->BS_VolID[0] = 0x11;
  boot->BS_VolID[1] = 0x29;
  boot->BS_VolID[2] = 0x03;
  boot->BS_VolID[3] = 0x45;
  boot->BS_VolLab[0] = 'N';
  boot->BS_VolLab[1] = 'O';
  boot->BS_VolLab[2] = ' ';
  boot->BS_VolLab[3] = 'N';
  boot->BS_VolLab[4] = 'A';
  boot->BS_VolLab[5] = 'M';
  boot->BS_VolLab[6] = 'E';
  boot->BS_VolLab[7] = ' ';
  boot->BS_VolLab[8] = ' ';
  boot->BS_VolLab[9] = ' ';
  boot->BS_VolLab[10] = ' ';
  boot->BS_FilSysType[0] = 'F';
  boot->BS_FilSysType[1] = 'A';
  boot->BS_FilSysType[2] = 'T';
  boot->BS_FilSysType[3] = '1';
  boot->BS_FilSysType[4] = '6';
  boot->BS_FilSysType[5] = ' ';
  boot->BS_FilSysType[6] = ' ';
  boot->BS_FilSysType[7] = ' ';

  for (i=0;i<sizeof(boot->padding);i++){
	boot->padding[i] = 0x90;
  }

  boot->BS_Sig[0] = 0x55;
  boot->BS_Sig[1] = 0xaa;
}

void mountFAT(BOOT_SECT *boot, FAT_SECT *fat){
  fat->RootDirSectors = ((boot->BPB_RootEntCnt*32)+(boot->BPB_BytsPerSec-1))/boot->BPB_BytsPerSec;
  fat->FATSz = boot->BPB_FATSz16;
  fat->FirstDataSector = (boot->BPB_RsvdSecCnt + (boot->BPB_NumFATs * fat->FATSz) + fat->RootDirSectors);
  fat->DataSec = boot->BPB_TotSec16 - (boot->BPB_RsvdSecCnt + (boot->BPB_NumFATs * fat->FATSz) + fat->RootDirSectors);
  fat->CountofClusters = fat->DataSec/boot->BPB_SecPerClus;
  fat->FirstRootDirSecNum = boot->BPB_RsvdSecCnt + (boot->BPB_NumFATs*boot->BPB_FATSz16);
  fat->ClustSize = boot->BPB_BytsPerSec*boot->BPB_SecPerClus;
}

void mountRoot(ROOT_DIR *root){
  int i;
  for (i=0;i<11;i++)
	root->ROOT_Name[i] = ' ';
  root->ROOT_Attr = 0x10;
  root->ROOT_NTRes = 0x00;
  root->ROOT_CrtTimeTenth = 0x00;
  root->ROOT_CrtTime = getTime();
  root->ROOT_CrtDate = getDate();
  root->ROOT_LstAccDate = getDate();
  root->ROOT_FstClusHI = 0x0000;
  root->ROOT_WrtTime = getTime();
  root->ROOT_WrtDate = getDate();
  root->ROOT_FstClusLO = 0xffff;
  root->ROOT_FileSize = 0x00000000;
}
