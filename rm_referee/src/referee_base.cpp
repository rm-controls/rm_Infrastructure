//
// Created by ljq on 2022/5/17.
//

#include "rm_referee/referee_base.h"

namespace rm_referee
{
RefereeBase::RefereeBase(ros::NodeHandle& nh, Base& base) : base_(base), nh_(nh)
{
  RefereeBase::joint_state_sub_ =
      nh.subscribe<sensor_msgs::JointState>("/joint_states", 10, &RefereeBase::jointStateCallback, this);
  RefereeBase::actuator_state_sub_ =
      nh.subscribe<rm_msgs::ActuatorState>("/actuator_states", 10, &RefereeBase::actuatorStateCallback, this);
  RefereeBase::dbus_sub_ = nh.subscribe<rm_msgs::DbusData>("/dbus_data", 10, &RefereeBase::dbusDataCallback, this);
  RefereeBase::chassis_cmd_sub_ = nh.subscribe<rm_msgs::ChassisCmd>("/controllers/chassis_controller/command", 10,
                                                                    &RefereeBase::chassisCmdDataCallback, this);
  RefereeBase::vel2D_cmd_sub_ =
      nh.subscribe<geometry_msgs::Twist>("/cmd_vel", 10, &RefereeBase::vel2DCmdDataCallback, this);
  RefereeBase::shoot_state_sub_ = nh.subscribe<rm_msgs::ShootState>("/controllers/shooter_controller/state", 10,
                                                                    &RefereeBase::shootStateCallback, this);
  RefereeBase::gimbal_cmd_sub_ = nh.subscribe<rm_msgs::GimbalCmd>("/controllers/gimbal_controller/command", 10,
                                                                  &RefereeBase::gimbalCmdDataCallback, this);
  RefereeBase::card_cmd_sub_ = nh.subscribe<rm_msgs::StateCmd>("/controllers/card_controller/command", 10,
                                                               &RefereeBase::cardCmdDataCallback, this);
  RefereeBase::engineer_cmd_sub_ =
      nh.subscribe<rm_msgs::EngineerUi>("/engineer_ui", 10, &RefereeBase::engineerUiDataCallback, this);
  RefereeBase::manual_data_sub_ =
      nh.subscribe<rm_msgs::ManualToReferee>("/manual_to_referee", 10, &RefereeBase::manualDataCallBack, this);
  RefereeBase::exchange_data_sub_ =
      nh.subscribe<rm_msgs::ExchangerMsg>("/pnp_publisher", 10, &RefereeBase::exchangeDataCallBack, this);
  RefereeBase::planning_result_sub_ =
      nh.subscribe<std_msgs::Int32>("/planning_result", 10, &RefereeBase::planningResultDataCallBack, this);
  RefereeBase::camera_name_sub_ = nh.subscribe("/camera_name", 10, &RefereeBase::cameraNameCallBack, this);
  if (base_.robot_id_ == rm_referee::RobotId::RED_RADAR || base_.robot_id_ == rm_referee::RobotId::BLUE_RADAR)
    RefereeBase::radar_date_sub_ =
        nh.subscribe<std_msgs::Int8MultiArray>("/data", 10, &RefereeBase::radarDataCallBack, this);
  XmlRpc::XmlRpcValue rpc_value;
  ros::NodeHandle ui_nh(nh, "ui");
  ui_nh.getParam("trigger_change", rpc_value);
  for (int i = 0; i < rpc_value.size(); i++)
  {
    if (rpc_value[i]["name"] == "chassis")
      chassis_trigger_change_ui_ = new ChassisTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "shooter")
      shooter_trigger_change_ui_ = new ShooterTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "gimbal")
      gimbal_trigger_change_ui_ = new GimbalTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "target")
      target_trigger_change_ui_ = new TargetTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "camera")
      camera_trigger_change_ui_ = new CameraTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "drag")
      drag_trigger_change_ui_ = new DragTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "gripper")
      gripper_trigger_change_ui_ = new GripperTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "exchange")
      exchange_trigger_change_ui_ = new ExchangeTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "planning")
      planning_result_trigger_change_ui_ = new PlanningResultTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "step")
      step_trigger_change_ui_ = new StepTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "reversal")
      reversal_trigger_change_ui_ = new ReversalTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "stone")
      stone_trigger_change_ui_ = new StoneTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "stone")
      stone_trigger_change_ui_ = new StoneTriggerChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "temperature")
      joint_temperature_trigger_change_ui_ = new JointTemperatureTriggerChangeUi(rpc_value[i], base_);
  }

  ui_nh.getParam("time_change", rpc_value);
  for (int i = 0; i < rpc_value.size(); i++)
  {
    if (rpc_value[i]["name"] == "capacitor")
      capacitor_time_change_ui_ = new CapacitorTimeChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "effort")
      effort_time_change_ui_ = new EffortTimeChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "dart_status")
      dart_status_time_change_ui_ = new DartStatusTimeChangeUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "lane_line")
      lane_line_time_change_ui_ = new LaneLineTimeChangeUi(rpc_value[i], base_);
  }

  ui_nh.getParam("fixed", rpc_value);
  fixed_ui_ = new FixedUi(rpc_value, base_);

  ui_nh.getParam("flash", rpc_value);
  for (int i = 0; i < rpc_value.size(); i++)
  {
    if (rpc_value[i]["name"] == "cover")
      cover_flash_ui_ = new CoverFlashUi(rpc_value[i], base_);
    if (rpc_value[i]["name"] == "spin")
      spin_flash_ui_ = new SpinFlashUi(rpc_value[i], base_);
  }

  add_ui_timer_ = nh.createTimer(ros::Duration(0.02), std::bind(&RefereeBase::addUi, this), false, false);
}
void RefereeBase::addUi()
{
  if (add_ui_times_ > 100)
  {
    ROS_INFO("End add");
    add_ui_timer_.stop();
    return;
  }

  ROS_INFO_THROTTLE(0.8, "Adding ui... %.1f%%", (add_ui_times_ / 100.) * 100);
  if (chassis_trigger_change_ui_)
    chassis_trigger_change_ui_->add();
  if (gimbal_trigger_change_ui_)
    gimbal_trigger_change_ui_->add();
  if (shooter_trigger_change_ui_)
    shooter_trigger_change_ui_->add();
  if (target_trigger_change_ui_)
    target_trigger_change_ui_->add();
  if (camera_trigger_change_ui_)
    camera_trigger_change_ui_->add();
  if (drag_trigger_change_ui_)
    drag_trigger_change_ui_->add();
  if (gripper_trigger_change_ui_)
    gripper_trigger_change_ui_->add();
  if (exchange_trigger_change_ui_)
    exchange_trigger_change_ui_->add();
  if (planning_result_trigger_change_ui_)
    planning_result_trigger_change_ui_->add();
  if (step_trigger_change_ui_)
    step_trigger_change_ui_->add();
  if (reversal_trigger_change_ui_)
    reversal_trigger_change_ui_->add();
  if (stone_trigger_change_ui_)
    stone_trigger_change_ui_->add();
  if (joint_temperature_trigger_change_ui_)
    joint_temperature_trigger_change_ui_->add();
  if (fixed_ui_)
    fixed_ui_->add();
  if (effort_time_change_ui_)
    effort_time_change_ui_->add();
  if (dart_status_time_change_ui_)
    dart_status_time_change_ui_->add();
  if (capacitor_time_change_ui_)
    capacitor_time_change_ui_->add();
  if (lane_line_time_change_ui_)
    lane_line_time_change_ui_->add();
  add_ui_times_++;
}

