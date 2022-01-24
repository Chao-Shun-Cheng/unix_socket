#include "ros/ros.h"
#include "unix_socket/unix_socket.h"

bool app_running = true;
char app_sigaltstack[SIGSTKSZ];
void app_signal_handler(int sig_num);
int app_setup_signals();

int main(int argc, char **argv)
{
    int ret = app_setup_signals();
    if (ret == -1) {
        printf("Fail to app_setup_signals\n");
        return -1;
    }

    ros::init(argc, argv, "unix_socket");
    ros::NodeHandle n;
    // ros::NodeHandle private_nh_("~");
    // private_nh_.param<bool>("ros2dsrc", distance_threshold_, 3);
    // ros::Subscriber sub1 = n.subscribe("/ndt_pose", 1, ndt_callback);
    
    unix_socket client;
    client.getconnect();
    while(app_running && client.ok) {
        client.receive_msgs();
    }
    client.stop();
    return 0;
}

void app_signal_handler(int sig_num)
{
    app_running = false;
}

int app_setup_signals()
{
    stack_t sigstack;
    struct sigaction sa;
    int ret = -1;

    sigstack.ss_sp = app_sigaltstack;
    sigstack.ss_size = SIGSTKSZ;
    sigstack.ss_flags = 0;
    if (sigaltstack(&sigstack, NULL) == -1) {
        perror("signalstack()");
        goto END;
    }

    sa.sa_handler = app_signal_handler;
    sa.sa_flags = SA_ONSTACK;
    if (sigaction(SIGINT, &sa, NULL) != 0) {
        perror("sigaction");
        goto END;
    }
    if (sigaction(SIGTERM, &sa, NULL) != 0) {
        perror("sigaction");
        goto END;
    }

    ret = 0;
END:
    return ret;
}