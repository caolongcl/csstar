#
set(CUR_TARGET CORE_INC)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(${CUR_TARGET}
  ${CUR_DIR}/result.hpp
  ${CUR_DIR}/mqtt.hpp
  ${CUR_DIR}/def.hpp
  ${CUR_DIR}/mqtt_p.hpp
  PARENT_SCOPE
)

set(${CUR_TARGET}_INSTALL
  ${CUR_DIR}/result.hpp
  ${CUR_DIR}/mqtt.hpp
  ${CUR_DIR}/def.hpp
  PARENT_SCOPE
)