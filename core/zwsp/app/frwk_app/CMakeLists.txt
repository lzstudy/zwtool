#指定最低版本
cmake_minimum_required(VERSION 3.10)

## 设置子目录和依赖库 - [根据情况修改]
set(libs pthread)
set(subdir common sub1 sub2)

# 设置目标名称
set(target main)
project(${target})

# 设置编译变量
#set(CMAKE_C_COMPILER "arm-linux-gnueabi-gcc")
set(CMAKE_C_COMPILER ${CC})
set(CMAKE_AR ${AR})
set(LINK_SCRIPTS "-T/home/zw/zwapp/app_framework/common/link.lds") 

# 设置编译选项
add_compile_options(-std=gnu11 -Wall -g -O1)
# add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
# add_compile_options(-Werror)

# 包含目录和添加子文件夹
foreach(dir ${subdir})
    include_directories(${dir})
    add_subdirectory(${dir})
    set(libs ${libs} ${dir})
endforeach()

# 添加main文件
aux_source_directory(${PROJECT_SOURCE_DIR} srcs)

# 添加库路径
# link_directories(${PROJECT_SOURCE_DIR})

# 生成可执行文件
add_executable(${target} ${srcs})
target_link_libraries(${target} ${libs} ${LINK_SCRIPTS})
