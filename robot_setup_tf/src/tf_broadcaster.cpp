//
// Created by ubuntu on 18-9-25.
//
#include <ros/ros.h>
#include <tf/transform_broadcaster.h>

int main(int argc, char **argv)
{
    ros::init(argc, argv, "robot_tf_broadcaster");
    ros::NodeHandle n;

    ros::Rate r(100);
// create a TransformBroadcaster object
    static tf::TransformBroadcaster broadcaster;

    while (ros::ok()) {
        tf::Transform transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.1, 0.0, 0.2));
        broadcaster.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "base_link", "base_laser"));
        r.sleep();
    }

    return 0;
}
