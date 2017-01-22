/*
SLog

*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "slog.h"

void slog_initbersih (slogptr pslog, const char *file, const char *app)
{
  FILE *f;

  strncpy (pslog->file, file, 255);
  strncpy (pslog->app, app, 11);
  pslog->siap = 0;

  if ( (f=fopen(file,"w+")) )
  {
    fprintf (f, "* slog=%s *\n", app);
    fclose (f);
    pslog->siap = 1;
  }
}

void slog_tulis (slogptr pslog, const char *pesan)
{
  FILE *f;
  time_t mtime;
  struct tm *mtm;

  if (pslog->siap)
  {
    time(&mtime);
    mtm = localtime (&mtime);

    if ( (f=fopen(pslog->file,"a+")) )
    {
      fprintf (f, "%4d-%02d-%02d+%02d:%02d:%02d %s\n", mtm->tm_year+1900, mtm->tm_mon+1, mtm->tm_mday,
        mtm->tm_hour, mtm->tm_min, mtm->tm_sec, pesan);
      fclose (f);
    }
  }
}
