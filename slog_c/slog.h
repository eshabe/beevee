/*
SLOG
*/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _SLOG_H__INCLUDED_
#define _SLOG_H__INCLUDED_

#ifdef  __cplusplus
extern "C" {
#endif


typedef struct _slog {
  char file[256];
  char app[13];
  short siap;
} slog;
typedef slog *slogptr;

void slog_initbersih (slogptr pslog, const char *file, const char *app);
void slog_tulis (slogptr pslog, const char *pesan);

#ifdef  __cplusplus
}
#endif

#endif
