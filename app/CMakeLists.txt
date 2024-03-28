cmake_minimum_required(VERSION 3.0.0)

project(jbapp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(CMAKE_DEBUG_POSTFIX d)

if(CMAKE_BUILD_TYPE)
  message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "Configuring for Debug build")
  elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "Configuring for Release build")
  endif()
endif()

set(jbcore_root ${CMAKE_CURRENT_SOURCE_DIR}/../jbcore)

set(eclipse-paho-mqtt-c_DIR ${jbcore_root}/lib/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}/paho_mqtt_c/lib/cmake/eclipse-paho-mqtt-c)
set(ZXing_DIR ${jbcore_root}/lib/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}/zxingcpp/lib/cmake/ZXing)

# 依赖
find_package(eclipse-paho-mqtt-c REQUIRED)
find_package(ZXing REQUIRED)
find_package(Threads REQUIRED)

set(jbcore_DIR ${jbcore_root}/out/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}/jbcore/lib/cmake/jbcore)
find_package(jbcore REQUIRED)

add_subdirectory(src)

add_executable(${PROJECT_NAME} ${JB_APP_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_link_libraries(${PROJECT_NAME} PRIVATE jbcore)

target_include_directories(${PROJECT_NAME}
PRIVATE 
  $<BUILD_INTERFACE:${jbcore_root}/out/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}/jbcore/include>
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)