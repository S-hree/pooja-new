#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <tx_api.h>
#include <tx_posix.h>
#include <nx_api.h>
#include <libcli.h>
#include <craton/cli.h>
#include <craton/net.h>
#include <craton/fs.h>

#include <linuxapi.h>

#include <nxd_bsd.h>



#include <wave_lib.h>

#include <dot3_common.h>
#include <dot3_api.h>

#include <porting_apis.h>

#define SERV_PORT 3123


int cmd_time_receiver(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;
	unsigned char msg[sizeof(struct timeval)];
	int n;
	int len = sizeof(cliaddr);
	struct timeval time, cur_time;
	int offset;
	int got = 0;
	int count = 0;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); /* create a socket */
	if (sockfd == -1) 
	{
    	cli_print(cli, "socket failed: %d\n");
    	return;
	}
  

	/* init servaddr */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	/* bind address and port to socket */
	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
		perror("recvfrom error");
		return;
	}
		
	do{
		n = recvfrom(sockfd, msg, sizeof(struct timeval), 0, (struct sockaddr *) &cliaddr, &len);
		if(n <= 0){
			cli_print(cli,"recvfrom error");
			return;
		}
	
		gettimeofday(&cur_time, NULL);
		time = *(struct timeval *)msg;
		offset = (cur_time.tv_sec - time.tv_sec) * 1000000 + cur_time.tv_usec - time.tv_usec;
		//if(offset < 0 || offset > 10000)
		//sum += offset;
		//if(got == 0){
		//	got = 1;
		//	printf("got one\n");
		//}
		//if(offset < 0 && got == 1)
			cli_print(cli,"time offset = %8d, time send = %ld.%06d, time recv = %d.%06d\n", offset, time.tv_sec, time.tv_usec, cur_time.tv_sec, cur_time.tv_usec);
		count++;
	}while(count < 100);
		//printf("average time offset = %8d\n", sum/count);
		
	close(sockfd);

	return 0;
}

