####################################################
# message("当前变量是：${dir}")
# message(STATUS "BOARD_TARGET=${BOARD_TARGET}")        -   打印变量
# aux_source_directory(${PROJECT_SOURCE_DIR} srcs)    	-	直接添加源文件
# list(APPEND SRC_FILE "./${DEMO_TARGET}/rpmsg_pty.c")  -   变量追加
####################################################

#指定最低版本
cmake_minimum_required(VERSION 3.10)

# 设置目标名称
set(target xxx)
 
#指定工程名称
project(${target})

# 设置编译选项
#set(CMAKE_C_COMPILER "arm-linux-gnueabi-gcc")
set(CMAKE_C_COMPILER ${CC})
set(CMAKE_AR ${AR})

# 设置编译选项
add_compile_options(-std=gnu11 -Wall -g -O0)
#add_compile_options(-Werror)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer)

# 添加库路径
# link_directories(${PROJECT_SOURCE_DIR})

# 添加库到 libs
set(libs pthread)

# 添加头文件路径
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../../lib)
include_directories(${PROJECT_SOURCE_DIR}/../../lib/pin)
include_directories(${PROJECT_SOURCE_DIR}/../../lib/serial)
include_directories(${PROJECT_SOURCE_DIR}/../../motor)

# 添加源文件到 srcs
aux_source_directory(${PROJECT_SOURCE_DIR} srcs)
aux_source_directory(${PROJECT_SOURCE_DIR}/../../lib srcs)
aux_source_directory(${PROJECT_SOURCE_DIR}/../../lib/pin srcs)
aux_source_directory(${PROJECT_SOURCE_DIR}/../../lib/serial srcs)
aux_source_directory(${PROJECT_SOURCE_DIR}/../../motor srcs)

# 生成动态库
add_library(${target} SHARED ${srcs})
target_link_libraries(${target} ${libs})
