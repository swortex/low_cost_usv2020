cmake_minimum_required(VERSION 2.8.3)
project(usv_gnc)
#2024 4 4

## Compile as C++11, supported in ROS Kinetic and newer
 add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  gps_common
  message_generation
  tf2_ros
  tf2
  tf
)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)
##add_message_files(FILES Joy.msg)
generate_messages(DEPENDENCIES  std_msgs sensor_msgs)
catkin_package(CATKIN_DEPENDS geometry_msgs sensor_msgs roscpp
rospy std_msgs message_runtime)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES uri_usv
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
#include_directories(
# include
#  ${catkin_INCLUDE_DIRS}
#)

# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
## Build talker and listener
include_directories(
include
/usr/include
 ${catkin_INCLUDE_DIRS})
 
link_directories(
/usr/lib 
)

## Specify libraries to link a library or executable target against
# target_link_libraries(${PROJECT_NAME}_node
#   ${catkin_LIBRARIES}
# )

#add_executable(rc_pwm src/rc_pwm.cpp)
#target_link_libraries(rc_pwm roboticscape ${catkin_LIBRARIES})

add_executable(heading_PID src/heading_PID.cpp)
target_link_libraries(heading_PID roboticscape ${catkin_LIBRARIES})

add_executable(heading_PID_GPS src/heading_PID_GPS.cpp)
target_link_libraries(heading_PID_GPS roboticscape ${catkin_LIBRARIES})

add_executable(heartbeat_sender src/heartbeat_sender.cpp)
target_link_libraries(heartbeat_sender roboticscape ${catkin_LIBRARIES})

#add_executable(utmtest src/utmtest.cpp)
#target_link_libraries(utmtest roboticscape ${catkin_LIBRARIES})

add_executable(auto_pwm src/auto_pwm.cpp)
target_link_libraries(auto_pwm roboticscape ${catkin_LIBRARIES})


add_executable(guidance_node
  include/guidance/guidance.h
  src/guidance.cpp
  src/guidance_node.cpp
)
add_dependencies(guidance_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(guidance_node ${catkin_LIBRARIES})
