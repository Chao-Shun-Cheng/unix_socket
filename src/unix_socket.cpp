#include "unix_socket/unix_socket.h"

unix_socket::unix_socket() 
{
    this->sock = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (this->sock < 0) {
		perror("socket");
		this->ok = 0;
	} else {
        std::cout << "socket suceess\n";
    }
}

void unix_socket::getconnect(app_type type)
{
    if (this->ok) {
		memset(&this->addr, 0, sizeof(this->addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, SERVER_SOCK_FILE);
        if(SERVER == type) {
            // unlink(SERVER_SOCK_FILE);
            if (bind(this->sock, (struct sockaddr *) &this->addr, sizeof(this->addr)) < 0) {
                perror("bind");
                ok = 0;
            } else {
                std::cout << "get connection.\n";
            }
        }
        else if(CLIENT == type) {
            if (connect(this->sock, (struct sockaddr *)&this->addr, sizeof(this->addr)) == -1) {
                perror("connect");
                ok = 0;
            } else {
                std::cout << "get connection.\n";
            }
        }
		
	}
}

void unix_socket::send_msgs(char *buf, int size)
{
    if (this->ok) {
		if (send(this->sock, buf, size, 0) == -1) {
			perror("send");
			this->ok = 0;
		}
		printf ("sent message to dsrc : %d\n", size);
	}
}

char *unix_socket::receive_msgs()
{
    if (this->ok) {
        char *rx_buf = (char *) malloc(sizeof(package));
		if ((recv(this->sock, rx_buf, sizeof(package), 0)) < 0) {
			perror("recv");
			ok = 0;
		} else {
            printf ("receive message from dsrc\n");
            return rx_buf;
        }
	} else {
        return 0;
    }
}

void unix_socket::stop()
{
    close(this->sock);
    unlink(SERVER_SOCK_FILE);
    unlink(CLIENT_SOCK_FILE);
    std::cout << "stop socket.\n";
}