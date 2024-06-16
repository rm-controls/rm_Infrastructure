//
// Created by gura on 24-5-29.
//

#pragma once

#include "rm_referee/ui/ui_base.h"

namespace rm_referee
{
class InteractiveSender : public UiBase
{
public:
  explicit InteractiveSender(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::deque<Graph>* graph_queue = nullptr,
                             std::deque<Graph>* character_queue = nullptr)
    : UiBase(rpc_value, base, graph_queue, character_queue){};

  void sendInteractiveData(int data_cmd_id, int receiver_id, unsigned char data);
  void sendRadarInteractiveData(const rm_referee::ClientMapReceiveData& data);
  void sendMapSentryData(const rm_referee::MapSentryData& data);
  void sendSentryCmdData(const rm_msgs::SentryInfoConstPtr& data);
  void sendRadarCmdData(const rm_msgs::RadarInfoConstPtr& data);
};

class CustomInfoSender : public InteractiveSender
{
public:
  explicit CustomInfoSender(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::deque<Graph>* graph_queue = nullptr,
                            std::deque<Graph>* character_queue = nullptr)
    : InteractiveSender(rpc_value, base, graph_queue, character_queue){};
  void sendCustomInfoData(std::wstring data);

protected:
  std::wstring last_custom_info_;
};

class BulletNumShare : public InteractiveSender
{
public:
  explicit BulletNumShare(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::deque<Graph>* graph_queue = nullptr,
                          std::deque<Graph>* character_queue = nullptr)
    : InteractiveSender(rpc_value, base, graph_queue, character_queue){};
  void sendBulletData();
  void updateBulletRemainData(const rm_msgs::BulletAllowance& data);

  ros::Time last_send_time_;
  int bullet_42_mm_num_, bullet_17_mm_num_;
};

}  // namespace rm_referee