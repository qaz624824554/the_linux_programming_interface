#include "tlpi_hdr.h"
#include <stdio.h>
#include <time.h>

int main() {
  time_t t = time(NULL);
  printf("%d\n", t);

  printf("%s\n", ctime(&t));

  struct tm *local_tm = localtime(&t);
  printf("%d/%d/%d %d:%d:%d\n", local_tm->tm_mon + 1, local_tm->tm_mday, local_tm->tm_year + 1900, local_tm->tm_hour, local_tm->tm_min, local_tm->tm_sec);

  struct tm *gm_tm = gmtime(&t);
  printf("%d/%d/%d %d:%d:%d\n", gm_tm->tm_mon + 1, gm_tm->tm_mday, gm_tm->tm_year + 1900, gm_tm->tm_hour, gm_tm->tm_min, gm_tm->tm_sec);
  
  printf("%ld\n", mktime(local_tm));

  printf("%s\n", asctime(local_tm));
  printf("%s\n", asctime(gm_tm));

  char *outstr[100];
  strftime(outstr, 100, "%Y-%m-%d %H:%M:%S %Z", local_tm);
  printf("%s\n", outstr);

  strftime(outstr, 100, "%Y-%m-%d %H:%M:%S %Z", gm_tm);
  printf("%s\n", outstr);
  
} 