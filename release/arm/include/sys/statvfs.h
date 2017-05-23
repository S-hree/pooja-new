/* Copyright (C) 2015 Autotalks Ltd. */
#ifndef _STATVFS_H
#define _STATVFS_H

#include <craton/check.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   File System statistics
*/

/** File system block count */
typedef unsigned int fsblkcnt_t;

/** File system file count */
typedef unsigned int fsfilcnt_t;

/** File system statistics */
struct statvfs {
  /** File system block size in bytes */
  unsigned long f_bsize;

  /** Fundamental file system block size in bytes */
  unsigned long f_frsize;
  
  /** Total number of blocks in file system in units of 'f_frsize' */
  fsblkcnt_t f_blocks;

  /** Total number of free blocks */
  fsblkcnt_t f_bfree;

  /** Number of free blocks available to unprivileged process */
  fsblkcnt_t f_bavail;

  /** Total number of i-nodes */
  fsfilcnt_t f_files;

  /** Total number of free i-nodes */
  fsfilcnt_t f_ffree;

  /** Number of i-nodes available to unprivileged process ('f_ffree' on Linux) */
  fsfilcnt_t f_favail;

  /** File system ID */
  unsigned long f_fsid;

  /** Mount flags */
  unsigned long f_flag;

  /** Maximum length of filename */
  unsigned long f_namemax;
};

/**
   Obtain information about a file system.

   @param[in] path Path to file in file system
   @param[out] buf File system statistics

   @retval 0 if succeeded
   @return -1 if failed and set errno appropriately
*/
int
statvfs(const char *path, struct statvfs *buf);

#ifdef __cplusplus
}
#endif

#endif /* _STATVFS_H */
