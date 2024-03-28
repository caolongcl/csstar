project(sample_mqtt VERSION 0.1.0 LANGUAGES C CXX)

set(CUR_TARGET JB_SAMPLES_MQTT)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(${CUR_TARGET}_INC
)

set(${CUR_TARGET}_SRC
  main.cpp
)

add_executable(${PROJECT_NAME} ${JB_SAMPLES_MQTT_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_link_libraries(${PROJECT_NAME} PRIVATE jbcore)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)