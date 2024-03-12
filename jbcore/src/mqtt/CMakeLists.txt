#
set(CUR_TARGET CORE_MQTT)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(${CUR_TARGET}_SRC
  ${CUR_DIR}/mqtt_p.cpp
  PARENT_SCOPE
)