// Copyright (c) 2021 Beijing Xiaomi Mobile Software Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef SENSOR_MANAGER__SENSOR_MANAGER_HPP_
#define SENSOR_MANAGER__SENSOR_MANAGER_HPP_
#include <sensor_msgs/msg/range.hpp>
#include <functional>
#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include <memory>
#include "pluginlib/class_loader.hpp"
#include "bcmgps_base/bcmgps_base.hpp"
#include "protocol/msg/gps_payload.hpp"
#include "lidar_base/lidar_base.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "manager_base/manager_base.hpp"
#include "ultrasonic_base/ultrasonic_base.hpp"
#include "tof_base/tof_base.hpp"
#include "protocol/msg/single_tof_payload.hpp"
#include "protocol/msg/head_tof_payload.hpp"
#include "protocol/msg/rear_tof_payload.hpp"
#include "rclcpp/rclcpp.hpp"
#include "protocol/srv/sensor_operation.hpp"
#include "cyberdog_machine/cyberdog_fs_machine.hpp"
#include "cyberdog_machine/cyberdog_heartbeats.hpp"

namespace cyberdog
{
namespace sensor
{
class SensorManager final : public cyberdog::machine::MachineActuator
{
  using ScanMsg = sensor_msgs::msg::LaserScan;                        // [topic 类型]激光数据

public:
  explicit SensorManager(const std::string & name);
  ~SensorManager();

  void Config();
  bool Init();
  void Run();
  int32_t SelfCheck();

public:
  int32_t OnError();
  int32_t OnLowPower();
  int32_t OnSuspend();
  int32_t OnProtected();
  int32_t OnActive();
  int32_t OnDeActive();
  int32_t OnSetUp();
  int32_t ONTearDown();
  int32_t OnOTA();

private:
  bool IsStateValid();

  template<typename T>
  bool SensorOperation(T elem, uint8_t oper_id);

private:
  std::string name_;
  std::vector<std::string> simulator_;
  rclcpp::Node::SharedPtr node_ptr_ {nullptr};
  std::unique_ptr<cyberdog::machine::HeartBeatsActuator> heart_beats_ptr_ {nullptr};
  rclcpp::executors::MultiThreadedExecutor executor;

private:
  std::shared_ptr<cyberdog::sensor::GpsBase> gps_;
  rclcpp::Publisher<protocol::msg::GpsPayload>::SharedPtr gps_publisher_;
  void gps_payload_callback(std::shared_ptr<protocol::msg::GpsPayload> msg);

  std::shared_ptr<cyberdog::sensor::LidarBase> lidar_;
  rclcpp::Publisher<ScanMsg>::SharedPtr lidar_publisher_;
  void lidar_payload_callback(std::shared_ptr<ScanMsg> msg);

  std::shared_ptr<cyberdog::sensor::UltrasonicBase> ultrasonic_;
  rclcpp::Publisher<sensor_msgs::msg::Range>::SharedPtr ultrasonic_publisher_;
  void ultrasonic_payload_callback(std::shared_ptr<sensor_msgs::msg::Range> msg);

  std::shared_ptr<cyberdog::sensor::TofBase> tof_;
  rclcpp::Publisher<protocol::msg::HeadTofPayload>::SharedPtr head_tof_publisher_;
  void head_tof_payload_callback(std::shared_ptr<protocol::msg::HeadTofPayload> msg);
  rclcpp::Publisher<protocol::msg::RearTofPayload>::SharedPtr rear_tof_publisher_;
  void rear_tof_payload_callback(std::shared_ptr<protocol::msg::RearTofPayload> msg);

  rclcpp::Service<protocol::srv::SensorOperation>::SharedPtr sensor_operation_srv_;
  void sensor_operation(
    const protocol::srv::SensorOperation::Request::SharedPtr request,
    protocol::srv::SensorOperation::Response::SharedPtr response);

private:
  const std::string Uninitialized_V = std::string("Uninitialized");
  const std::string SetUp_V = std::string("SetUp");
  const std::string TearDown_V = std::string("TearDown");
  const std::string SelfCheck_V = std::string("SelfCheck");
  const std::string Active_V = std::string("Active");
  const std::string DeActive_V = std::string("DeActive");
  const std::string Protected_V = std::string("Protected");
  const std::string LowPower_V = std::string("LowPower");
  const std::string OTA_V = std::string("OTA");
  const std::string Error_V = std::string("Error");
};  // class SensorManager
}  // namespace sensor
}  // namespace cyberdog

#endif  // SENSOR_MANAGER__SENSOR_MANAGER_HPP_
