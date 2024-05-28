#指定最低版本
cmake_minimum_required(VERSION 3.10)

## 设置子目录和依赖库 - [根据情况修改]
set(subdir common pin led pwm uart i2c)
set(libs pthread)

# 需要编译的库存入此变量
set(hal_libs)

# 设置目标名称
set(target hal)
project(${target})

# 设置编译选项
#set(CMAKE_C_COMPILER "arm-linux-gnueabi-gcc")
set(CMAKE_C_COMPILER ${CC})
set(CMAKE_AR ${AR})

# 设置编译选项
add_compile_options(-std=gnu11 -Wall -g -O1)
add_compile_options(-fPIC)
# add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
#add_compile_options(-Werror)

# 包含目录和添加子文件夹
foreach(dir ${subdir})
    include_directories(${dir})
    add_subdirectory(${dir})
    set(hal_libs ${hal_libs} hf_${dir})
endforeach()

# 添加main文件
aux_source_directory(${PROJECT_SOURCE_DIR}/scripts srcs)

# 生成可执行文件
# add_executable(${target} ${srcs})
add_library(${target} SHARED ${srcs})
target_link_libraries(${target} ${libs} ${hal_libs})
