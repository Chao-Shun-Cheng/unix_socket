#include "unix_socket/unix_socket.h"

package *information;
std_msgs::Float32 speed;
std_msgs::Float32 pedal;
bool app_running = true;
char app_sigaltstack[SIGSTKSZ];
void app_signal_handler(int sig_num);
void generateBoundingBox(autoware_msgs::DetectedObjectArray &output, const geometry_msgs::Pose vehicle_pose);
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
    ros::Publisher pub_pose = n.advertise<autoware_msgs::DetectedObjectArray>("leading_car/objects", 1);
    ros::Publisher pub_velocity = n.advertise<std_msgs::Float32>("leading_car_velocity", 1);
    ros::Publisher pub_throttle = n.advertise<std_msgs::Float32>("leading_car_throttle", 1);

    while(app_running && server.ok && ros::ok()) {
        printf("-----------------------\n");
        char *data = server.receive_msgs();
        information = (package *) data;
        printf("Frame : %d\n", information->count);

        autoware_msgs::DetectedObjectArray output;
        generateBoundingBox(output, information->vehicle_pose);
        pub_pose.publish(output);
        speed.data = information->velocity;
        pub_velocity.publish(speed);
        pedal.data = information->throttle;
        pub_throttle.publish(pedal);
        free(information);
    }
    server.stop();
    return 0;
}

void app_signal_handler(int sig_num)
{
    app_running = false;
}

void generateBoundingBox(autoware_msgs::DetectedObjectArray &output, const geometry_msgs::Pose vehicle_pose)
{
    autoware_msgs::DetectedObject object;
    object.header.stamp = ros::Time::now();
    object.header.frame_id = "world";
    object.label = "Leading Vehicle";
    object.pose = vehicle_pose;
    object.dimensions.x = 3.6;
    object.dimensions.y = 1.8;
    object.dimensions.z = 1.7;
    object.pose_reliable = true;
    output.header = object.header;
    output.objects.push_back(object);
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