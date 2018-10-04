#include <serial/Serial.h>

int Serial::speed_arr[16] = {B115200, B38400, B19200,  B9600,  B4800,  B2400,
                             B1200,   B300,   B115200, B38400, B19200, B9600,
                             B4800,   B2400,  B1200,   B300};
int Serial::name_arr[16] = {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,
                            115200, 38400, 19200, 9600, 4800, 2400, 1200, 300};

Serial::Serial() {
  // serial open and configuration
  serial_port = open(UART_DEVICE, O_RDWR);
  if (serial_port < 0) {
    perror(UART_DEVICE);
    exit(1);
  }
  printf("Open...\n");
  set_speed(serial_port, 115200);
  if (set_Parity(serial_port, 8, 1, 'N') == FALSE) {
    printf("Set Parity Error\n");
    exit(0);
  }
  set_disp_mode(serial_port, 0);
  printf("Open successed\n");

  // ros
  pub = nh.advertise<serial::wheelPulse>("serial/wheel_pulse_", 64);
  sub = nh.subscribe<serial::wheelSpeed>("serial/wheel_speed_", 64,
                                         &Serial::callBack_wheelspd_, this);
}

Serial::~Serial() {
  close(serial_port);
  std::cout << "close serial ok!" << std::endl;
}

// send data
void Serial::sendData(const VData &data) {
  tdata[0] = 0xEB;
  tdata[1] = 0x90;

  tdata[2] = data.wheelSpeed[0].uc[0];
  tdata[3] = data.wheelSpeed[0].uc[1];
  tdata[4] = data.wheelSpeed[0].uc[2];
  tdata[5] = data.wheelSpeed[0].uc[3];

  tdata[6] = data.wheelSpeed[1].uc[0];
  tdata[7] = data.wheelSpeed[1].uc[1];
  tdata[8] = data.wheelSpeed[1].uc[2];
  tdata[9] = data.wheelSpeed[1].uc[3];

  tdata[10] = data.wheelSpeed[2].uc[0];
  tdata[11] = data.wheelSpeed[2].uc[1];
  tdata[12] = data.wheelSpeed[2].uc[2];
  tdata[13] = data.wheelSpeed[2].uc[3];

  tdata[14] = data.wheelSpeed[3].uc[0];
  tdata[15] = data.wheelSpeed[3].uc[1];
  tdata[16] = data.wheelSpeed[3].uc[2];
  tdata[17] = data.wheelSpeed[3].uc[3];

  std::cout << std::endl;
  std::cout << "wheelSpeed1= " << data.wheelSpeed[0].d_32 << std::endl;
  std::cout << "wheelSpeed2= " << data.wheelSpeed[1].d_32 << std::endl;
  std::cout << "wheelSpeed3= " << data.wheelSpeed[2].d_32 << std::endl;
  std::cout << "wheelSpeed4= " << data.wheelSpeed[3].d_32 << std::endl;
  std::cout << std::endl;

  Append_CRC16_Check_Sum(tdata, 20);

  write(serial_port, tdata, 20);
}

// receive data
void Serial::receive() {
  int bytes;
  ioctl(serial_port, FIONREAD, &bytes);
  if (bytes == 0)
    return;
  
  cout << "bytes= " << bytes << endl;

  bytes = read(serial_port, rdata, 21); //\r\n
  cout << "rdata= " <<(int)rdata[0] << endl;
  cout << "rdata= " <<(int)rdata[12] << endl;
  if (rdata[0] == 0xeb && rdata[1] == 0x91 &&
      Verify_CRC16_Check_Sum(rdata, 20)) {
    wheelPulse[0].uc[0] = rdata[2];
    wheelPulse[0].uc[1] = rdata[3];
    wheelPulse[0].uc[2] = rdata[4];
    wheelPulse[0].uc[3] = rdata[5];

    wheelPulse[1].uc[0] = rdata[6];
    wheelPulse[1].uc[1] = rdata[7];
    wheelPulse[1].uc[2] = rdata[8];
    wheelPulse[1].uc[3] = rdata[9];

    wheelPulse[2].uc[0] = rdata[10];
    wheelPulse[2].uc[1] = rdata[11];
    wheelPulse[2].uc[2] = rdata[12];
    wheelPulse[2].uc[3] = rdata[13];

    wheelPulse[3].uc[0] = rdata[14];
    wheelPulse[3].uc[1] = rdata[15];
    wheelPulse[3].uc[2] = rdata[16];
    wheelPulse[3].uc[3] = rdata[17];

    wheelPulse_msg.wheelPulse1 = wheelPulse[0].d_32;
    wheelPulse_msg.wheelPulse2 = wheelPulse[1].d_32;
    wheelPulse_msg.wheelPulse3 = wheelPulse[2].d_32;
    wheelPulse_msg.wheelPulse4 = wheelPulse[3].d_32;

    cout << "wheelPulse[0]= " << wheelPulse[0].d_32 << endl;
  }
}

