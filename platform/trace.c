#ifndef __CRATON__

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#include <atlk/trace.h>

#define ATLK_TRACE_ENVAR_NAME "ATLK_TRACE_FILE"

static pthread_once_t tr_once = PTHREAD_ONCE_INIT;
static FILE *tr_stream = NULL;

static void
tr_init(void)
{
  char *filename;

  /* Default stream */
  tr_stream = stdout;

  filename = getenv(ATLK_TRACE_ENVAR_NAME);
  if (filename) {
    FILE *stream = fopen(filename, "w+");
    if (!stream) {
      fprintf(stderr, "Failed to open %s (errno %d)\n", filename, errno);
    }
    else {
      tr_stream = stream;
    }
  }
  else {
    fprintf(stderr, "*** Note: you can select destination of Autotalks "
            "diagnostic logs via environment variable ATLK_TRACE_FILE\n");
  }

  /* Make the stream unbuffered */
  setbuf(tr_stream, NULL);
}

void
tr_log(int level, const char *fmt, ...)
{
  static const char level_chars[] = {'E', 'W', 'N', 'I', 'D'};

  va_list args;
  int rc;
  char level_char;

  rc = pthread_once(&tr_once, tr_init);
  assert(!rc);

  if ((level < 0) || (level >= sizeof(level_chars))) {
    level_char = '?';
  }
  else {
    level_char = level_chars[level];
  }

  fputc(level_char, tr_stream);
  fputc(':', tr_stream);
  fputc(' ', tr_stream);

  va_start(args, fmt);
  vfprintf(tr_stream, fmt, args);
  va_end(args);

  fputc('\n', tr_stream);
}

#endif /* __CRATON__ */
