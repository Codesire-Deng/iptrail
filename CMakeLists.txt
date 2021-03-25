cmake_minimum_required(VERSION 3.10.0)
project(iptrail VERSION 0.1.0)

# 指定C++标准
set(CMAKE_CXX_STANDARD 17)

# 当 --config DEBUG 时请打开此选项。链接DEBUG版本的库
set(LIB_DEBUG ON)

# define DEBUG
add_definitions(-DDEBUG)    # 启用部分调试信息
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # 禁用MSVC关于fopen的编译警告

# 指定include搜索目录
include_directories(${PROJECT_SOURCE_DIR}/include)

if (WIN32)
    include_directories("C:/boost_1_75_0")
    include_directories("C:/OpenSSL-Win64/include")
    link_directories("C:/boost_1_75_0/stage/lib")
    link_directories("C:/OpenSSL-Win64/lib")
endif()
    
    
# 指定静态/动态库所在目录
# link_directories("C:/boost_1_75_0/libs")

# 执行src/CMakeLists.txt
add_subdirectory(./src)

# include(CTest)
# enable_testing()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
