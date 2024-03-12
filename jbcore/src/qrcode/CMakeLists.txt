#
set(CUR_TARGET CORE_QRCODE)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(${CUR_TARGET}_SRC
  ${CUR_DIR}/qrcode_p.cpp
  PARENT_SCOPE
)