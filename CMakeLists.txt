cmake_minimum_required(VERSION 3.0.2)
project(unix_socket)

find_package(catkin REQUIRED COMPONENTS
  autoware_msgs
  roscpp
  rospy
)

catkin_package(
   CATKIN_DEPENDS autoware_msgs roscpp rospy
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(unix_socket
        src/main.cpp
        src/unix_socket.cpp
        )
target_link_libraries(unix_socket
        ${catkin_LIBRARIES}
        )
add_dependencies(unix_socket
        ${catkin_EXPORTED_TARGETS}
        )

install(TARGETS
        unix_socket
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch/
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
        PATTERN ".svn" EXCLUDE)

if (CATKIN_ENABLE_TESTING)
  roslint_add_test()
endif()