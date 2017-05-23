/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/stat.h>

#include <tx_api.h>
#include <tx_posix.h>

#include <craton/fs.h>

/*
  CRATON ThreadX File System Example

  This example demonstrates usage of file system API for code running on top
  of CRATON processor with ThreadX RTOS.

  The example demonstrates supported file system functions. It can be run on
  microSD or flash devices:

    - Device 'A' is mapped to microSD card (available on some boards).
    - Device 'B' is mapped to flash device.
*/

/* Device used in this example */
#define EXAMPLE_FS_DEVICE "B:/"

/* Maximum number of characters in path name */
#define EXAMPLE_DIR_NAME_MAX 80

/* Size of buffer used in this example */
#define EXAMPLE_BUFFER_SIZE 128

/* Name of directory used in this example */
#define EXAMPLE_DIR "example_dir"

/* Name of 1st file used in this example */
#define EXAMPLE_FILE_1 "example_file_1"

/* Name of 2nd file used in this example */
#define EXAMPLE_FILE_2 "example_file_2"

/* Format of example line */
#define EXAMPLE_LINE_FMT "example_line_%d"

#ifdef __CRATON_ARM

int example_cleanup(void)
{
  /* Directory stream object */
  DIR *dir = NULL;
  /* Directory entry object */
  struct dirent *dirent = NULL;
  /* Directory statistics */
  struct fs_dirstat dirstat;
  /* Return code */
  int rc = 0;

  /* Change directory to root of device */
  rc = chdir(EXAMPLE_FS_DEVICE);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "chdir failed, errno %d\n", rc);
    return rc;
  }
  printf("Changed to directory '%s'\n", EXAMPLE_FS_DEVICE);

  /* Obtain directory statistics */
  rc = fs_dirstat(EXAMPLE_DIR, &dirstat, FS_DIRSTAT_DEPTH_MAX);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "fs_dirstat failed, errro %d\n", rc);
    return rc;
  }
  printf("Directory statistics: number of dirs %d, number of files %d\n",
         dirstat.num_of_dirs, dirstat.num_of_files);

  /* Open the example directory */
  dir = opendir(EXAMPLE_DIR);
  if (dir == NULL) {
    rc = -errno;
    fprintf(stderr, "opendir failed, errno %d\n", rc);
    goto error;
  }
  printf("Opened directory '%s'\n", EXAMPLE_DIR);

  printf("Recursing over directory tree...\n");
  while ((dirent = readdir(dir))) {
    struct stat status;
    char dpath[DIRENT_NAME_MAX];

    /* Stat expects a relative path */
    sprintf(dpath, "%s/%s", EXAMPLE_DIR, dirent->d_name);

    /* Get file status */
    rc = stat(dpath, &status);
    if (rc == -1) {
      rc = -errno;
      fprintf(stderr, "stat failed, errno %d", rc);
      goto error;
    }

    switch(status.st_mode) {
    case S_IFDIR:
      printf("Found directory '%s'\n", dirent->d_name);
      break;
    case S_IFREG:
      printf("Found file '%s', removing it...\n", dirent->d_name);

      /* Delete the file */
      rc = remove(dpath);
      if (rc == -1) {
        rc = -errno;
        fprintf(stderr, "remove failed, errno %d\n", rc);
        goto error;
      }
      break;
    default:
      fprintf(stderr, "Unsupported file type %d", status.st_mode);
      break;
    }
  }

error:
  /* Close directory (cleanup resources used by opendir) */
  rc = closedir(dir);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "closedir failed, errno %d\n", rc);
  }
  printf("Closed directory '%s'\n", EXAMPLE_DIR);

  /* Remove example directory */
  rc = rmdir(EXAMPLE_DIR);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "rmdir failed, errno %d\n", rc);
  }
  printf("Removed directory '%s'\n", EXAMPLE_DIR);

  return rc;
}

