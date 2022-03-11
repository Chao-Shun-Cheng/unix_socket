cmake_minimum_required(VERSION 3.0.2)
project(unix_socket)

find_package(catkin REQUIRED COMPONENTS
  autoware_msgs
  autoware_can_msgs
  roscpp
  rospy
  message_generation
  tf
  std_msgs
)

add_message_files(
   FILES
   vehiclestatus.msg
 )

generate_messages(
   DEPENDENCIES
   std_msgs
)

catkin_package(
   CATKIN_DEPENDS autoware_msgs roscpp rospy message_runtime autoware_can_msgs tf std_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(unix_socket_receive
        src/receive.cpp
        src/unix_socket.cpp
        )
target_link_libraries(unix_socket_receive
        ${catkin_LIBRARIES}
        )
add_dependencies(unix_socket_receive
        ${catkin_EXPORTED_TARGETS}
        )

add_executable(unix_socket_send
        src/send.cpp
        src/unix_socket.cpp
)
target_link_libraries(unix_socket_send
        ${catkin_LIBRARIES}
        )
add_dependencies(unix_socket_send
        ${catkin_EXPORTED_TARGETS}
        )

install(TARGETS
        unix_socket_receive unix_socket_send
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