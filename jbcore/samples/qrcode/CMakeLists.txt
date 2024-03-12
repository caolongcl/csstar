project(sample_qrcode VERSION 0.1.0 LANGUAGES C CXX)

set(CUR_TARGET JB_SAMPLES_ZXINGCPP)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(INC
  ${CUR_DIR}/stb_image.h
)

set(SRC
  ${CUR_DIR}/main.cpp
)

# 声明测试 sdk 库的代码
add_executable(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# 链接 jbcore 库
target_link_libraries(${PROJECT_NAME} PRIVATE jbcore)

# 设置安装路径为 bin
install(TARGETS ${PROJECT_NAME} DESTINATION bin)