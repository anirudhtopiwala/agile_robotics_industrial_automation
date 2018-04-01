#pragma once

#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <iostream>
#include <string>

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

#include <osrf_gear/VacuumGripperControl.h>
#include <osrf_gear/VacuumGripperState.h>

class UR10Controller {
 public:
  UR10Controller();
  ~UR10Controller();
  bool planner();
  void execute();
  void goToTarget(const geometry_msgs::Pose& target);
  void sendRobotHome();
  bool dropPart(geometry_msgs::Pose& part_pose);
  void gripperToggle(const bool& state);
  void gripper_callback(const osrf_gear::VacuumGripperState::ConstPtr& grip);
  void gripper_state_check(geometry_msgs::Pose pose);
  void pickPart(geometry_msgs::Pose& part_pose);

 private:
  ros::NodeHandle ur10_nh_;
  ros::ServiceClient gripper_client_;
  ros::NodeHandle gripper_nh_;
  ros::Subscriber gripper_subscriber_;

  tf::TransformListener robot_tf_listener_;
  tf::StampedTransform robot_tf_transform_;
  tf::TransformListener agv_tf_listener_;
  tf::StampedTransform agv_tf_transform_;

  geometry_msgs::Pose target_pose_;

  moveit::planning_interface::MoveGroupInterface robot_move_group_;
  moveit::planning_interface::MoveGroupInterface::Plan robot_planner_;

  osrf_gear::VacuumGripperControl gripper_service_;
  osrf_gear::VacuumGripperState gripper_status_;

  std::string object;
  bool plan_success_;
  std::vector<double> home_joint_pose_;
  geometry_msgs::Pose home_cart_pose_;
  geometry_msgs::Quaternion fixed_orientation_;
  geometry_msgs::Pose agv_position_;
  std::vector<double> end_position_;
  double offset_;
  int counter_;
  bool gripper_state, pick, drop;
};