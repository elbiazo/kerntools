cmake_minimum_required(VERSION 3.22)

project(kerntool
    VERSION 1.0
    DESCRIPTION "Kernel Pwning Library"
    LANGUAGES CXX
)
set(CXX_STANDARD_REQUIRED 17)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.x
)

FetchContent_MakeAvailable(spdlog)


include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_library(kt 
    src/kernel.cc
    src/util.cc
    src/pwn.cc
)

target_link_libraries(kt spdlog::spdlog)
