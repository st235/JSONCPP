cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(json-beautifier VERSION 1.0 DESCRIPTION "Pretty prints any JSON file.")

add_executable(json-beautifier
    main.cpp
)

# Add JSONCPP Dependency

include(FetchContent)

FetchContent_Declare(
  JSONCPP
  GIT_REPOSITORY git@github.com:st235/JSONCPP.git
  GIT_TAG "main"
  GIT_SHALLOW TRUE
  GIT_PROGRESS ON
)
FetchContent_MakeAvailable(JSONCPP)

target_link_libraries(json-beautifier jsoncpp)
