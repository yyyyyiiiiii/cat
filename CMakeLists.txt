cmake_minimum_required(VERSION 3.30)

project(cat LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(CMAKE_BUILD_TYPE "Build type: [Debug, Release]" Release)

add_compile_options(
  $<$<CONFIG:Release>:$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-O3>>
  $<$<CONFIG:Release>:$<$<CXX_COMPILER_ID:MSVC>:/O2>>
  $<$<CONFIG:Debug>:$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-g>>
  $<$<CONFIG:Debug>:$<$<CXX_COMPILER_ID:MSVC>:/DEBUG>>
)

add_executable(${PROJECT_NAME} main.c options.c)
