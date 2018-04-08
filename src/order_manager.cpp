// BSD 3-Clause License

// Copyright (c) 2018, Neel Parikh
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "agile_robotics_industrial_automation/order_manager.hpp"

OrderManager::OrderManager() {
  order_subscriber_ = manager_nh_.subscribe("/ariac/orders", 10,
                                            &OrderManager::orderCallback, this);
  scanned_objects_ = camera_.getParts();
}

OrderManager::~OrderManager() {}

// void OrderManager::orderCallback(const osrf_gear::Order::ConstPtr& order_msg)
// {
//   for (auto& kit : order_msg->kits) {
//     for (auto& object : kit.objects) {
//       order_[object.type].push_back(scanned_objects_[object.type].front());
//       scanned_objects_[object.type].pop_front();
//     }
//   }
// }

void OrderManager::orderCallback(const osrf_gear::Order::ConstPtr& order_msg) {
  order_ = *order_msg;
}

std::string OrderManager::getPartType(std::string object) {
  // ROS_WARN_STREAM("<<<<<<<>>>>>>>>>>"
  //                 << object << ">>>>>>>>>>type:" << typeid(object).name());
  // ROS_INFO_STREAM("here 6>>>>");
  std::string part = scanned_objects_[object].back();
  // ROS_INFO_STREAM("here 7>>>>");
  scanned_objects_[object].pop_back();
  // ROS_INFO_STREAM("here 8>>>>");
  return part;
}

// void OrderManager::executeOrder() {
//   ROS_INFO_STREAM("Executing order...");
//   geometry_msgs::Pose part_pose;
//   bool success;

//   for (auto& i : order_) {
//     ROS_WARN_STREAM(">>>>>>: " << i.first);
//     ros::Duration(1.0).sleep();
//   }

//   ros::spinOnce();
//   ros::Duration(3.0).sleep();
//   geometry_msgs::Pose dropped_pose =
//       camera_.getPartPose("/world", "/agv2_load_point_frame");
//   dropped_pose.position.z += 0.1;

//   for (auto& kit : order_) {
//     ROS_INFO_STREAM(">>>>>>>>" << kit.first);
//     std::list<std::string> parts = kit.second;
//     std::string object;
//     while (!parts.empty()) {
//       ROS_INFO_STREAM("here 1>>>> " << kit.first);
//       object = this->getPartType(kit.first);
//       ROS_INFO_STREAM("here 2>>>>");
//       part_pose = camera_.getPartPose("/world", object);
//       ROS_INFO_STREAM("here 3>>>>");
//       part_pose.position.z = part_pose.position.z + 0.025;
//       robot_.pickPart(part_pose);
//       ROS_INFO_STREAM("here 4>>>>");
//       parts.pop_front();
//       ROS_INFO_STREAM("here 5>>>>");
//       ROS_INFO_STREAM("Part picked: " << object);
//       success = robot_.dropPart(dropped_pose);
//       if (!success) {
//         ROS_WARN_STREAM("Part lost and cannot drop!!!");
//         parts.push_front(this->getPartType(kit.first));
//         ROS_WARN_STREAM("Current part list size: " << parts.size());
//       }
//     }

//     ROS_INFO_STREAM("Part dropped: " << object);
//   }
// }

bool OrderManager::pickAndPlace(const std::string object_type) {
  ROS_INFO_STREAM("here 1>>>> " << object_type);
  std::string object_frame = this->getPartType(object_type);
  auto part_pose = camera_.getPartPose("/world",object_frame);
  robot_.pickPart(part_pose);
  geometry_msgs::Pose drop_pose =
      camera_.getPartPose("/world", "/agv2_load_point_frame");
  drop_pose.position.z += 0.12;

  ROS_INFO_STREAM("here pickAndPlace >>>>");
  auto result = robot_.dropPart(drop_pose);
  return result;
}

void OrderManager::executeOrder() {
  ROS_INFO_STREAM("Executing order...");
  // geometry_msgs::Pose part_pose;
  bool failed;
  // std::string object_frame;
  std::list<std::string> failed_parts;

  ros::spinOnce();
  ros::Duration(1.0).sleep();

  for (auto kit : order_.kits) {
    ROS_INFO_STREAM(">>>>>>>>" << kit.kit_type);
    // std::list<std::string> parts = kit.second;
    // std::string object;
    for (auto& object : kit.objects) {
      // object_frame = this->getPartType(object.type);
      ROS_INFO_STREAM("here 2>>>>");
      // part_pose = camera_.getPartPose("/world", object_frame);
      // ROS_INFO_STREAM("here 3>>>>");
      // part_pose.position.z = part_pose.position.z + 0.025;
      failed = pickAndPlace(object.type);
      if (failed) {
        ROS_WARN_STREAM("Part lost and cannot drop!!!");
        // parts.push_front(this->getPartType(kit.first));
        // ROS_WARN_STREAM("Current part list size: " << parts.size());
        // scanned_objects_[object.type].emplace_back(object_frame);
        ROS_WARN_STREAM("Adding part : " << object.type);
        failed_parts.emplace_back(object.type);
      }
    }

    for (auto& i : failed_parts){
      ROS_WARN_STREAM("part : " << i);
    }

    if (failed_parts.size()) {
      auto it = failed_parts.begin();
      while (!failed_parts.empty()) {
        ROS_WARN_STREAM(
            "Current size of failed_parts is: " << failed_parts.size());
        failed_parts.pop_front();
        failed = pickAndPlace(*it);
        if (failed) {
          failed_parts.emplace_back(*it);
        }
      }
      failed_parts.clear();
    }

    ROS_INFO_STREAM("Part dropped: " << object);
  }
}

// std::map<std::string, std::list<std::string>> OrderManager::getOrder() {
//   return this->order_;
// }
