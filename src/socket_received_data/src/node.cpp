
#include <ros/ros.h>

// ROS messages
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include <nav_msgs/Odometry.h>

// Ethernet
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

// Packet structure
#include "dispatch.h"

// UINT16_MAX is not defined by default in Ubuntu Saucy
#ifndef UINT16_MAX
#define UINT16_MAX (65535)
#endif
#define QUEUE 20
#define BUFFER_SIZE 2048


static inline bool openSocket(const std::string &interface, const std::string &ip_addr, 
                              uint16_t port, int *fd_ptr,int *conn_ptr, 
                              sockaddr_in *sock_ptr,sockaddr_in *client_addr)
{
 
  // Create TCP socket
  int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

  
  memset(sock_ptr, 0, sizeof(sockaddr_in));

  sock_ptr->sin_family = AF_INET;
  sock_ptr->sin_port = htons(port);
  sock_ptr->sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (sockaddr*)sock_ptr, sizeof(sockaddr)) == -1) { 
    ROS_INFO("bind failed!!");
    return false;  }
  else{ 
    *fd_ptr = fd; }

  if (listen(fd,QUEUE) == -1){
    ROS_INFO("listen failed!!");
    return false;}   

  //客户端套字
   socklen_t len = sizeof(sockaddr);

  int conn = accept(fd, (sockaddr*)&client_addr, &len);

  if(conn == -1){
    ROS_INFO("connet failed!!");
    return false;}
  else{ 
    *conn_ptr = conn;
     }

  return true;
}

static inline int readSocket(int conn, VeloBlockObj *socket_recv)
{
  if (conn >= 0) {

    
    int needRecv = sizeof(VeloBlockObj);
    VeloBlockObj *socket_recv = (VeloBlockObj *)malloc(needRecv);
    


    char *buffer = (char *)malloc(needRecv);
    int pos = 0;
    int len ;
    while(pos<needRecv)
    {
        ROS_INFO("needRecv: %d,pos: %d",needRecv,pos);
        len = recv(conn, buffer, BUFFER_SIZE, 0);//接收
        ROS_INFO("len: %d",len);
        if(len <= 0)
        {
            perror("recv error!");
            break;
        }
        pos += len;
    }
    memcpy(socket_recv,buffer,needRecv);
    return true;

  }
  return -1;
}

static inline double SQUARE(double x)
{
  return x * x;
}

static inline void handlePacket()
{

}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "socket_received_data");
  ros::NodeHandle node;
  ros::NodeHandle priv_nh("~");

  std::string interface = "";
  priv_nh.getParam("interface", interface);

  std::string ip_addr = "";
  priv_nh.getParam("ip_address", ip_addr);

  int port = 5001;
  priv_nh.getParam("port", port);

  std::string frame_id = "cross_data";
  priv_nh.getParam("frame_id", frame_id);

  std::string frame_id_vel = "barrier";
  priv_nh.getParam("frame_id_vel", frame_id_vel);

  if (port > UINT16_MAX) {
    ROS_ERROR("Port %u greater than maximum value of %u", port, UINT16_MAX);
  }

  if (interface.length() && ip_addr.length()) {
    ROS_INFO("Preparing to listen on interface %s port %u for packets from ip %s", interface.c_str(), port, ip_addr.c_str());
  } else if (interface.length()) {
    ROS_INFO("Preparing to listen on interface %s port %u", interface.c_str(), port);
  } else if (ip_addr.length()) {
    ROS_INFO("Preparing to listen on port %u for packets from ip %s", port, ip_addr.c_str());
  } else {
    ROS_INFO("Preparing to listen on port %u", port);
  }

  int fd,conn;
  sockaddr_in sock;
  sockaddr_in client;
  if (openSocket(interface, ip_addr, port, &fd, &conn, &sock,&client)) 
  {
    // Set up Publishers                                                       
    // ros::Publisher pub_barrier = node.advertise<std::string>("barrier/location", 2);
    ROS_INFO("open socket success!");

    // Variables 
    VeloBlockObj * packet;
    sockaddr source;
    bool first = true;  
    

    // Loop until shutdown
    while (ros::ok()) {
      // ROS_INFO("ros ok!");
      if (readSocket(conn,  packet)){
        if (true) {//validatePacket(&packet)
          if (first) {
            first = false;
            ROS_INFO("Connected to 3dparty data at %s:%u", inet_ntoa(((sockaddr_in*)&source)->sin_addr), htons(((sockaddr_in*)&source)->sin_port));
          }
          // handlePacket(&packet, pub_fix, pub_vel, pub_imu, pub_odom, frame_id, frame_id_vel);
          
          ROS_INFO("ID: %d x: %f y: %f  z: %f" ,packet->id,packet->x,packet->y,packet->z);

          free(packet);
          
          
        }
      }

      // handle callbacks
      ros::spinOnce();
    }

    // Close socket
    close(fd);
    close(conn);
  } else {
    ROS_FATAL("Failed to open socket");
    ros::WallDuration(1.0).sleep();
  }

  return 0;
}
