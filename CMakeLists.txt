cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(gazebo_geospatial_plugin)

find_package(ignition-plugin1 REQUIRED COMPONENTS all)
find_package(ignition-common4 REQUIRED)
find_package(ignition-math6 REQUIRED)
find_package(ignition-rendering6 REQUIRED)
find_package(jsoncpp REQUIRED)

add_library(gazebo_geospatial_plugin SHARED gazebo_geospatial_plugin.cc)

target_link_libraries(gazebo_geospatial_plugin
  PRIVATE
    ignition-plugin1
    ignition-common4
    ignition-math6
    ignition-rendering6
    jsoncpp)
