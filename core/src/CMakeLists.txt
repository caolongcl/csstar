project(cscore)

set(CUR_TARGET CS_CORE)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> CS File Scan << ${CUR_DIR}")

add_subdirectory(mqtt)

set(CS_CORE_INC
  ${CORE_MQTT_INC}
)

set(CS_CORE_SRC
  ${CORE_MQTT_SRC}
)

# 声明要编译的库，依赖源文件 ${src}
add_library(${PROJECT_NAME} ${CS_CORE_SRC})

# 链接 sdk 库时会自动引入库头文件路径
target_include_directories(${PROJECT_NAME}
                           INTERFACE
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mqtt>
                            $<INSTALL_INTERFACE:include>
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
install(FILES ${CS_CORE_INC} DESTINATION include)