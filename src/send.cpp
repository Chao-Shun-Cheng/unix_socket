#include "unix_socket/unix_socket.h"

package information;
bool app_running = true;
bool receive_pose = false;
bool receive_can = false;
char app_sigaltstack[SIGSTKSZ];
void app_signal_handler(int sig_num);
int app_setup_signals();

void ndt_callback(const geometry_msgs::PoseStamped &input)
{
    receive_pose = true;
    information.vehicle_pose = input;
    return;
}

void caninfo_callback(const autoware_can_msgs::CANInfo &input)
{
    receive_can = true;
    information.speed = input.speed;
    return;
}

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

    ros::init(argc, argv, "unix_socket_send");
    ros::NodeHandle n;
    
    ros::Subscriber pose_sub = n.subscribe("/ndt_pose", 1, ndt_callback);
    ros::Subscriber can_sub = n.subscribe("/can_info", 1, caninfo_callback);
    
    ros::Rate loop_rate(10);

    while(app_running && client.ok && ros::ok()) {
        ros::spinOnce();
        if(receive_can && receive_pose) {
            printf("-----------------------\n");
            char *data = tool::package2char(&information);
            client.send_msgs(data, sizeof(information));
            free(data);
        } 
        loop_rate.sleep();
    }
    client.stop();
    return 0;
}

void app_signal_handler(int sig_num)
{
    app_running = false;
    // client.stop();
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