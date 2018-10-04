#include <ros/ros.h>
#include <geometry_msgs/PointStamped.h>
#include <tf/transform_listener.h>
int fun(int a, int b)
{
    ROS_INFO_STREAM(a+b << std::endl);
}

void transformPoint(const tf::TransformListener &listener)
{
    //    boost::function and boost::bind
    boost::function<int (int, int)> f = boost::bind<int>(&fun, _1,_2);
    f(100, 10);
    geometry_msgs::PointStamped laser_point;
    laser_point.header.frame_id = "base_laser";
    laser_point.header.stamp = ros::Time();

    laser_point.point.x = 1.0;
    laser_point.point.y = 0.2;
    laser_point.point.z = 0.0;

    try {
        geometry_msgs::PointStamped base_point;
        listener.transformPoint("base_link", laser_point, base_point);

        ROS_INFO_STREAM(
            "base_laser: " << laser_point.point.x << "," << laser_point.point.y << "," << laser_point.point.z
                           << "----> " << "base_link: " << base_point.point.x << "," << base_point.point.y << ","
                           << base_point.point.z  << std::endl << "stamp:" << base_point.header.stamp.toSec());
    }
    catch(tf::ExtrapolationException &e) {
        ROS_ERROR_STREAM("Received an exception trying to transform a point from \"base_laser\" to \"base_link\":" << e.what());

    }

}

int main(int argc, char *argv[])
{

    ros::init(argc, argv, "robot_tf_listener");
    ros::NodeHandle n;

    tf::TransformListener listener(ros::Duration(10));

    ros::Timer time = n.createTimer(ros::Duration(1.0), boost::bind(&transformPoint, boost::ref(listener)));

    ros::spin();
    return 0;
}
