project(jbcore)

set(CUR_TARGET JB_CORE)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(root_DIR ${CUR_DIR}/..)

find_package(eclipse-paho-mqtt-c REQUIRED)
find_package(ZXing REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(impl)

set(JB_CORE_SRC
  ${CORE_IMPL_SRC}
)

add_library(${PROJECT_NAME} ${JB_CORE_SRC})
target_link_libraries(${PROJECT_NAME}
  eclipse-paho-mqtt-c::paho-mqtt3a-static
  ZXing::ZXing
  Threads::Threads
)

target_include_directories(${PROJECT_NAME}
                          PUBLIC
                            $<BUILD_INTERFACE:${root_DIR}/include>
                            $<INSTALL_INTERFACE:include>
                           )

set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION "1.0.0")
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION "1")


set(install_libs ${PROJECT_NAME})

install(TARGETS ${install_libs}
  EXPORT ${PROJECT_NAME}Targets
  DESTINATION lib)

install(FILES 
  ${CORE_INC_INSTALL}
  DESTINATION include)