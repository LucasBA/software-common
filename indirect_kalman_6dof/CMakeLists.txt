# Catkin User Guide: http://www.ros.org/doc/groovy/api/catkin/html/user_guide/user_guide.html
# Catkin CMake Standard: http://www.ros.org/doc/groovy/api/catkin/html/user_guide/standards.html
cmake_minimum_required(VERSION 2.8.3)
project(indirect_kalman_6dof)
# Load catkin and all dependencies required for this package
# TODO: remove all from COMPONENTS that are not catkin packages.
find_package(catkin REQUIRED COMPONENTS nav_msgs std_msgs message_runtime rospy uf_common tf geometry_msgs message_generation sensor_msgs)
# CATKIN_MIGRATION: removed during catkin migration
# cmake_minimum_required(VERSION 2.4.6)

# CATKIN_MIGRATION: removed during catkin migration
# include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
set(ROS_BUILD_TYPE RelWithDebInfo)


# CATKIN_MIGRATION: removed during catkin migration
# rosbuild_init()

#set the default path for built executables to the "bin" directory

# CATKIN_MIGRATION: removed during catkin migration
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory

# CATKIN_MIGRATION: removed during catkin migration
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
add_message_files(
  FILES
  Debug.msg
)
#uncomment if you have defined services
add_service_files(
  FILES
  SetPosition.srv
  SetIgnoreMagnetometer.srv
)
## Generate added messages and services with any dependencies listed here
generate_messages(
    DEPENDENCIES std_msgs geometry_msgs
)

# catkin_package parameters: http://ros.org/doc/groovy/api/catkin/html/dev_guide/generated_cmake_api.html#catkin-package
# TODO: fill in what other packages will need to use this package
catkin_package(
    DEPENDS  # TODO
    CATKIN_DEPENDS nav_msgs std_msgs message_runtime rospy uf_common tf geometry_msgs message_generation sensor_msgs
    INCLUDE_DIRS include# TODO include
    LIBRARIES # TODO
)

include_directories(include ${Boost_INCLUDE_DIR} ${catkin_INCLUDE_DIRS})


#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)

# CATKIN_MIGRATION: removed during catkin migration
# rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME})
add_executable(indirect_kalman_6dof
#  src/main.cpp
  src/node.cpp
  src/INS.cpp src/INSInitializer.cpp
  src/Kalman.cpp src/NavigationComputer.cpp)
target_link_libraries(indirect_kalman_6dof ${catkin_LIBRARIES})
add_dependencies(indirect_kalman_6dof ${catkin_EXPORTED_TARGETS})
add_dependencies(indirect_kalman_6dof ${PROJECT_NAME}_generate_messages_cpp)
set_target_properties(indirect_kalman_6dof PROPERTIES COMPILE_FLAGS -std=c++11)
#target_link_libraries(example ${PROJECT_NAME})
