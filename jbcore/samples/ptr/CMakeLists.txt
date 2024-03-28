project(sample_ptr VERSION 0.1.0 LANGUAGES C CXX)

set(CUR_TARGET JB_SAMPLES_PTR)
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS ">> DSG File Scan << ${CUR_DIR}")

set(INC
)

set(SRC
  ${CUR_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_link_libraries(${PROJECT_NAME} PRIVATE jbcore)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)