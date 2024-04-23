#include <ros/ros.h>
#include <ros/time.h>
#include <stdlib.h>
#include <iostream>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/AttitudeTarget.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Twist.h>
#include <aruco_msgs/Detector_vel.h>
#include <prometheus_msgs/ControlCommand.h>
#include "uav_msgs/feedback_delay.h"

#define NODE_NAME "uav_auto_takeoff"

using namespace std;

prometheus_msgs::ControlCommand Command_to_pub;
aruco_msgs::Detector_vel detector_cmd;
mavros_msgs::AttitudeTarget attitude_setpoint;
mavros_msgs::State mav_state;
uav_msgs::feedback_delay uav_feedback_msg;

int rate_hz = 100;
int duration1 = rate_hz * 5; // rate*5
int duration2 = rate_hz * 6; // rate*6
double reveived_timestamp = 0;
double forward_delay = 0;

void detector_vel_cb(const aruco_msgs::Detector_vel::ConstPtr &msg)
{
    detector_cmd = *msg;

    // reveived_timestamp = detector_cmd.timestamp;
    // forward_delay = ros::Time::now().toSec() - reveived_timestamp;
}

void mav_state_cb(const mavros_msgs::State::ConstPtr &msg)
{
    mav_state = *msg;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "uav_auto_takeoff");
    ros::NodeHandle nh;
    ros::Rate rate(rate_hz);

    ros::Publisher command_pub = nh.advertise<prometheus_msgs::ControlCommand>("/prometheus/control_command", 25);
    ros::Publisher attitude_pub = nh.advertise<mavros_msgs::AttitudeTarget>("/mavros/setpoint_raw/attitude", 50);
    // ros::Publisher feedback_pub = nh.advertise<uav_msgs::feedback_delay>("/uav_feedback/delay", 10);

    ros::Subscriber detector_vel_sub = nh.subscribe<aruco_msgs::Detector_vel>("/aruco_single/detector_vel", 25, detector_vel_cb);
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, mav_state_cb);
    // ros::Subscriber attitued_loopback_sub = nh.subscribe<mavros_msgs::AttitudeTarget>("/mavros/setpoint_raw/target_attitude", 50);

    Command_to_pub.Mode = prometheus_msgs::ControlCommand::Idle;
    Command_to_pub.Command_ID = 0;
    Command_to_pub.source = NODE_NAME;
    Command_to_pub.Reference_State.Move_mode = prometheus_msgs::PositionReference::XYZ_POS;
    Command_to_pub.Reference_State.Move_frame = prometheus_msgs::PositionReference::ENU_FRAME;
    Command_to_pub.Reference_State.position_ref[0] = 0;
    Command_to_pub.Reference_State.position_ref[1] = 0;
    Command_to_pub.Reference_State.position_ref[2] = 0;
    Command_to_pub.Reference_State.velocity_ref[0] = 0;
    Command_to_pub.Reference_State.velocity_ref[1] = 0;
    Command_to_pub.Reference_State.velocity_ref[2] = 0;
    Command_to_pub.Reference_State.acceleration_ref[0] = 0;
    Command_to_pub.Reference_State.acceleration_ref[1] = 0;
    Command_to_pub.Reference_State.acceleration_ref[2] = 0;
    Command_to_pub.Reference_State.yaw_ref = 0;

    cout.setf(ios::fixed);
    // setprecision(n) 设显示小数精度为n位
    cout << setprecision(2);
    // 左对齐
    cout.setf(ios::left);
    // 强制显示小数点
    cout.setf(ios::showpoint);

    while (ros::ok() && !mav_state.connected)
    {
        ros::spinOnce();

        // ==================== delay ====================
        // uav_feedback_msg.forward_delay = forward_delay;
        // uav_feedback_msg.timestamp = ros::Time::now().toSec();
        // feedback_pub.publish(uav_feedback_msg);
        // ==================== delay ====================

        rate.sleep();
    }

    // ==================== delay ====================
    // uav_feedback_msg.forward_delay = forward_delay;
    // uav_feedback_msg.timestamp = ros::Time::now().toSec();
    // feedback_pub.publish(uav_feedback_msg);
    // ==================== delay ====================

    int offboard_flag;
    cout << "Input 1 to switch to offboard mode and arm the drone." << endl;
    cin >> offboard_flag;

    if (offboard_flag == 1)
    {
        Command_to_pub.header.stamp = ros::Time::now();
        Command_to_pub.Mode = prometheus_msgs::ControlCommand::Idle;
        Command_to_pub.Command_ID = Command_to_pub.Command_ID + 1;
        Command_to_pub.source = NODE_NAME;
        Command_to_pub.Reference_State.yaw_ref = 999;
        command_pub.publish(Command_to_pub);
        Command_to_pub.Reference_State.yaw_ref = 0.0;
        offboard_flag = 0;
    }

    int take_off_flag;
    cout << "Input 1 to takeoff." << endl;
    cin >> take_off_flag;

    int i = 0;
    if (take_off_flag == 1)
    {
        while (i < duration1)
        {
            Command_to_pub.header.stamp = ros::Time::now();
            Command_to_pub.Mode = prometheus_msgs::ControlCommand::Takeoff;
            Command_to_pub.Command_ID = Command_to_pub.Command_ID + 1;
            Command_to_pub.source = NODE_NAME;
            command_pub.publish(Command_to_pub);
            take_off_flag = 0;

            // ==================== delay ====================
            // uav_feedback_msg.forward_delay = forward_delay;
            // uav_feedback_msg.timestamp = ros::Time::now().toSec();
            // feedback_pub.publish(uav_feedback_msg);
            // ==================== delay ====================

            rate.sleep();
            ROS_INFO("Taking off:%d", i);
            i++;
        }
    }
    /*
    int move_flag;
    cout << "Input 1 to move to predefined position." << endl;
    cin >> move_flag;

    if (move_flag != 1)
    {
        return -1;
    }
    */
    i = 0;
    while (i < duration2)
    {
        ros::spinOnce();
        if (detector_cmd.flag == 1)
        {
            break;
        }
        Command_to_pub.header.stamp = ros::Time::now();
        Command_to_pub.Mode = prometheus_msgs::ControlCommand::Move;
        Command_to_pub.Command_ID = Command_to_pub.Command_ID + 1;
        Command_to_pub.source = NODE_NAME;
        Command_to_pub.Reference_State.Move_mode = 0;
        Command_to_pub.Reference_State.Move_frame = 0;
        Command_to_pub.Reference_State.position_ref[0] = 2.0;
        Command_to_pub.Reference_State.position_ref[1] = 0.0;
        Command_to_pub.Reference_State.position_ref[2] = 1.2;
        Command_to_pub.Reference_State.yaw_ref = 0.0;
        command_pub.publish(Command_to_pub);

        // ==================== delay ====================
        // uav_feedback_msg.forward_delay = forward_delay;
        // uav_feedback_msg.timestamp = ros::Time::now().toSec();
        // feedback_pub.publish(uav_feedback_msg);
        // ==================== delay ====================

        rate.sleep();
        ROS_INFO("Move ENU:%d", i);
        i++;
    }

    while (ros::ok())
    {
        ros::spinOnce();
        Command_to_pub.header.stamp = ros::Time::now();
        Command_to_pub.source = NODE_NAME;

        // ==================== delay ====================
        // uav_feedback_msg.forward_delay = forward_delay;
        // uav_feedback_msg.timestamp = ros::Time::now().toSec();
        // feedback_pub.publish(uav_feedback_msg);
        // ==================== delay ====================

        if (detector_cmd.flag == 0)
        {
            Command_to_pub.Mode = prometheus_msgs::ControlCommand::Hold;
            Command_to_pub.Command_ID = Command_to_pub.Command_ID + 1;
            command_pub.publish(Command_to_pub);
            cout << "Hold" << endl;
        }

        if (detector_cmd.flag == 3)
        {
            Command_to_pub.Mode = prometheus_msgs::ControlCommand::Disarm;
            Command_to_pub.Command_ID = Command_to_pub.Command_ID + 1;
            command_pub.publish(Command_to_pub);
            cout << "Disarm" << endl;
        }

        if (detector_cmd.flag == 1 || detector_cmd.flag == 2)
        {
            Command_to_pub.attitude_control = detector_cmd.attitude_control_flag;

            if (Command_to_pub.attitude_control == true)
            {
                attitude_setpoint.header.stamp = ros::Time::now();

                attitude_setpoint.type_mask = detector_cmd.attitude_control_type_mask;
                attitude_setpoint.thrust = detector_cmd.thrust;

                attitude_setpoint.orientation.w = detector_cmd.quat.w;
                attitude_setpoint.orientation.x = detector_cmd.quat.x;
                attitude_setpoint.orientation.y = detector_cmd.quat.y;
                attitude_setpoint.orientation.z = detector_cmd.quat.z;

                attitude_setpoint.body_rate.x = detector_cmd.body_rate.x;
                attitude_setpoint.body_rate.y = detector_cmd.body_rate.y;
                attitude_setpoint.body_rate.z = detector_cmd.body_rate.z;

                attitude_pub.publish(attitude_setpoint);

                cout << "Attitude Control" << endl;
            }
            else
            {

                Command_to_pub.Mode = prometheus_msgs::ControlCommand::Move;
                Command_to_pub.Command_ID = Command_to_pub.Command_ID + 1;
                Command_to_pub.Reference_State.Move_mode = Command_to_pub.Reference_State.XYZ_VEL;
                Command_to_pub.Reference_State.Move_frame = Command_to_pub.Reference_State.ENU_FRAME;
                Command_to_pub.Reference_State.yaw_ref = 0.0;

                Command_to_pub.Reference_State.velocity_ref[0] = detector_cmd.vel.linear.x;
                Command_to_pub.Reference_State.velocity_ref[1] = detector_cmd.vel.linear.y;
                Command_to_pub.Reference_State.velocity_ref[2] = detector_cmd.vel.linear.z;

                Command_to_pub.Reference_State.acceleration_ref[0] = detector_cmd.accel_sp.x;
                Command_to_pub.Reference_State.acceleration_ref[1] = detector_cmd.accel_sp.y;
                Command_to_pub.Reference_State.acceleration_ref[2] = detector_cmd.accel_sp.z;

                Command_to_pub.Reference_State.Yaw_Rate_Mode = true;
                Command_to_pub.Reference_State.yaw_rate_ref = detector_cmd.yaw_rate_ref;

                command_pub.publish(Command_to_pub);

                cout << "Move ENU" << endl;
            }
        }
        rate.sleep();
    }

    cout << "...................................................";

    return 0;
}