########################################################################################################
# 编译应用
########################################################################################################
function build_app()
{
    cd 01_app
    cmake -S . -B _build
    make -C _build
    pt -b _build/main
}

########################################################################################################
# 编译内核模块
########################################################################################################
function build_kmod()
{
    cd 02_kmod
    make
    pt -b *.ko
}

########################################################################################################
# 编译二进制
########################################################################################################
function build_bin()
{
    cd 03_bin
    make 
    pt -b *.bin
}

########################################################################################################
# clean
########################################################################################################
function build_clean()
{
    rm 01_app/_build -rf
    pushd 02_kmod 
    make clean
    popd
    make -C 03_bin clean
}

########################################################################################################
# 编译入口
########################################################################################################
function build_entry()
{
    case $1 in
        "app")
        build_app
        ;;

        "kmod")
        build_kmod
        ;;

        "bin")
        build_bin
        ;;

        "clean")
        build_clean
        ;;
    esac
}

build_entry $@