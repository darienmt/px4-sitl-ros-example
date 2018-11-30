#include "drone_offboard.h"
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/CommandTOL.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/PositionTarget.h>

Drone::Drone()
{
  getHomeGeoPoint();
  getAltitude();
  set_vel_pub_ = nh_.advertise<mavros_msgs::PositionTarget>("mavros/setpoint_raw/local", 10);
  takeoff_client_ = nh_.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/takeoff");
  set_mode_client_ = nh_.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");
  land_client_ = nh_.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/land");
}

Drone::~Drone()
{
}

bool Drone::arm()
{
  if (!home_set_)
  {
    ROS_ERROR("Can't arm: No GPS Fix!");
    return false;
  }

  auto arming_client = nh_.serviceClient<mavros_msgs::CommandBool>("/mavros/cmd/arming");
  mavros_msgs::CommandBool srv_arm;
  srv_arm.request.value = true;
  if (arming_client.call(srv_arm) && srv_arm.response.success)
    return true;
  else
    return false;
}

bool Drone::takeoff()
{
  if (!home_set_)
  {
    ROS_ERROR("Can't takeoff: No GPS Fix!");
    return false;
  }

  mavros_msgs::CommandTOL srv_takeoff{};
  srv_takeoff.request.altitude = altitude_in_amsl_;
  srv_takeoff.request.latitude = home_.geo.latitude;
  srv_takeoff.request.longitude = home_.geo.longitude;

  if (takeoff_client_.call(srv_takeoff) && srv_takeoff.response.success)
    return true;
  else
    return false;
}

bool Drone::land()
{
  if (!home_set_)
  {
    ROS_ERROR("Can't land: No GPS Fix!");
    return false;
  }

  mavros_msgs::CommandTOL srv_land{};

  if (land_client_.call(srv_land) && srv_land.response.success)
    return true;
  else
    return false;
}

bool Drone::setOffboardMode()
{
  mavros_msgs::SetMode offboard_set_mode;
  offboard_set_mode.request.custom_mode = "OFFBOARD";
  if (set_mode_client_.call(offboard_set_mode) && offboard_set_mode.response.mode_sent)
    return true;
  else
    return false;
}

void Drone::getHomeGeoPoint()
{
  // FCU Home position: See http://docs.ros.org/api/sensor_msgs/html/msg/NavSatFix.html
  auto home_sub = nh_.subscribe<mavros_msgs::HomePosition>("mavros/home_position/home", 1,
                                                           boost::bind(&Drone::setHomeGeoPointCB, this, _1));

  ROS_INFO("Waiting for Aero FC Home to be set...");
  while (ros::ok() && !home_set_)
  {
    ros::spinOnce();
    rate_.sleep();
  }
}

// Callback that gets called periodically from MAVROS notifying Global Poistion of Aero FCU
void Drone::setHomeGeoPointCB(const mavros_msgs::HomePositionConstPtr& home)
{
  home_ = *home;
  home_set_ = true;
  ROS_INFO("Received Home (WGS84 datum): %lf, %lf, %lf", home_.geo.latitude, home_.geo.longitude, home_.geo.altitude);
}

void Drone::getAltitude()
{
  ros::Subscriber altitude_sub =
      nh_.subscribe<mavros_msgs::Altitude>("/mavros/altitude", 1, boost::bind(&Drone::getAltitudeCB, this, _1));
  while (ros::ok() && !altitude_received_)
  {
    ros::spinOnce();
    rate_.sleep();
  }
}

void Drone::getAltitudeCB(const mavros_msgs::AltitudeConstPtr& altitude)
{
  altitude_ = *altitude;
  altitude_received_ = true;
  altitude_in_amsl_ = altitude_.amsl;
}

float Drone::toRadFromDeg(float deg)
{
  return static_cast<float>(deg / 180.0f * M_PI);
}

void Drone::setOffboardVelocityBody(float vx, float vy, float vz, float yaw_rate, std::size_t count)
{
  mavros_msgs::PositionTarget pos{};

  pos.coordinate_frame = mavros_msgs::PositionTarget::FRAME_BODY_NED;
  pos.type_mask = mavros_msgs::PositionTarget::IGNORE_PX | mavros_msgs::PositionTarget::IGNORE_PY |
                  mavros_msgs::PositionTarget::IGNORE_PZ | mavros_msgs::PositionTarget::IGNORE_AFX |
                  mavros_msgs::PositionTarget::IGNORE_AFY | mavros_msgs::PositionTarget::IGNORE_AFZ |
                  mavros_msgs::PositionTarget::FORCE | mavros_msgs::PositionTarget::IGNORE_YAW;
  pos.velocity.x = vx;
  pos.velocity.y = vy;
  pos.velocity.z = vz;
  pos.yaw_rate = toRadFromDeg(yaw_rate);

  for (; count > 0; count--)
  {
    set_vel_pub_.publish(pos);
    rate_.sleep();
  }
}

void Drone::setOffboardVelocityNED(float vx, float vy, float vz, float yaw, std::size_t count)
{
  mavros_msgs::PositionTarget pos{};

  pos.coordinate_frame = mavros_msgs::PositionTarget::FRAME_LOCAL_NED;
  pos.type_mask = mavros_msgs::PositionTarget::IGNORE_PX | mavros_msgs::PositionTarget::IGNORE_PY |
                  mavros_msgs::PositionTarget::IGNORE_PZ | mavros_msgs::PositionTarget::IGNORE_AFX |
                  mavros_msgs::PositionTarget::IGNORE_AFY | mavros_msgs::PositionTarget::IGNORE_AFZ |
                  mavros_msgs::PositionTarget::FORCE | mavros_msgs::PositionTarget::IGNORE_YAW_RATE;
  pos.velocity.x = vx;
  pos.velocity.y = vy;
  pos.velocity.z = vz;
  pos.yaw = toRadFromDeg(yaw);

  for (; count > 0; count--)
  {
    set_vel_pub_.publish(pos);
    rate_.sleep();
  }
}