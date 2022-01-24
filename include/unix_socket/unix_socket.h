#include <signal.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>

#ifndef IPC_H
#define IPC_H
#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "server.sock"
#endif

class unix_socket
{
private:
    int sock;
    struct sockaddr_un addr;
    
public:
    bool ok;
    unix_socket();
    void stop();
    void getconnect();
    void send_msgs(uint8_t *buf, int size);
    uint8_t *receive_msgs();
};