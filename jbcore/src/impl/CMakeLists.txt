#
set(CUR_TARGET CORE_IMPL)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(${CUR_TARGET}_SRC
  ${CUR_DIR}/mqtt.cpp
  ${CUR_DIR}/qrcode.cpp
  PARENT_SCOPE
)