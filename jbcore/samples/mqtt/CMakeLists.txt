project(sample_base VERSION 0.1.0 LANGUAGES C CXX)

set(CUR_TARGET JB_SAMPLES_MQTT)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(${CUR_TARGET}_INC
)

set(${CUR_TARGET}_SRC
  main.cpp
)

# 声明测试 sdk 库的代码
add_executable(${PROJECT_NAME} ${JB_SAMPLES_MQTT_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# 链接 jbcore 库
target_link_libraries(${PROJECT_NAME} PRIVATE jbcore)

# 设置安装路径为 bin
install(TARGETS ${PROJECT_NAME} DESTINATION bin)