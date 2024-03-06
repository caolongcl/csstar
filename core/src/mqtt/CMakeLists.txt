#
set(CUR_TARGET CORE_MQTT)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> CS File Scan << ${CUR_DIR}")

set(${CUR_TARGET}_INC
  ${CUR_DIR}/mqtt.hpp
  PARENT_SCOPE)

set(${CUR_TARGET}_SRC
  ${CUR_DIR}/mqtt.cpp
  PARENT_SCOPE
)