#include <signal.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <std_msgs/Float64.h>
#include "autoware_msgs/DetectedObject.h"
#include "autoware_msgs/DetectedObjectArray.h"
#include "autoware_can_msgs/CANInfo.h"
#ifndef IPC_H
#define IPC_H
#define CLIENT_SOCK_FILE "/tmp/client.sock"
#define SERVER_SOCK_FILE "/tmp/server.sock"
#endif

struct package {
    int count;
    geometry_msgs::Pose vehicle_pose;
    double velocity;
};

enum app_type { SERVER = 0, CLIENT = 1 };

class unix_socket
{
private:
    int sock;
    struct sockaddr_un addr;
    
public:
    bool ok = true;;
    unix_socket();
    void stop();
    void getconnect(app_type type);
    void send_msgs(char *buf, int size);
    char *receive_msgs();
};

