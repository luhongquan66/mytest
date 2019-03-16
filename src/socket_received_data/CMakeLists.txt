cmake_minimum_required(VERSION 2.8.3)
project(socket_received_data)


find_package(catkin REQUIRED COMPONENTS roscpp)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  geometry_msgs
  nav_msgs
  std_msgs
  tf
)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)


catkin_package(
  #CATKIN_DEPENDS  roscpp  rospy  geometry_msgs  nav_msgs  std_msgs  
  #DEPENDS EIGEN3 PCL

)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include 
  ${catkin_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
)




add_executable(socket_pub_node
  src/node.cpp
)
add_dependencies(socket_pub_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

target_link_libraries(socket_pub_node
  ${catkin_LIBRARIES}
)