int Serial::set_disp_mode(int fd, int option) {
  int err;
  struct termios term;
  if (tcgetattr(fd, &term) == -1) {
    perror("Cannot get the attribution of the terminal");
    return 1;
  }
  if (option)
    term.c_lflag |= ECHOFLAGS;
  else
    term.c_lflag &= ~ECHOFLAGS;
  err = tcsetattr(fd, TCSAFLUSH, &term);
  if (err == -1 && err == EINTR) {
    perror("Cannot set the attribution of the terminal");
    return 1;
  }
  return 0;
}

void Serial::set_speed(int fd, int speed) {
  int i;
  int status;
  struct termios Opt;
  tcgetattr(fd, &Opt);
  for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
    if (speed == name_arr[i]) {
      tcflush(fd, TCIOFLUSH);
      cfsetispeed(&Opt, speed_arr[i]);
      cfsetospeed(&Opt, speed_arr[i]);
      status = tcsetattr(fd, TCSANOW, &Opt);
      if (status != 0) {
        perror("tcsetattr fd1");
        return;
      }
      tcflush(fd, TCIOFLUSH);
    }
  }
}

int Serial::set_Parity(int fd, int databits, int stopbits, int parity) {
  struct termios options;
  if (tcgetattr(fd, &options) != 0) {
    perror("SetupSerial 1");
    return (FALSE);
  }
  options.c_cflag &= ~CSIZE;
  switch (databits) {
  case 7:
    options.c_cflag |= CS7;
    break;
  case 8:
    options.c_cflag |= CS8;
    break;
  default:
    fprintf(stderr, "Unsupported data size\n");
    return (FALSE);
  }
  switch (parity) {
  case 'n':
  case 'N':
    options.c_cflag &= ~PARENB; /* Clear parity enable */
    options.c_iflag &= ~INPCK;  /* Enable parity checking */
    break;
  case 'o':
  case 'O':
    options.c_cflag |= (PARODD | PARENB); /* */
    options.c_iflag |= INPCK;             /* Disnable parity checking */
    break;
  case 'e':
  case 'E':
    options.c_cflag |= PARENB;  /* Enable parity */
    options.c_cflag &= ~PARODD; /* */
    options.c_iflag |= INPCK;   /* Disnable parity checking */
    break;
  case 'S':
  case 's': /*as no parity*/
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    break;
  default:
    fprintf(stderr, "Unsupported parity\n");
    return (FALSE);
  }
  /* */
  switch (stopbits) {
  case 1:
    options.c_cflag &= ~CSTOPB;
    break;
  case 2:
    options.c_cflag |= CSTOPB;
    break;
  default:
    fprintf(stderr, "Unsupported stop bits\n");
    return (FALSE);
  }
  /* Set input parity option */
  if (parity != 'n')
    options.c_iflag |= INPCK;
  tcflush(fd, TCIFLUSH);
  options.c_cc[VTIME] = 150; /* 15 seconds*/
  options.c_cc[VMIN] = 0;    /* Update the options and do it NOW */
  if (tcsetattr(fd, TCSANOW, &options) != 0) {
    perror("SetupSerial 3");
    return (FALSE);
  }
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
  options.c_oflag &= ~OPOST;                          /*Output*/
  return (TRUE);
}

// subcrible wheelSpeed
void Serial::callBack_wheelspd_(const serial::wheelSpeed::ConstPtr &whlspd) {
  VData sdata;

  sdata.wheelSpeed[0].d_32 = whlspd->wheelSpeed1;
  sdata.wheelSpeed[1].d_32 = whlspd->wheelSpeed2;
  sdata.wheelSpeed[2].d_32 = whlspd->wheelSpeed3;
  sdata.wheelSpeed[3].d_32 = whlspd->wheelSpeed4;

  sendData(sdata);
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "serial_node");
  Serial serial_;
  // test
  serial::wheelPulse whlspd_msg_;
  whlspd_msg_.wheelPulse1 = 1;
  whlspd_msg_.wheelPulse2 = 2;
  whlspd_msg_.wheelPulse3 = 3;
  whlspd_msg_.wheelPulse4 = 4;
  serial_.pub.publish(whlspd_msg_);

  ros::Rate loop_rate(1000); // 1ms
  while (ros::ok()) {
    serial_.receive();
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
