#include	"unp.h"

void connect_to_srv(char *ipaddr);

int
main(int argc, char **argv)
{
	int i;

	if (argc != 3)
		err_quit("usage: a.out <IPaddress> <Num>");

	for (i = 0; i < atoi(argv[2]); i++) {
		connect_to_srv(argv[1]);
	}

	exit(0);
}

void connect_to_srv(char *ipaddr)
{
	int					sockfd, n, m;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(49999);	/* daytime server */
	if (inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", ipaddr);

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");

	/* MSG_PEEK */
	// while((n = recv(sockfd, recvline, MAXLINE, MSG_PEEK)) > 0) {
	// 	printf("data length: %d\n", n);
	// 	if ((m = read(sockfd, recvline, n)) < 0)
	// 		err_quit("read error");

	// 	recvline[n] = 0;	/* null terminate */
	// 	if (fputs(recvline, stdout) == EOF)
	// 		err_sys("fputs error");
	// }

	/* ioctl */
	// n = -1;
	// while (n <= 0)
	// 	ioctl(sockfd, FIONREAD, &n);

	// printf("data length: %d\n", n);
	// if ((m = read(sockfd, recvline, n)) < 0)
	// 	err_quit("read error");

	// recvline[m] = 0;	/* null terminate */
	// if (fputs(recvline, stdout) == EOF)
	// 	err_sys("fputs error");

	// if (m < 0)
	// 	err_sys("read error");

	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (n < 0)
		err_sys("read error");

	close(sockfd);
}
