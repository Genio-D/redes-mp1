#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BUFFER_SIZE 4096
#define TRUE 1
#define FALSE 0

int open_socket(char *host_name, int port) {
	int client_socket;
	struct sockaddr_in server_addr;
	int conversion_status;
	int connection_status;
	struct hostent *host;
	struct in_addr **addr_list;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == -1) {
		fprintf(stderr, "socket: Error\n");
		exit(-1);
	}

	host = gethostbyname(host_name);

	if(!host)
		fprintf(stderr, "gethostbyname: Error\n");

	addr_list = (struct in_addr **)host->h_addr_list;
	char *host_address = inet_ntoa(*addr_list[0]);

	conversion_status = inet_pton(AF_INET, host_address, &server_addr.sin_addr);
	if(!conversion_status) {
		fprintf(stderr, "inet_pton: Error\n");
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	connection_status = connect(
		client_socket,
		(struct sockaddr *) &server_addr,
		sizeof(server_addr));
	
	if(connection_status == -1) {
		fprintf(stderr, "connection: Error\n");
		exit(-1);
	}
	return client_socket;
}

void use_socket(int socket_fd) {
	char message[BUFFER_SIZE];
	char server_response[BUFFER_SIZE];
	fd_set fd_mask;
	int stdin_fd = fileno(stdin);

	while(TRUE) {
		FD_ZERO(&fd_mask);
		FD_SET(stdin_fd, &fd_mask);
		FD_SET(socket_fd, &fd_mask);

		select(socket_fd + 1, &fd_mask, NULL, NULL, NULL);

		if(FD_ISSET(socket_fd, &fd_mask)) {
			int len = recv(socket_fd, &server_response, BUFFER_SIZE, 0);
			if(len == 0)
				break;
			server_response[len] = '\0';
			printf("%s", server_response);
		}
		if(FD_ISSET(stdin_fd, &fd_mask)) {
			char *ret = fgets(message, BUFFER_SIZE, stdin);
			if(ret == NULL)
				break;
			send(socket_fd, ret, strlen(ret), 0);
		}
	}
}

int main(int argc, char *argv[]) {
	int port;
	int client_socket;

	if(argc != 3) {
		fprintf(stderr, "Not enough arguments");
		exit(-1);
	}
	port = strtol(argv[2], NULL, 10);
	client_socket = open_socket(argv[1], port);
	use_socket(client_socket);
	close(client_socket);
	exit(0);
}
