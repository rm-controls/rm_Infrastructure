//
// Created by llljjjqqq on 22-11-4.
//

#include "rm_referee/ui/trigger_change_ui.h"

namespace rm_referee
{
void TriggerChangeUi::setContent(const std::string& content)
{
  graph_->setContent(content);
  display();
}

void TriggerChangeUi::display()
{
  graph_->setOperation(rm_referee::GraphOperation::UPDATE);
  graph_->display();
  graph_->sendUi(ros::Time::now());
}

void ChassisTriggerChangeUi::display()
{
  if (s_l_ == rm_msgs::DbusData::MID && s_r_ == rm_msgs::DbusData::UP)
    updateConfig(chassis_mode_, false, 1, false);
  else
    updateConfig(chassis_mode_, power_limit_state_ == rm_common::PowerLimit::BURST, 0,
                 power_limit_state_ == rm_common::PowerLimit::CHARGE);
  graph_->setOperation(rm_referee::GraphOperation::UPDATE);
  graph_->displayTwice(true);
  graph_->sendUi(ros::Time::now());
}

void ChassisTriggerChangeUi::displayInCapacity()
{
  if (key_ctrl_ && key_shift_ && key_b_ && base_.robot_id_ != rm_referee::RobotId::RED_ENGINEER &&
      base_.robot_id_ != rm_referee::RobotId::BLUE_ENGINEER)
    updateConfig(254, 0);
  graph_->setOperation(rm_referee::GraphOperation::UPDATE);
  graph_->displayTwice(true);
  graph_->sendUi(ros::Time::now());
}

void ChassisTriggerChangeUi::updateConfig(uint8_t main_mode, bool main_flag, uint8_t sub_mode, bool sub_flag)
{
  if (main_mode == 254)
  {
    graph_->setContent("Cap reset");
    graph_->setColor(rm_referee::GraphColor::YELLOW);
    return;
  }
  graph_->setContent(getChassisState(main_mode));
  if (main_flag)
    graph_->setColor(rm_referee::GraphColor::ORANGE);
  else if (sub_flag)
    graph_->setColor(rm_referee::GraphColor::GREEN);
  else if (sub_mode == 1)
    graph_->setColor(rm_referee::GraphColor::PINK);
  else
    graph_->setColor(rm_referee::GraphColor::WHITE);
}

std::string ChassisTriggerChangeUi::getChassisState(uint8_t mode)
{
  if (mode == rm_msgs::ChassisCmd::RAW)
    return "raw";
  else if (mode == rm_msgs::ChassisCmd::FOLLOW)
    return "follow";
  else if (mode == rm_msgs::ChassisCmd::TWIST)
    return "twist";
  else
    return "error";
}

void ChassisTriggerChangeUi::updateChassisCmdData(const rm_msgs::ChassisCmd::ConstPtr data)
{
  chassis_mode_ = data->mode;
  display();
}

void ChassisTriggerChangeUi::updateManualCmdData(const rm_msgs::ManualToReferee::ConstPtr data)
{
  power_limit_state_ = data->power_limit_state;
}

void ChassisTriggerChangeUi::updateDbusData(const rm_msgs::DbusData::ConstPtr data)
{
  s_l_ = data->s_l;
  s_r_ = data->s_r;
  key_ctrl_ = data->key_ctrl;
  key_shift_ = data->key_shift;
  key_b_ = data->key_b;
}

void ChassisTriggerChangeUi::updateCapacityData(const rm_msgs::CapacityData data)
{
  displayInCapacity();
}

void ShooterTriggerChangeUi::display()
{
  updateConfig(shooter_mode_, 0, shoot_frequency_, false);
  TriggerChangeUi::display();
}

void ShooterTriggerChangeUi::updateConfig(uint8_t main_mode, bool main_flag, uint8_t sub_mode, bool sub_flag)
{
  graph_->setContent(getShooterState(main_mode));
  if (sub_mode == rm_common::HeatLimit::LOW)
    graph_->setColor(rm_referee::GraphColor::WHITE);
  else if (sub_mode == rm_common::HeatLimit::HIGH)
    graph_->setColor(rm_referee::GraphColor::YELLOW);
  else if (sub_mode == rm_common::HeatLimit::BURST)
    graph_->setColor(rm_referee::GraphColor::ORANGE);
}

std::string ShooterTriggerChangeUi::getShooterState(uint8_t state)
{
  if (state == rm_msgs::ShootState::STOP)
    return "stop";
  else if (state == rm_msgs::ShootState::READY)
    return "ready";
  else if (state == rm_msgs::ShootState::PUSH)
    return "push";
  else if (state == rm_msgs::ShootState::BLOCK)
    return "block";
  else
    return "error";
}

void ShooterTriggerChangeUi::updateShootStateData(const rm_msgs::ShootState::ConstPtr& data)
{
  shooter_mode_ = data->state;
  display();
}

void ShooterTriggerChangeUi::updateManualCmdData(rm_msgs::ManualToReferee::ConstPtr data)
{
  shoot_frequency_ = data->shoot_frequency;
}

void GimbalTriggerChangeUi::display()
{
  updateConfig(gimbal_mode_, gimbal_eject_);
  graph_->setOperation(rm_referee::GraphOperation::UPDATE);
  graph_->displayTwice(true);
  graph_->sendUi(ros::Time::now());
}

void GimbalTriggerChangeUi::updateConfig(uint8_t main_mode, bool main_flag, uint8_t sub_mode, bool sub_flag)
{
  graph_->setContent(getGimbalState(main_mode));
  if (main_flag)
    graph_->setColor(rm_referee::GraphColor::ORANGE);
  else
    graph_->setColor(rm_referee::GraphColor::WHITE);
}

std::string GimbalTriggerChangeUi::getGimbalState(uint8_t mode)
{
  if (mode == rm_msgs::GimbalCmd::DIRECT)
    return "direct";
  else if (mode == rm_msgs::GimbalCmd::RATE)
    return "rate";
  else if (mode == rm_msgs::GimbalCmd::TRACK)
    return "track";
  else
    return "error";
}

void GimbalTriggerChangeUi::updateGimbalCmdData(const rm_msgs::GimbalCmd::ConstPtr data)
{
  gimbal_mode_ = data->mode;
  display();
}

void GimbalTriggerChangeUi::updateManualCmdData(const rm_msgs::ManualToReferee::ConstPtr data)
{
  gimbal_eject_ = data->gimbal_eject;
}

void TargetTriggerChangeUi::display()
{
  if (base_.robot_id_ != rm_referee::RobotId::BLUE_HERO && base_.robot_id_ != rm_referee::RobotId::RED_HERO)
    updateConfig(det_target_, shoot_frequency_ == rm_common::HeatLimit::BURST, det_armor_target_,
                 det_color_ == rm_msgs::StatusChangeRequest::RED);
  else
    updateConfig(gimbal_eject_, shoot_frequency_, det_armor_target_, det_color_ == rm_msgs::StatusChangeRequest::RED);
  TriggerChangeUi::display();
}

void TargetTriggerChangeUi::updateConfig(uint8_t main_mode, bool main_flag, uint8_t sub_mode, bool sub_flag)
{
  graph_->setContent(getTargetState(main_mode, sub_mode));
  if (main_flag)
    graph_->setColor(rm_referee::GraphColor::ORANGE);
  else if (sub_flag)
    graph_->setColor(rm_referee::GraphColor::PINK);
  else
    graph_->setColor(rm_referee::GraphColor::CYAN);
}

std::string TargetTriggerChangeUi::getTargetState(uint8_t target, uint8_t armor_target)
{
  if (base_.robot_id_ != rm_referee::RobotId::BLUE_HERO && base_.robot_id_ != rm_referee::RobotId::RED_HERO)
  {
    if (target == rm_msgs::StatusChangeRequest::BUFF)
      return "buff";
    else if (target == rm_msgs::StatusChangeRequest::ARMOR && armor_target == rm_msgs::StatusChangeRequest::ARMOR_ALL)
      return "armor_all";
    else if (target == rm_msgs::StatusChangeRequest::ARMOR &&
             armor_target == rm_msgs::StatusChangeRequest::ARMOR_OUTPOST_BASE)
      return "armor_base";
    else
      return "error";
  }
  else
  {
    if (target == 1)
      return "eject";
    else if (armor_target == rm_msgs::StatusChangeRequest::ARMOR_ALL)
      return "all";
    else if (armor_target == rm_msgs::StatusChangeRequest::ARMOR_OUTPOST_BASE)
      return "base";
    else
      return "error";
  }
}

void TargetTriggerChangeUi::updateManualCmdData(const rm_msgs::ManualToReferee::ConstPtr data)
{
  det_target_ = data->det_target;
  shoot_frequency_ = data->shoot_frequency;
  det_armor_target_ = data->det_armor_target;
  det_color_ = data->det_color;
  gimbal_eject_ = data->gimbal_eject;
}

void TargetTriggerChangeUi::updateShootStateData(const rm_msgs::ShootState::ConstPtr& data)
{
  display();
}

void PolygonTriggerChangeGroupUi::display()
{
  for (auto graph : graph_vector_)
  {
    graph.second->setOperation(rm_referee::GraphOperation::UPDATE);
    graph.second->display();
    graph.second->sendUi(ros::Time::now());
  }
}

void SentryInteractiveDataTriggerChangeUi::display()
{
  updateConfig(sentry_data_.state, 0);
  graph_->setOperation(rm_referee::GraphOperation::UPDATE);
  graph_->displayTwice(true);
  graph_->sendUi(ros::Time::now());
}

void SentryInteractiveDataTriggerChangeUi::updateConfig(uint8_t main_mode, bool main_flag, uint8_t sub_mode,
                                                        bool sub_flag)
{
  graph_->setContent(getSentryState(main_mode));
}

std::string SentryInteractiveDataTriggerChangeUi::getSentryState(uint8_t mode)
{
  if (mode == rm_msgs::SentryData::CRUISE)
    return "CRUISE";
  else if (mode == rm_msgs::SentryData::CRUISE_GYRO)
    return "CRUISE_GYRO";
  else
    return "error";
}

void SentryInteractiveDataTriggerChangeUi::sendSentryData(const rm_msgs::SentryData::ConstPtr data)
{
  sentry_data_.state = data->state;
  if (base_.robot_id_ < 100)
  {
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::RED_SENTRY, data->state);
  }
  else if (base_.robot_id_ > 100)
  {
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::BLUE_SENTRY, data->state);
  }
}
void SentryInteractiveDataTriggerChangeUi::sendInteractiveResult(const rm_msgs::SentryData::ConstPtr data)
{
  sentry_data_.state = data->state;
  if (base_.robot_id_ < 100)
  {
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::RED_HERO, data->state);
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::RED_STANDARD_3, data->state);
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::RED_STANDARD_4, data->state);
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::RED_STANDARD_5, data->state);
  }
  else if (base_.robot_id_ > 100)
  {
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::BLUE_HERO, data->state);
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::BLUE_STANDARD_3, data->state);
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::BLUE_STANDARD_4, data->state);
    sentry_interactive_sender_->sendInteractiveData(rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN +
                                                        rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA,
                                                    rm_msgs::GameRobotStatus::BLUE_STANDARD_5, data->state);
  }
}
void SentryInteractiveDataTriggerChangeUi::updateInteractiveData(const rm_referee::InteractiveData& interactive_data,
                                                                 const ros::Time& time)
{
  if ((interactive_data.header_data_.data_cmd_id_ !=
       rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN + rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA) ||
      (base_.robot_id_ != rm_msgs::GameRobotStatus::RED_SENTRY ||
       base_.robot_id_ != rm_msgs::GameRobotStatus::BLUE_SENTRY))
    return;
  sentry_data_.state = interactive_data.data_;
  sentry_state_pub_.publish(sentry_data_);
}
void SentryInteractiveDataTriggerChangeUi::updateInteractiveResult(const rm_referee::InteractiveData& interactive_data,
                                                                   const ros::Time& time)
{
  if ((interactive_data.header_data_.data_cmd_id_ !=
       rm_referee::DataCmdId::ROBOT_INTERACTIVE_CMD_MIN + rm_msgs::SentryData ::SENTRY_INTERACTIVE_DATA) ||
      (base_.robot_id_ == rm_msgs::GameRobotStatus::RED_SENTRY ||
       base_.robot_id_ == rm_msgs::GameRobotStatus::BLUE_SENTRY))
    return;
  sentry_data_.state = interactive_data.data_;
  display();
}
}  // namespace rm_referee
