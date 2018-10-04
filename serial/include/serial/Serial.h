#ifndef SERIAL_H
#define SERIAL_H

#include <ros/ros.h>
#include <serial/wheelPulse.h>
#include <serial/wheelSpeed.h>
#include <serial/CRC_Check.h>  //CRC
#include <serial/global_struct.h>

#include <stdio.h>    //Standard input-output definition
#include <stdlib.h>   //standard function library definition
#include <unistd.h>   //Unix standard function definition
#include <sys/types.h>  //include primitive system data types
#include <sys/stat.h>  //get file attribute
#include <fcntl.h>     //Defined related macros used in file operations
#include <termios.h>   //PPSIX terminal control define
#include <errno.h>     // error number define
#include <sys/ioctl.h>
#include <string.h>    //string dispose function
#include <sys/ioctl.h>  //IO control
#include <iostream>

using namespace std;

#define TRUE 0
#define FALSE -1

#define BAUDRATE        115200
#define UART_DEVICE     "/dev/ttyUSB0" 

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)

class Serial
{
public:
    Serial();
    ~Serial();
    void sendData(const VData &data);
    void receive();
    ros::Publisher pub;
private:
    int set_disp_mode(int fd,int option);
    void set_speed(int fd, int speed);
    int set_Parity(int fd,int databits,int stopbits,int parity);

    //ros function
    // void callBack_wheelspd_(const serial::wheelSpeed::ConstPtr& whlspd);
    void callBack_wheelspd_(const serial::wheelSpeed::ConstPtr& whlspd);

    int serial_port;
    unsigned char tdata[24];
    unsigned char rdata[256];
    static int speed_arr[16];
    static int name_arr[16];

    int32uchar wheelPulse[4];   
    serial::wheelPulse wheelPulse_msg;
    //ros variable
    ros::NodeHandle nh;
    // ros::Publisher pub;
    ros::Subscriber sub;  
};
#endif