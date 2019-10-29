#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 1000
#define BUFFER_SIZE 4096
#define LISTEN_MAX 10

int client_sockets[MAX_CLIENTS];

/*sends message for every available client except for except_id
	except_id = 0 for no exceptions
*/
int broadcast_message(char *msg, int except_fd) {
	int i, client_fd;
	for(i = 0; i < MAX_CLIENTS; i++) {
		client_fd = client_sockets[i];
		if(client_fd > 0 && client_fd != except_fd)
			send(client_fd, msg, strlen(msg), 0);
	}
	return 1;
}

void signal_handler(int t) {
	int i, client_fd;
	for(i = 0; i < MAX_CLIENTS; i++) {
		client_fd = client_sockets[i];
		if(client_fd > 0)
			close(client_fd);
	}
	exit(0);
}


int main(int argc, char **argv) {

	char buffer[BUFFER_SIZE];
	struct sockaddr_in serverSocket;
	serverSocket.sin_family = AF_INET;
	serverSocket.sin_port = htons(atoi(argv[1]));
	serverSocket.sin_addr.s_addr = INADDR_ANY;
	fd_set readfds;
	int max_fd;
	int client_fd;
	FD_ZERO(&readfds);
	int listener_fd;
	if((listener_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit(-1);
	if(bind(listener_fd, (struct sockaddr *)&serverSocket, sizeof(serverSocket)) < 0)
		exit(-1);
	if(listen(listener_fd, LISTEN_MAX) < 0)
		exit(-1);
	int serverSocketSize =  sizeof(serverSocket); 
	struct sigaction sigact;
	sigact.sa_handler = signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	int i;

	if(argc > 2)
		exit(-1);

	for(i = 0; i < MAX_CLIENTS; i++) {
		client_sockets[i] = 0;
	}

	printf("Listening on port: %s\n", argv[1]);

	while(1) {
		fflush(stdout);
		FD_ZERO(&readfds);
		FD_SET(listener_fd, &readfds);
		max_fd = listener_fd;
		for(i = 0; i < MAX_CLIENTS; i++) {
			client_fd = client_sockets[i];
			if(client_fd > 0)
				FD_SET(client_fd, &readfds);
			if(client_fd > max_fd)
				max_fd = client_fd;
		}

		select(max_fd + 1, &readfds, NULL, NULL, NULL);

		/*new user joined*/
		if(FD_ISSET(listener_fd, &readfds)) {
			client_fd = accept(listener_fd, (struct sockaddr *)&serverSocket, (socklen_t*)&serverSocketSize);
			char joined_msg[BUFFER_SIZE];
			snprintf(joined_msg, sizeof(joined_msg), "%s:%d joined.\n", \
				inet_ntoa(serverSocket.sin_addr), ntohs(serverSocket.sin_port));
			printf("%s", joined_msg);
			for(i = 0; i < MAX_CLIENTS; i++) {
				if(client_sockets[i] == 0) {
					client_sockets[i] = client_fd;
					break;
				}
			}
			broadcast_message(joined_msg, 0);
		}

		for(i = 0; i < MAX_CLIENTS; i++) {
			client_fd = client_sockets[i];
			/*message received from some user*/
			if(FD_ISSET(client_fd, &readfds)) {
				int msg_length = read(client_fd, buffer, sizeof(buffer));

				/*received end-of-file*/
				if(msg_length == 0) {
					fflush(stdout);
					getpeername(client_fd, (struct sockaddr*)&serverSocket, (socklen_t*)&serverSocketSize);
					char left_msg[BUFFER_SIZE];
					snprintf(left_msg, sizeof(left_msg), "%s:%d left.\n", \
						inet_ntoa(serverSocket.sin_addr), ntohs(serverSocket.sin_port));
					printf("%s", left_msg);
					close(client_fd);
					client_sockets[i] = 0;
					broadcast_message(left_msg, 0);
				}
					/*received a normal user message*/
				else {
					buffer[msg_length] = '\0';
					getpeername(client_fd, (struct sockaddr*)&serverSocket, (socklen_t*)&serverSocketSize);
					char usr_msg[BUFFER_SIZE];
					snprintf(usr_msg, sizeof(usr_msg), "%s:%d %s", \
						inet_ntoa(serverSocket.sin_addr), ntohs(serverSocket.sin_port), buffer);
					printf("%s", usr_msg);
					broadcast_message(usr_msg, client_fd);
				}
			}
		}
	}

	return 0;
}