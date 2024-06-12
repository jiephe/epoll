cmake_minimum_required(VERSION 3.10)

project(EPoll)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++0x -fpermissive")

include_directories(
                    ${CMAKE_CURRENT_SOURCE_DIR}/
)

file(GLOB SRC_FILE
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

set(DLIBS
    -lpthread -ldl
)

add_executable(${PROJECT_NAME} ${SRC_FILE})
target_link_libraries(${PROJECT_NAME} ${DLIBS})
