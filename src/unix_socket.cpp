#include "unix_socket/unix_socket.h"

unix_socket::unix_socket() 
{
    if ((sock = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		ok = 0;
	} 
    if (ok) {
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, CLIENT_SOCK_FILE);
		unlink(CLIENT_SOCK_FILE);
		if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("bind");
			ok = 0;
		} else {
            std::cout << "socket suceess\n";
        }
	}
}

void unix_socket::getconnect()
{
    if (ok) {
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, SERVER_SOCK_FILE);
		if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
			perror("connect");
			ok = 0;
		} else {
            std::cout << "get connection.\n";
        }
	}
}

void unix_socket::send_msgs(uint8_t *buf, int size)
{
    if (ok) {
		if (send(sock, (void*) buf, size, 0) == -1) {
			perror("send");
			ok = 0;
		}
		printf ("sent message to dsrc\n");
	}
}

uint8_t *unix_socket::receive_msgs()
{
    if (ok) {
        int len;
        uint8_t *rx_buf = (uint8_t*) malloc(sizeof(uint8_t) * 1460);
		if ((len = recv(sock, rx_buf, 1460, 0)) < 0) {
			perror("recv");
			ok = 0;
		} else {
            printf ("receive message from dsrc\n", len);
            return rx_buf;
        }
	} else {
        return 0;
    }
}

void unix_socket::stop()
{
    close(sock);
    std::cout << "stop socket.\n";
}