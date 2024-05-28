工作流程
=========


1 时钟驱动注册流程
-----------------------

时钟在内核是一个大数组 ``struct clk *clk[]``, 其中数组下标就是时钟的ID, 
驱动在初始化时通过clk_register注册 ``gate``, ``mux``, ``divider`` 等六大类时钟
硬件, 最后调用provider注册接口, 将整个时钟大数组提供给内核管理

================================================ ====================================
kernel/drivers/clk/imx/clk-imx8mp.c              时钟文件关于clk大数组的初始化
kernel/include/dt-bindings/clock/imx8mp-clock.h  时钟ID定义
================================================ ====================================

.. code-block::

    ############################################# 1 设备树部分
    # 1.1 时钟节点配置, 通过of_clk_get_by_name(np, "osc_24m")解析
    osc_24m: clock-osc-24m {
        compatible = "fixed-clock";
        #clock-cells = <0>;
        clock-frequency = <24000000>;
        clock-output-names = "osc_24m";
    };

    # 1.2 时钟节点
    clk: clock-controller@30380000 {
        clocks = <&osc_32k>, <&osc_24m>, <&clk_ext1>, <&clk_ext2>, <&clk_ext3>, <&clk_ext4>;
        clock-names = "osc_32k", "osc_24m", "clk_ext1", "clk_ext2", "clk_ext3", "clk_ext4";
        assigned-clocks = <&clk IMX8MP_CLK_NOC>, <&clk IMX8MP_CLK_NOC_IO>, ...
        assigned-clock-parents = <&clk IMX8MP_SYS_PLL2_1000M>, <&clk IMX8MP_SYS_PLL1_800M>, ...
        assigned-clock-rates = <1000000000>, <800000000>,
    };

    ############################################# 2 驱动部分
    # 2.1 驱动匹配imx8mp_clocks_probe, 各种类型的clock register, 最后注册provider
    imx_clk_mux2 + imx_clk_gate3 + imx_clk_divider2 + imx8m_clk_composite + of_clk_add_provider

    # 2.2 of_clk_add_provider

