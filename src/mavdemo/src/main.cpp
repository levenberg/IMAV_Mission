#include <cstdlib>

#include <ros/ros.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/CommandTOL.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/PositionTarget.h>
#include <std_msgs/String.h>
#include <stdio.h>
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/Vector3Stamped.h"
#include "geometry_msgs/PoseStamped.h"
#include "sensor_msgs/Imu.h"
void localposcb(const geometry_msgs::PoseStamped &msg)
{
   ROS_INFO("local position: %f", msg.pose.position.x );
}

void imu_callback(const sensor_msgs::Imu &msg )
{
  ROS_INFO("Imu data: %f", msg.linear_acceleration.x);
}

int main(int argc, char **argv)
{
  
  int ncount=0;
  ros::init(argc, argv, "mavros_takeoff");
  ros::NodeHandle n;
   
  sleep(10);   //wait for mavros ok
  ros::Subscriber local_pos_sub = n.subscribe("/mavros/local_position/pose", 1, localposcb);
  ros::Subscriber imu_sub =n.subscribe("mavros/imu/data",1, imu_callback);
  
  ros::Publisher chatter_pub = n.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",100);
  ros::Rate loop_rate(20);

  ////////////////////////////////////////////
  /////////////////GUIDED/////////////////////
  ////////////////////////////////////////////
  ros::ServiceClient cl = n.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");
  mavros_msgs::SetMode srv_setMode;
  srv_setMode.request.base_mode = 0;
  srv_setMode.request.custom_mode = "GUIDED";
  if(cl.call(srv_setMode)){
    ROS_ERROR("setmode send ok %d value:", srv_setMode.response.success);
  }else{
    ROS_ERROR("Failed SetMode");
    return -1;
  }
  
  ////////////////////////////////////////////
  ///////////////////ARM//////////////////////
  ////////////////////////////////////////////
  ros::ServiceClient arming_cl = n.serviceClient<mavros_msgs::CommandBool>("/mavros/cmd/arming");
  mavros_msgs::CommandBool srv;
  srv.request.value = true;
  if(arming_cl.call(srv)){
    sleep(1);
    ROS_ERROR("ARM send ok %d", srv.response.success);
  }else{
    ROS_ERROR("Failed arming or disarming");
  }
  
  ////////////////////////////////////////////
  /////////////////TAKEOFF////////////////////
  ////////////////////////////////////////////
  ros::ServiceClient takeoff_cl = n.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/takeoff");
  mavros_msgs::CommandTOL srv_takeoff;
  srv_takeoff.request.altitude = 3;
  srv_takeoff.request.latitude = 0;
  srv_takeoff.request.longitude = 0;
  srv_takeoff.request.min_pitch = 0;
  srv_takeoff.request.yaw = 0;
  if(takeoff_cl.call(srv_takeoff)){
    ROS_ERROR("srv_takeoff send ok %d", srv_takeoff.response.success);
  }else{
    ROS_ERROR("Failed Takeoff");
  }
  sleep(5);
  ////////////////////////////////////////////
  /////////////////DO STUFF///////////////////
  ////////////////////////////////////////////
  

  
  geometry_msgs::PoseStamped msg;
  ros::Time begin=ros::Time::now();
  while(ros::ok()&&ros::Time::now()-begin<ros::Duration(30.0)){
    msg.header.stamp = ros::Time::now();
    ncount++;
    msg.header.seq=ncount;
    msg.pose.position.x = -10;
    msg.pose.position.y = 0;
    msg.pose.position.z = 3;
    msg.pose.orientation.x=0.0;
    msg.pose.orientation.y=0.0;
    msg.pose.orientation.z=0.0;
    msg.pose.orientation.w=0.0;
    /*msg.velocity.x = 0.8;
    msg.velocity.y = 0.8;
    msg.velocity.z = 0.5;
    msg.yaw = 0;
    msg.yaw_rate=0;*/
    ROS_INFO("velocity_setpoint:");
    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
  }    
  
  ////////////////////////////////////////////
  ///////////////////LAND/////////////////////
  ////////////////////////////////////////////
  ros::ServiceClient land_cl = n.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/land");
  mavros_msgs::CommandTOL srv_land;
  srv_land.request.altitude = 3;
  srv_land.request.latitude = 0;
  srv_land.request.longitude = 0;
  srv_land.request.min_pitch = 0;
  srv_land.request.yaw = 0;
  if(land_cl.call(srv_land)){
    ROS_INFO("srv_land send ok %d", srv_land.response.success);
  }else{
    ROS_ERROR("Failed Land");
  }
  
  while (n.ok())
  {
    ros::spinOnce();
    loop_rate.sleep();
  }
  
  return 0;
}

