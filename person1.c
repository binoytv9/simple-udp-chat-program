#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>


#define MAXBUFLEN 	100
#define MYPORT 		"8000"
#define PARTNERPORT 	"8001"

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET)
		return &(((struct sockaddr_in6 *)sa)->sin6_addr);

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
	int rv;
	int sockfd;
	int numBytes;
	char buf[MAXBUFLEN];
	struct addrinfo hints, *servinfo, *p;

	// adding this processes' info
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	if((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("socket");
			continue;
		}

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("bind");
			continue;
		}

		break;
	}

	if(p == NULL){
		fprintf(stderr, "failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo); // freeing the linked list

	// adding parter processes' info
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	if((rv = getaddrinfo(NULL, PARTNERPORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo 2: %s\n", gai_strerror(rv));
		return 1;
	}

	if(servinfo == NULL){
		fprintf(stderr, "failed to set partner process address\n");
		return 2;
	}

	printf("please start the chat....!!!\n");
	while(1){
		printf("me>>> ");
		if(fgets(buf, MAXBUFLEN, stdin) == NULL)
			break;

		buf[strlen(buf)-1] = '\0'; // removing the trailing '\n'

		if((sendto(sockfd, buf, strlen(buf), 0, servinfo->ai_addr, servinfo->ai_addrlen)) == -1){
			perror("sendto");
			exit(1);
		}

		if((numBytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0, NULL, NULL)) == -1){
			perror("recvfrom");
			exit(1);
		}

		printf("p2>>> ");
		buf[numBytes] = '\0';
		printf("%s\n", buf);
	}
	printf("\n");

	freeaddrinfo(servinfo); // freeing the linked list
	close(sockfd);
	return 0;
}
