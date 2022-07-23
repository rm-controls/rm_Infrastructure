//
// Created by ljq on 2022/5/17.
//

#pragma once

#include "rm_referee/robot_referee.h"
#include "rm_common/decision/calibration_queue.h"

#include <std_srvs/Empty.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/Float64.h>
#include <rm_msgs/EngineerAction.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <utility>

namespace rm_referee
{
class EngineerReferee : public RobotReferee
{
public:
  explicit EngineerReferee(ros::NodeHandle& nh, Data& data);
  void run() override;
  void interactiveDataCallBack(const rm_referee::InteractiveData& interactive_data_,
                               const ros::Time& last_get_) override;
  void jointStateCallback(const sensor_msgs::JointState::ConstPtr& joint_state) override;
  void actuatorStateCallback(const rm_msgs::ActuatorState::ConstPtr& data) override;
  void cardCmdDataCallback(const rm_msgs::StateCmd::ConstPtr& data) override;
  void engineerCmdDataCallback(const rm_msgs::EngineerCmd ::ConstPtr& data) override;

private:
  bool symbol;
  rm_common::CalibrationQueue* power_on_calibration_{};
};

}  // namespace rm_referee
