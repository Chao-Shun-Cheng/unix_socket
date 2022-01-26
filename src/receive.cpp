#include "unix_socket/unix_socket.h"

package *information;
bool app_running = true;
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
    unix_socket server;
    server.getconnect(SERVER);
    
    /* setting signal section */

    ros::init(argc, argv, "unix_socket_receive");
    ros::NodeHandle n;

    while(app_running && server.ok && ros::ok()) {
        printf("-----------------------\n");
        char *data = server.receive_msgs();
        information = (package *) data;
        std::cout << "speed : " << information->speed << std::endl;
        free(information);
    }
    server.stop();
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