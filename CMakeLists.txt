cmake_minimum_required(VERSION 2.8)
project( opencv_cpp )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable( remove_horizon remove_horizon.cpp )
target_link_libraries( remove_horizon ${OpenCV_LIBS} )

