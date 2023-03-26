cmake_minimum_required(VERSION 3.22)
project(kerntool
    VERSION 1.0
    DESCRIPTION "Kernel Pwning Library"
    LANGUAGES CXX
)
set(CXX_STANDARD_REQUIRED 17)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_library(kt kernel.cc)