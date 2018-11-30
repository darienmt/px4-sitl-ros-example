#pragma once

#include <ros/ros.h>
#include <mavros_msgs/HomePosition.h>
#include <mavros_msgs/Altitude.h>

class Drone
{
public:
  Drone();
  ~Drone();

  // Actions
  bool arm();
  bool takeoff();
  bool land();
  bool setOffboardMode();

  // Offboard Velocity Control
  void setOffboardVelocityNED(float vx, float vy, float vz, float yaw, std::size_t count = 1);
  void setOffboardVelocityBody(float vx, float vy, float vz, float yaw_rate, std::size_t count = 1);

private:
  void getHomeGeoPoint();
  void getAltitude();
  void setHomeGeoPointCB(const mavros_msgs::HomePositionConstPtr& home);
  void getAltitudeCB(const mavros_msgs::AltitudeConstPtr& altitude);
  float toRadFromDeg(float deg);

  ros::NodeHandle nh_;
  mavros_msgs::HomePosition home_{};
  mavros_msgs::Altitude altitude_{};
  ros::Publisher set_vel_pub_;
  ros::ServiceClient takeoff_client_;
  ros::ServiceClient set_mode_client_;
  ros::ServiceClient land_client_;
  bool home_set_ = false;
  bool altitude_received_ = false;
  float altitude_in_amsl_ = 0.0f;
  ros::Rate rate_ = ros::Rate(20.0);
};