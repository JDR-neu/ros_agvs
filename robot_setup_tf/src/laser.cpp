//
// Created by ubuntu on 18-9-26.
//

#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <tf/tf.h>
int main(int argc, char** argv)
{
    ros::init(argc, argv, "laser_scan_publisher");

    ros::NodeHandle n;
    ros::Publisher scan_pub = n.advertise<sensor_msgs::LaserScan>("scan", 50);

    size_t  num_readings = 100;
    double laser_frequency = 40;
    double ranges[num_readings];
    double intensities[num_readings];

    int count = 0;
    ros::Rate r(1.0);

    while (ros::ok())
    {
        for(size_t i= 0; i < num_readings; i++)
        {
            ranges[i] = i;
            intensities[i] = 100 + count;
        }
        ros::Time scan_time = ros::Time::now();
        sensor_msgs::LaserScan scan;
        scan.header.stamp = scan_time;
        scan.header.frame_id = "base_link";
        std::string odom_frame = tf::resolve("robot2", "scan");
        ROS_INFO_STREAM(odom_frame);
        scan.angle_min = -1.57;
        scan.angle_max = 1.57;
        scan.angle_increment = 3.14/ num_readings;
        scan.time_increment = 1/laser_frequency / num_readings;
        scan.range_min = 0.0;
        scan.range_max = 100.0;

        scan.ranges.insert(scan.ranges.end(),ranges, ranges+ num_readings);
        scan.intensities.insert(scan.intensities.end(), intensities, intensities+num_readings);
        scan_pub.publish(scan);
        ++count;
        r.sleep();

    }

    return 0;
}