void RefereeBase::robotStatusDataCallBack(const rm_msgs::GameRobotStatus& data, const ros::Time& last_get_data_time)
{
  if (fixed_ui_)
    fixed_ui_->display();
}
void RefereeBase::gameStatusDataCallBack(const rm_msgs::GameStatus& data, const ros::Time& last_get_data_time)
{
}
void RefereeBase::capacityDataCallBack(const rm_msgs::CapacityData& data, ros::Time& last_get_data_time)
{
  if (capacitor_time_change_ui_)
    capacitor_time_change_ui_->updateCapacityData(data, last_get_data_time);
  if (chassis_trigger_change_ui_)
    chassis_trigger_change_ui_->updateCapacityData(data);
}
void RefereeBase::powerHeatDataCallBack(const rm_msgs::PowerHeatData& data, const ros::Time& last_get_data_time)
{
}
void RefereeBase::robotHurtDataCallBack(const rm_msgs::RobotHurt& data, const ros::Time& last_get_data_time)
{
}
void RefereeBase::interactiveDataCallBack(const rm_referee::InteractiveData& data, const ros::Time& last_get_data_time)
{
}
void RefereeBase::eventDataCallBack(const rm_msgs::EventData& data, const ros::Time& last_get_data_time)
{
}
void RefereeBase::jointStateCallback(const sensor_msgs::JointState::ConstPtr& data)
{
  if (effort_time_change_ui_)
    effort_time_change_ui_->updateJointStateData(data, ros::Time::now());
  if (lane_line_time_change_ui_)
    lane_line_time_change_ui_->updateJointStateData(data, ros::Time::now());
}
void RefereeBase::actuatorStateCallback(const rm_msgs::ActuatorState::ConstPtr& data)
{
}
void RefereeBase::dbusDataCallback(const rm_msgs::DbusData::ConstPtr& data)
{
  if (add_ui_flag_ && data->s_r == rm_msgs::DbusData::UP)
  {
    add_ui_flag_ = false;
    add_ui_timer_.start();
    add_ui_times_ = 0;
  }
  if (data->s_r != rm_msgs::DbusData::UP)
  {
    add_ui_flag_ = true;
    add_ui_timer_.stop();
  }
  if (chassis_trigger_change_ui_)
    chassis_trigger_change_ui_->updateDbusData(data);
}
void RefereeBase::chassisCmdDataCallback(const rm_msgs::ChassisCmd::ConstPtr& data)
{
  if (chassis_trigger_change_ui_)
    chassis_trigger_change_ui_->updateChassisCmdData(data);
  if (spin_flash_ui_)
    spin_flash_ui_->updateChassisCmdData(data, ros::Time::now());
}
void RefereeBase::vel2DCmdDataCallback(const geometry_msgs::Twist::ConstPtr& data)
{
}
void RefereeBase::shootStateCallback(const rm_msgs::ShootState::ConstPtr& data)
{
  if (target_trigger_change_ui_)
    target_trigger_change_ui_->updateShootStateData(data);
  if (shooter_trigger_change_ui_)
    shooter_trigger_change_ui_->updateShootStateData(data);
}
void RefereeBase::gimbalCmdDataCallback(const rm_msgs::GimbalCmd::ConstPtr& data)
{
  if (gimbal_trigger_change_ui_)
    gimbal_trigger_change_ui_->updateGimbalCmdData(data);
}
void RefereeBase::cardCmdDataCallback(const rm_msgs::StateCmd::ConstPtr& data)
{
}
void RefereeBase::engineerUiDataCallback(const rm_msgs::EngineerUi::ConstPtr& data)
{
  if (drag_trigger_change_ui_)
    drag_trigger_change_ui_->updateDragUiData(data);
  if (gripper_trigger_change_ui_)
    gripper_trigger_change_ui_->updateGripperUiData(data);
  if (stone_trigger_change_ui_)
    stone_trigger_change_ui_->updateStoneUiData(data);
  if (step_trigger_change_ui_)
    step_trigger_change_ui_->updateStepUiData(data);
  if (reversal_trigger_change_ui_)
    reversal_trigger_change_ui_->updateReversalUiData(data);
  if (joint_temperature_trigger_change_ui_)
    joint_temperature_trigger_change_ui_->updateJointTemperatureUiData(data);
}
void RefereeBase::manualDataCallBack(const rm_msgs::ManualToReferee::ConstPtr& data)
{
  if (chassis_trigger_change_ui_)
    chassis_trigger_change_ui_->updateManualCmdData(data);
  if (shooter_trigger_change_ui_)
    shooter_trigger_change_ui_->updateManualCmdData(data);
  if (gimbal_trigger_change_ui_)
    gimbal_trigger_change_ui_->updateManualCmdData(data);
  if (target_trigger_change_ui_)
    target_trigger_change_ui_->updateManualCmdData(data);
  if (cover_flash_ui_)
    cover_flash_ui_->updateManualCmdData(data, ros::Time::now());
}
void RefereeBase::radarDataCallBack(const std_msgs::Int8MultiArrayConstPtr& data)
{
}
void RefereeBase::cameraNameCallBack(const std_msgs::StringConstPtr& data)
{
  if (camera_trigger_change_ui_)
    camera_trigger_change_ui_->updateCameraName(data);
}
void RefereeBase::exchangeDataCallBack(const rm_msgs::ExchangerMsg::ConstPtr& data)
{
  if (exchange_trigger_change_ui_)
    exchange_trigger_change_ui_->updateExchangeData(data);
}
void RefereeBase::planningResultDataCallBack(const std_msgs::Int32::ConstPtr& data)
{
  if (planning_result_trigger_change_ui_)
    planning_result_trigger_change_ui_->updatePlanningResultData(data);
}
}  // namespace rm_referee
