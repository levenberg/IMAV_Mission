cmake_minimum_required(VERSION 2.8.3)
project(mavdemo)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs genmsg)

## Generate added messages and services
generate_messages(DEPENDENCIES std_msgs)

## Declare a catkin package
catkin_package()

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(mavdemo src/main.cpp)
target_link_libraries(mavdemo ${catkin_LIBRARIES})
add_dependencies(mavdemo mavdemo_generate_messages_cpp)

install(
	TARGETS mavdemo
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
