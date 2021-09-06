cmake_minimum_required(VERSION 3.0.2)
project(led_controller)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  actionlib_msgs
  rospy
  std_msgs
  message_generation
)

add_message_files(
  FILES
  Color.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

# What other packages will need to use this package
catkin_package(
  CATKIN_DEPENDS rospy message_runtime
)
