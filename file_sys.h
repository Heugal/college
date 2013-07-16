#ifndef FILE_SYS
#define FILE_SYS

typedef FILE* ifp;
ifp fsMount(ifp, char*);
ifp fsSearch(ifp, short);
ifp fsCreateDir(ifp, char*);
ifp fsCreateFile(ifp, char*);
ifp fsChangeDir(ifp, char*);
ifp fsOpen(ifp, char*, char*);
int fsClose(char*);
ifp fsUnmount(ifp, char*);
ifp fsCloseDisk(ifp);
void fsPrintDir(void);
char* fnameCreate(char*);
unsigned short getTime(void);
unsigned short getDate(void);

typedef struct directory{
  char currDir[256];
  int depth;
}PWD;

typedef struct open_files{
  char openFiles[32][11];
  int filePos[32];
  int numOpen;
}OPEN_LIST;

typedef struct fat_sector{
  unsigned int RootDirSectors;
  unsigned int FATSz;
  unsigned int FirstDataSector;
  unsigned int DataSec;
  unsigned int CountofClusters;
  unsigned int FAType;
  unsigned int FAT16ClusEntryVal;
  unsigned int FirstRootDirSecNum;
  unsigned int ClustSize;
}FAT_SECT;

typedef struct boot_sector{
  unsigned char BPB_jmpBoot[3];
  unsigned char BS_OEMName[8];
  unsigned short BPB_BytsPerSec;
  unsigned char BPB_SecPerClus;
  unsigned short BPB_RsvdSecCnt;
  unsigned char BPB_NumFATs;
  unsigned short BPB_RootEntCnt;
  unsigned short BPB_TotSec16;
  unsigned char BPB_Media;
  unsigned short BPB_FATSz16;
  unsigned short BPB_SecPerTrk;
  unsigned short BPB_NumHeads;
  unsigned int BPB_HiddSec;
  unsigned int BPB_TotSec32;
  unsigned char BS_DrvNum;
  unsigned char BS_Reserved1;
  unsigned char BS_BootSig;
  unsigned char BS_VolID[4];
  unsigned char BS_VolLab[11];
  unsigned char BS_FilSysType[8];
  unsigned char padding[444];
  unsigned char BS_Sig[2];
}BOOT_SECT;

#endif
