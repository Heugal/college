LAB 4 
Written by Stephan Luchanko (tuc02096)
File System Lab

To compile:
I have included a script that runs a MakeFile to compile all of the C files and headers at once
and create an empty 2 or 10MB drive. To run the script, type:
  ./DiskInit2 for 2MB drive
  ./DiskInit10 for 10MB drive

If I cannot include executable files in email, then instead of using the script the following
commands can be used instead:
  make -f MakeFile
  dd if=/dev/zero of=Drive2MB bs=1 count=2000000  for 2MB drives
  dd if=/dev/zero of=Drive10MB bs=1 count=10000000  for 10MB drives

If permission is denied for either of these files, type:
  chmod 755 ./DiskInit2   for 2MB drive
  chmod 755 ./DiskInit10  for 10MB drive
If .o files do not work on current CPU architecture, type:
  rm *.o
and run ether of the above scripts. The program can then be run by typing:
  ./practice_system
and use the name:
  Drive2MB for the 2MB drive
  Drive10MB for the 10MB drive

To view information on the drive, use the command:
hexdump -C Drive2MB for 2MB drive
hexdump -C Drive10MB for 10MB drive

This program was compiled on the Unbuntu Linux operating system

The program itself consists of the following files:
  sys_test.c  -    used to test the various I/O functions
  fsMount.c   -    used to mount the 512MB boot sector, the FAT sector, and the root sector
  fsCreateDir.c -  used to create an empty directory
  fsCreateFile.c - used to create an empty handle to a file
  fsChangeDir.c -  used to change the directory to the specified directory
  fsOpen.c    -    used to open a specified file. the file returns a pointer in the 
		   disk to the first writable data area and puts the open file into
		   and open file list.
  fsClose.c   -	   used to close a file and return to the directory which contains 
		   the information which points to the data sector of the file and
		   removes the file from the open file list
  getTD.c     -    Gets the current time and date and converts this to a two byte format
  fsSearch.c  -    Takes a cluster number and finds the position in the FAT of the cluster
		   Either returns a pointer to the position, or the numerical value of the
		   FAT entry based on the entry# and offset
  file_sys.h  -    header file which stores various data structures used for positioning and
		   updating lists

  files not included (work in progress):
  fsRead.c    -    reads from the disk and stores that info into a buffer
  fsWrite.c   -    writes from a buffer to the disk provided that the file is not too large
		   (16K bytes)
  fsCloseDisk.c -  Closes and keeps current infomation on the disk
  fsUnmount.c   -  Unmounts the disk and wipes all information

The latter 4 sections are still being worked on, but I have worked almost nonstop for the 
past few weeks and I do not want to lose points, so I will send what I have completed as of
right now and hopefully have the rest done as fast as I can write it.

Progress on this lab was split into two sections. The first section consisted of mounting the
file system and using the computations based on the size of the disk to create the FAT and
sizes of the clusters etc. The second part consisted of reading values based on the position
of data structures created and written to the filesystem. Each directory has a header which
stores the name, time of creation, attributes, cluster number in the FAT and filesize. To find
positining of various data members in the drive, I would used fseek and ftell to find values
stored in the drive (FAT table, cluster number, etc) and calculate where in the file I would
need to reposition the file pointer to in order to create the proper results. Afterwards 
I would debug the file system to make sure that the directories were being created as they 
were intended to. This required meticulous checking of file pointer positioning to make sure
that every single value was correct, because if the file pointer was off by even one byte, 
it would result in completely incorrect data.
