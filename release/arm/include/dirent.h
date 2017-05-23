/* Copyright (C) 2015 Autotalks Ltd. */
#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/stat.h>

#include <craton/check.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   Format of directory entries
*/

/** Directory entry name max length */
#define DIRENT_NAME_MAX 250

/** Directory Entry */
struct dirent {
  /** File serial number, not used */
  ino_t d_ino;

  /** Name of entry */
  char d_name[DIRENT_NAME_MAX];

};

/** Directory stream object */
typedef struct {
  /* Directory file */
  int dd_fd;

  /* Position in buffer */
  int dd_loc;

  /* Position of seek */
  int dd_seek;

  /* Allocated buffer */
  char *dd_buf;

  /* Allocated buffer length */
  int dd_len;
  
  /* Size of data in buffer */
  int dd_size;

} DIR;

/**
   Open a directory stream corresponding to @p name.

   An open directory stream must be closed via ::closedir by user.

   @param[in] name Directory path

   @retval Pointer to directory stream object if succeeded
   @return NULL if failed and set errno appropriately
*/
DIR *
opendir(const char *name);

/**
   Read successive entries from directory stream @p dirp.

   @param[in] Pointer to directory stream object

   @retval Pointer to directory entry
   @retval NULL if the end of the directory stream is reached
   @return NULL if failed and set errno appropriately
*/
struct dirent *
readdir(DIR *dirp);

/**
   Close directory stream @p dirp.

   @param[in] Pointer to directory stream object

   @retval 0 if succeeded
   @return -1 if failed and set errno appropriately
*/
int
closedir(DIR *dirp);

#ifdef __cplusplus
}
#endif

#endif /* _DIRENT_H */
