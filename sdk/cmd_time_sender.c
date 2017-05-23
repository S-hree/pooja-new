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


//int main(int argc, char *argv[]){
int cmd_time_sender(cli_instance_t *cli, const char *command, char *argv[], int argc)
{	
	int sockfd;
	struct sockaddr_in servaddr;
	unsigned char msg[sizeof(struct timeval)];
	int n,count = 0;
	int len = sizeof(servaddr);
	struct timeval time;	
	int a1, a2, a3, a4;
	//unsigned int addr;
	
	if(argc != 1){
		cli_print(cli, "time_sender <ip>");
		return 0;
	}
		
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); /* create a socket */
	if (sockfd == -1) 
	{
    	cli_print(cli, "socket failed: %d\n");
    	return;
	}
	
	/* init servaddr */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	if(sscanf(argv[0], "%d.%d.%d.%d", &a1,&a2,&a3,&a4) != 4)
	{
		perror( "sscanf error \n");
		return;
	}
		
	
	
	//addr = (unsigned int) (a1 << 24 + a2 << 16 + a3 << 8 + a4);
	servaddr.sin_addr.s_addr = htonl(IP_ADDRESS(a1, a2, a3, a4));
		
	/*
	if(inet_pton(AF_INET, argv[0], &servaddr.sin_addr) <= 0){
		cli_print(cli,"[%s] is not a valid IPaddress\n", argv[0]);
		exit(1);
	}
	
	*/

	/* bind address and port to socket */
	//if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
	//	perror("bind error");
	//	exit(1);
	//}
	do{
			
		gettimeofday(&time, NULL);
			
		*(struct timeval *) msg = time;
		n = sendto(sockfd, msg, sizeof(struct timeval), 0, (struct sockaddr *) &servaddr, len);
			
		if(n <= 0){
			perror("send error");
			return;
		}
		usleep(50000);
		//printf("time offset = %d\n", 1);
		count++;
	}while(count < 100);
	
	close(sockfd);

	return 0;
}

