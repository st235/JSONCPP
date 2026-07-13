cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(response-parser VERSION 1.0 DESCRIPTION "Parses JSON responses from Rick And Morty API.")

add_executable(response-parser
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

target_link_libraries(response-parser jsoncpp)
