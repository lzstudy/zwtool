TARGET=main
BUILD_DIR=_build

function build_task()
{
	cmake -S . -B $BUILD_DIR            # 创建编译目录
	make -C $BUILD_DIR                  # 编译
	# pt -b $BUILD_DIR/$TARGET			# 推送
}

build_task
