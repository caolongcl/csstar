
set(CUR_TARGET JB_APP)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(${CUR_TARGET}_INC
)

set(${CUR_TARGET}_SRC
  ${CUR_DIR}/main.cpp
  PARENT_SCOPE
)