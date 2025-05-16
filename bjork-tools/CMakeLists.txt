cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(bjork_tool)

include(FetchContent)

FetchContent_Declare(
  curl
  URL https://curl.se/download/curl-8.7.1.tar.gz
)
FetchContent_MakeAvailable(curl)

add_executable(bjork bjork.cpp spinner.cpp system.cpp)
target_link_libraries(bjork PRIVATE CURL::libcurl)
