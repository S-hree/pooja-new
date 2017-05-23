/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>

#include <nxd_bsd.h>

/*
  CRATON NX-BSD UDP Recieve Example

  This example demonstrates basic usage of NX-BSD API for code running on top
  of CRATON processor with ThreadX RTOS.

  A UDP socket is opened and bound to an arbitrary port number. UDP packets
  are received in a loop and their length is printed to console.
*/

/* Port number used in this example */
#define EXAMPLE_PORT_NUMBER 2015

/* Maximum message size in octets */
#define EXAMPLE_MAX_MSG_SIZE 1000

void craton_user_init(void)
{
  /* Internet address family socket address */
  struct sockaddr_in sockaddr;
  /* Socket's file descriptor */
  int fd = -1;
  /* POSIX return code */
  int rc = 0;

  printf("NX-BSD UDP receive example.\n");

  /* Create a UDP socket */
  printf("Creating UDP socket...\n");
  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (fd == -1) {
    fprintf(stderr, "socket failed: %d\n", errno);
    return;
  }

  /* Prepare socket address struct */
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(EXAMPLE_PORT_NUMBER);
  sockaddr.sin_addr.s_addr = INADDR_ANY;

  /* Bind the socket */
  printf("Binding UDP socket to port %d...\n", EXAMPLE_PORT_NUMBER);
  rc = bind(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
  if (rc == -1) {
    fprintf(stderr, "bind failed: %d\n", errno);
    return;
  }

  while (1) {
    /* Buffer for received messages */
    char msg[EXAMPLE_MAX_MSG_SIZE];
    /* Length of received message */
    ssize_t len = 0;

    printf("Receiving UDP packet...\n");
    len = recv(fd, msg, sizeof(msg), 0);
    if (len == -1) {
      fprintf(stderr, "recv failed: %d\n", errno);
      continue;
    }

    /* Print received packet length */
    printf("Received UDP packet length: %d bytes.\n", len);
  }

  return;
}
