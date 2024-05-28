工作流程
===========

1 获取并使能时钟流程
----------------------

.. code-block::

    ############################################# 1 设备树部分
    # 1.1 配置clocks节点, 其中CLK_I2C_ROOT是时钟唯一ID, 由厂商定义, 每个时钟都会有唯一一个ID
    clocks = <&clk CLK_I2C_ROOT>

    # 1.2 配置clock-names节点, 与clocks节点配合使用, 驱动中通过此标识获取到clocks
    clock-names = "zw-i2c-clk"

    ############################################# 2 驱动部分
    #include <linux/clk.h>

    # 2.1 解析设备树时钟, 获取clk句柄
    struct clk * devm_clk_get(dev, "zw-i2c-clk");

    # 2.2 设置时钟频率
    clk_set_rate(clk, 120000);

    # 3.3 使能时钟
    clk_prepare_enable(clk);


2 系统时钟初始化流程
-----------------------

时钟在内核是 ``struct clk *clks[]`` 一个大数组, 其中数组下标就是时钟的ID,