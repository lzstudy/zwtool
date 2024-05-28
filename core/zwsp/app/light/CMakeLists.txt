#指定最低版本
cmake_minimum_required(VERSION 3.10)

# 设置目标名称
set(target light)
 
#指定工程名称
project(${target})

# 设置编译选项
#set(CMAKE_C_COMPILER "arm-linux-gnueabi-gcc")
set(CMAKE_C_COMPILER ${CC})
set(CMAKE_AR ${AR})

# 设置编译选项
add_compile_options(-std=gnu11 -Wall -g -O1)
#add_compile_options(-Werror)

# 添加库路径
# link_directories(${PROJECT_SOURCE_DIR})

# 添加库到 libs
set(libs pthread)

# 添加头文件路径
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../framework)
include_directories(${PROJECT_SOURCE_DIR}/../framework/pwm)
include_directories(${PROJECT_SOURCE_DIR}/../framework/pin)

# 添加源文件到 srcs
aux_source_directory(${PROJECT_SOURCE_DIR} srcs)
aux_source_directory(${PROJECT_SOURCE_DIR}/../framework/ srcs)
aux_source_directory(${PROJECT_SOURCE_DIR}/../framework/pwm srcs)
aux_source_directory(${PROJECT_SOURCE_DIR}/../framework/pin srcs)

# 生成动态库
# add_library(${target} SHARED ${srcs})

# 生成可执行文件
add_executable(${target} ${srcs})

# 设置链接库
target_link_libraries(${target} ${libs})
