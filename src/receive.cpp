#include "unix_socket/unix_socket.h"

bool app_running = true;
bool receive_pose = false;
bool receive_can = false;
char app_sigaltstack[SIGSTKSZ];
void app_signal_handler(int sig_num);
int app_setup_signals();

int main(int argc, char **argv)
{
    /* setting signal section */
    int ret = app_setup_signals();
    if (ret == -1) {
        printf("Fail to app_setup_signals\n");
        return -1;
    }
    unix_socket client;
    client.getconnect();
    /* setting signal section */

    ros::init(argc, argv, "unix_socket_receive");
    ros::NodeHandle n;
    
    while(app_running && client.ok && ros::ok()) {
        package *data = (package *) client.receive_msgs();
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