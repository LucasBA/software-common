# Catkin User Guide: http://www.ros.org/doc/groovy/api/catkin/html/user_guide/user_guide.html
# Catkin CMake Standard: http://www.ros.org/doc/groovy/api/catkin/html/user_guide/standards.html
cmake_minimum_required(VERSION 2.8.3)
project(uf_common)
# Load catkin and all dependencies required for this package
# TODO: remove all from COMPONENTS that are not catkin packages.
find_package(catkin REQUIRED COMPONENTS message_generation message_runtime geometry_msgs tf actionlib interactive_markers std_msgs actionlib_msgs cmake_modules)
catkin_python_setup()

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
#set(ROS_BUILD_TYPE RelWithDebInfo)


# CATKIN_MIGRATION: removed during catkin migration
# rosbuild_find_ros_package(actionlib_msgs)

# CATKIN_MIGRATION: removed during catkin migration
# include(${actionlib_msgs_PACKAGE_PATH}/cmake/actionbuild.cmake)
add_action_files(
  FILES
  MoveTo.action
)


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
  PoseTwistStamped.msg
  x.msg
  PoseTwist.msg
  VelocityMeasurements.msg
  Float64Stamped.msg
  VelocityMeasurement.msg
  Acceleration.msg
)
#uncomment if you have defined services
#rosbuild_gensrv()

find_package(Eigen REQUIRED)
include_directories(${Eigen_INCLUDE_DIRS})
## Generate added messages and services with any dependencies listed here
generate_messages(
    DEPENDENCIES std_msgs actionlib_msgs geometry_msgs uf_common
)

# catkin_package parameters: http://ros.org/doc/groovy/api/catkin/html/dev_guide/generated_cmake_api.html#catkin-package
# TODO: fill in what other packages will need to use this package
catkin_package(
    DEPENDS Eigen # TODO
    CATKIN_DEPENDS message_generation message_runtime geometry_msgs tf actionlib interactive_markers std_msgs actionlib_msgs
    INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} include# TODO include
    LIBRARIES # TODO
)

include_directories(${EIGEN_INCLUDE_DIRS} include ${Boost_INCLUDE_DIR} ${catkin_INCLUDE_DIRS})


#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(example examples/example.cpp)
#target_link_libraries(example ${PROJECT_NAME})

install(PROGRAMS scripts/interactive_marker_moveto scripts/velocitymeasurements_to_vector3 scripts/param_saver DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
