#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/netfilter_ipv4.h>

int main(int argc, char *argv[])
{
	if(argc==1) {
		printf("This Linux program accepts TCP connections and prints out the sin_addr behind SO_ORIGINAL_DST.");
		printf("Usage:\n");
		printf("    tcp-origdest <listen-port>\n");
		exit(1);
	}
	
	int listen_port = atoi(argv[1]);
	
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	
	// Create socket and get descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}
	
	// Bind to listen-address:port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(listen_port);

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt() failed");
		exit(EXIT_FAILURE);
	}
	
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address))<0) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}
	
	// Start TCP listening
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	
	printf("TCP listening on %d ...\n", listen_port);
	
	int count = 0;
	while(1)
	{
		// Wait to accept a connection.
		if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen))<0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		
		count++;
		
		printf("[%d] Connection from: %s:%d\n", count,
			inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		
		//
		// getsockopt(SO_ORIGINAL_DST)
		//
		
		struct sockaddr_in oaddr;
		socklen_t oaddr_len = sizeof(oaddr);
		if (getsockopt(new_socket, SOL_IP, SO_ORIGINAL_DST, &oaddr, &oaddr_len) < 0) {
			printf("    getsockopt(SO_ORIGINAL_DST) errno=%d, %s\n", errno, strerror(errno));
		}
		else {
			printf("    SO_ORIGINAL_DST: %s:%d\n", inet_ntoa(oaddr.sin_addr), ntohs(oaddr.sin_port));
		}
		
		// Say bye and disconnect client.

		const char bye[] = "Bye!\n";
		write(new_socket, bye, sizeof(bye)-1);
		
		usleep(500*1000);
		
		close(new_socket);
		printf("\n");
	}
	
	// Since the user will press Ctrl+C to stop the program, this close() has no chance to be executed.
	close(server_fd);
	
	return 0;
}
