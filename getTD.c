#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_sys.h"

unsigned short getTime(void){
  time_t now;
  struct tm *tm;
  now = time(0);
  tm = localtime(&now);

  unsigned short sec = tm->tm_sec;
  unsigned short min = tm->tm_min;
  unsigned short hour = tm->tm_hour;
  unsigned short theTime = 0x0000;
  
  hour = hour<<11;
  min = min<<5;

  theTime = theTime|hour;
  theTime = theTime|min;
  theTime = theTime|sec;

  return theTime;  
}

unsigned short getDate(void){
  time_t now;
  struct tm *tm;
  now = time(0);
  tm = localtime(&now);

  unsigned short day = tm->tm_mday;
  unsigned short month = tm->tm_mon;
  unsigned short year = tm->tm_year;
  year -= 80;
  unsigned short theDate = 0x0000;

  year = year<<9;
  month = month<<5;

  theDate = theDate|year;
  theDate = theDate|month;
  theDate = theDate|day;

  return theDate;
}
