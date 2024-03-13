project(jbcore)

set(CUR_TARGET JB_CORE)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(root_DIR ${CUR_DIR}/..)

set(eclipse-paho-mqtt-c_DIR ${root_DIR}/lib/paho_mqtt_c/lib/cmake/eclipse-paho-mqtt-c)
find_package(eclipse-paho-mqtt-c REQUIRED)

set(ZXing_DIR ${root_DIR}/lib/zxingcpp/lib/cmake/ZXing)
find_package(ZXing REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(impl)

set(JB_CORE_SRC
  ${CORE_IMPL_SRC}
)

add_library(${PROJECT_NAME} ${JB_CORE_SRC})
target_link_libraries(${PROJECT_NAME} 
  eclipse-paho-mqtt-c::paho-mqtt3a-static
  ZXing::ZXing Threads::Threads
)

# 链接 jbcore 库时会自动引入库头文件路径
target_include_directories(${PROJECT_NAME}
                          PUBLIC
                            $<BUILD_INTERFACE:${root_DIR}/include>
                            $<INSTALL_INTERFACE:include>
                          PRIVATE
                            $<BUILD_INTERFACE:${root_DIR}/include>
                           )

# 设置版本号
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION "1.0.0")
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION "1")

# 设置要安装的库
set(install_libs ${PROJECT_NAME})
# 将库安装到 lib 目录下
install(TARGETS ${install_libs}
  EXPORT ${PROJECT_NAME}Targets
  DESTINATION lib)
# 安装头文件到 include 目录下
install(FILES 
  ${CORE_INC_INSTALL}
  DESTINATION include)