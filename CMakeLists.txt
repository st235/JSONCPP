cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(JSONCPP VERSION 0.9.5 DESCRIPTION "JSONCPP is a lightweight implementation of JSON Data Interchange Standard for C++ programming language.")

# Library targets.

add_library(jsoncpp INTERFACE)

target_sources(jsoncpp INTERFACE
  src/json_beautifier.cpp
  src/json_minifier.cpp
  src/json_parser.cpp
  src/json_parser.h
  src/json_tokeniser.h
  src/json.cpp
)
target_include_directories(jsoncpp INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

#  Enable assertions statements in the codebase.
option(ENABLE_ASSERT "Enables assertion mechanism, useful for debugging." OFF)
if (ENABLE_ASSERT)
  message(STATUS "Enabling assertion mechanism.")
  target_compile_definitions(jsoncpp INTERFACE
    ASSERT=${ENABLE_ASSERT}
  )
endif()

# Compile test targets, disabled by default.
option(COMPILE_TESTS "Compiled with tests when turned on." OFF)

if (COMPILE_TESTS)
  message(STATUS "Compiling tests.")

  # Test targets.
  include(FetchContent)

  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )

  # For Windows: Prevent overriding the parent project's compiler/linker settings.
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  add_executable(jsoncpp_tests
    # Utils.
    tests/integration/test_utils.h

    # Unit tests.
    #  Testing types.
    tests/unit/json_null_tests.cpp
    tests/unit/json_boolean_tests.cpp
    tests/unit/json_number_tests.cpp
    tests/unit/json_string_tests.cpp
    tests/unit/json_array_tests.cpp
    tests/unit/json_object_tests.cpp
    #  Internal types tests.
    tests/unit/json_tokeniser_tests.cpp

    # Integration tests.
    tests/integration/json_dump_tests.cpp
    tests/integration/json_malformed_json_tests.cpp
    tests/integration/json_valid_json_tests.cpp
    #  Visitors.
    tests/integration/json_beautifier_tests.cpp
    tests/integration/json_minifier_tests.cpp
  )

  target_link_libraries(jsoncpp_tests PRIVATE jsoncpp)
  target_include_directories(jsoncpp_tests PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

  # GMock is required to use EXPECT_THAT.
  target_link_libraries(jsoncpp_tests PRIVATE GTest::gtest_main GTest::gmock_main)

  # Enable all possible warnings and treat them as errors.
  # This check only affects tests builds, as if applied to
  # an INTERFACE library, will affect clients of the library.
  if(MSVC)
    message(STATUS "Detected MSVC compiler.")
    target_compile_options(jsoncpp_tests PRIVATE /W4 /WX /permissive-)
  else()
    target_compile_options(jsoncpp_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)

    if (CMAKE_CXX_COMPILER_ID MATCHES ".*[Cc]lang")
      message(STATUS "Detected Clang compiler.")
      target_compile_options(jsoncpp_tests PRIVATE -Wconversion -Wno-unused-function -Wconditional-uninitialized)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "Detected GCC compiler.")
        target_compile_options(jsoncpp_tests PRIVATE -Wno-uninitialized)
    endif()
  endif()

  include(GoogleTest)
  gtest_discover_tests(jsoncpp_tests)

endif()
