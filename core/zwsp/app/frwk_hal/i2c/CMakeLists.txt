# 设置目标名字和依赖库 - [根据需要修改]
set(target hf_i2c)
set(libs )

# 编译本目录下所有.c文件
aux_source_directory(. srcs)

# 生成静态库
add_library(${target} ${srcs})
target_link_libraries(${target} ${libs})