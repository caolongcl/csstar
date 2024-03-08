#
set(CUR_TARGET CORE_API)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(${CUR_TARGET}_INC
  ${CUR_DIR}/result.hpp
  ${CUR_DIR}/mqtt.hpp
  PARENT_SCOPE)

set(${CUR_TARGET}_SRC
  PARENT_SCOPE
)