void craton_user_init(void)
{
  /* Example buffer */
  char buf[EXAMPLE_BUFFER_SIZE];
  /* Directory path */
  char dirpath[EXAMPLE_DIR_NAME_MAX];
  /* File descriptor */
  int fd = -1;
  /* Current directory */
  char *current_dir = NULL;
  /* Size of read data */
  ssize_t size = 0;
  /* Return code */
  int rc = 0;

  printf("Start file system example...\n");

  /* Change directory to root of device */
  rc = chdir(EXAMPLE_FS_DEVICE);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "chdir failed, errno %d\n", rc);
    goto error;
  }
  printf("Changed directory to '%s'\n", EXAMPLE_FS_DEVICE);

  /* Make a new directory */
  rc = mkdir(EXAMPLE_DIR, 0);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "mkdir failed, errno %d\n", rc);
    goto error;
  }
  printf("Made new directory '%s'\n", EXAMPLE_DIR);

  /* Change directory to the directory we created */
  rc = chdir(EXAMPLE_DIR);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "chdir failed, errno %d\n", rc);
    goto error;
  }
  printf("Changed directory to '%s'\n", EXAMPLE_DIR);


  /* Get current directory */
  current_dir = getcwd(dirpath, sizeof(dirpath));
  if (current_dir == NULL) {
    rc = -errno;
    fprintf(stderr, "getcwd failed, errno %d\n", rc);
    goto error;
  }
  printf("Current directory is '%s'\n", current_dir);

  /* Open a new file (create it if it does not exist) */
  fd = open(EXAMPLE_FILE_1, O_APPEND | O_RDWR);
  if (fd == -1) {
    rc = -errno;
    fprintf(stderr, "open failed, errno %d\n", rc);
    goto error;
  }
  printf("Opened file '%s'\n", EXAMPLE_FILE_1);

  /* Write 10 example lines */
  for (int i = 1; i <= 5; ++i) {
    snprintf(buf, sizeof(buf), EXAMPLE_LINE_FMT "\n", i);
    size = write(fd, buf, strlen(buf) + 1);
    if (size == -1) {
      rc = -errno;
      fprintf(stderr, "write failed, errno %d\n", rc);
      goto error;
    }
    printf("Written line '" EXAMPLE_LINE_FMT "'\n", i);
  }

  /* Close the file */
  rc = close(fd);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "close failed, errno %d\n", rc);
    goto error;
  }
  printf("Closed file '%s'\n", EXAMPLE_FILE_1);

  /* Mark file descriptor as closed */
  fd = -1;

  /* Rename the file */
  rc = rename(EXAMPLE_FILE_1, EXAMPLE_FILE_2);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "rename failed, errno %d\n", rc);
    goto error;
  }
  printf("Renamed file '%s' to '%s'\n", EXAMPLE_FILE_1, EXAMPLE_FILE_2);

  /* Open the renamed file in read-only mode */
  fd = open(EXAMPLE_FILE_2, O_RDONLY);
  if (fd == -1) {
    rc = -errno;
    fprintf(stderr, "open failed, errno %d\n", rc);
    goto error;
  }
  printf("Opened file '%s'\n", EXAMPLE_FILE_2);

  /* Seek to 2nd line (assuming all lines are of same length) */
  rc = lseek(fd, size, SEEK_SET);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "lseek failed, errno %d\n", rc);
    goto error;
  }
  printf("Seeked to 2nd line written.\n");

  /* Read 2nd line (excluding newline char) */
  size = read(fd, buf, size - 1);
  if (size == -1) {
    rc = -errno;
    fprintf(stderr, "read failed, errno %d\n", rc);
    goto error;
  }
  printf("Read 2nd line:\n%s", buf);

  /* Close the file */
  rc = close(fd);
  if (rc == -1) {
    rc = -errno;
    fprintf(stderr, "close failed, errno %d\n", rc);
    goto error;
  }
  printf("Closed file '%s'\n", EXAMPLE_FILE_2);
  
  /* Mark file descriptor as closed */
  fd = -1;

  /* Cleanup example resources */
  rc = example_cleanup();
  if (rc == 0) {
    printf("File system example completed successfully!\n");
  }
  return;

error:
  example_cleanup();
}

#else /* __CRATON_ARM */

void craton_user_init(void)
{
}

#endif /* __CRATON_ARM */
