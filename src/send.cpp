#include "unix_socket/unix_socket.h"

package information;
bool app_running = true;
bool receive_pose = false;
bool receive_can = false;
char app_sigaltstack[SIGSTKSZ];
void app_signal_handler(int sig_num);
int app_setup_signals();

tf::StampedTransform ndt2map;

/**
  * This function can transform pose from local frame to map frame.
**/
geometry_msgs::Pose getTransformedPose(const geometry_msgs::Pose &in_pose, const tf::StampedTransform &tf_stamp)
{
    tf::Transform transform;
    geometry_msgs::PoseStamped out_pose;
    transform.setOrigin(tf::Vector3(in_pose.position.x, in_pose.position.y, in_pose.position.z));
    transform.setRotation(tf::Quaternion(in_pose.orientation.x, in_pose.orientation.y, in_pose.orientation.z, in_pose.orientation.w));
    geometry_msgs::PoseStamped pose_out;
    tf::poseTFToMsg(tf_stamp * transform, out_pose.pose);
    return out_pose.pose;
}

/**
  * This function can find the transformation matrix from local frame to map frame.
  * local2global_ will save the matrix including translation and rotation.
**/
bool updateNecessaryTransform(const std::string &input, tf::StampedTransform &local2global_)
{
    bool success = true;
    tf::TransformListener tf_listener_;
    try {
        tf_listener_.waitForTransform(input, "world", ros::Time(0), ros::Duration(1.0));
        tf_listener_.lookupTransform("world", input, ros::Time(0), local2global_);
    } catch (tf::TransformException ex)  // check TF
    {
        ROS_ERROR("%s", ex.what());
        success = false;
    }
    return success;
}

void ndt_callback(const geometry_msgs::PoseStamped &input)
{
    bool success = updateNecessaryTransform(input.header.frame_id, ndt2map);
    if (!success) {
        ROS_INFO("Could not find coordiante transformation for NDT Pose");
        return;
    }
    information.vehicle_pose = getTransformedPose(input.pose, ndt2map);
    receive_pose = true;
    return;
}

void caninfo_callback(const autoware_can_msgs::CANInfo &input)
{
    information.velocity = input.speed;
    receive_can = true;
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
    client.getconnect(CLIENT);
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
            printf("Frame : %d\n", information.count++);
            char *data = (char *) &information;
            client.send_msgs(data, sizeof(package));
        } 
        receive_can = receive_pose = false;
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

