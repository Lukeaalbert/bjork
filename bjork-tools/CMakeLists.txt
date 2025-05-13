cmake_minimum_required(VERSION 3.14)
project(bjork_tool)

include(FetchContent)

FetchContent_Declare(
  curl
  URL https://curl.se/download/curl-8.7.1.tar.gz
)
FetchContent_MakeAvailable(curl)

add_executable(bjork bjork.cpp)
target_link_libraries(bjork PRIVATE CURL::libcurl)
