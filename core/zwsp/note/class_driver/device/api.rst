函数接口
==========

1 头文件
---------

#include <linux/clk.h>

2 数据结构
------------


3 函数接口
------------

3.1 获取时钟相关接口
**********************

通过clock名字获取struct clk指针的过程

.. code-block:: c

    # 经常使用
    struct clk *devm_clk_get(struct device *dev, const char *id);

    struct clk *clk_get(struct device *dev, const char *id);

    struct clk *of_clk_get(struct device_node *np, int index);

    struct clk *of_clk_get_by_name(struct device_node *np, const char *name);

    struct clk *of_clk_get_from_provider(struct of_phandle_args *clkspec);

    void clk_put(struct clk *clk);

3.2 控制时钟相关接口
**********************

.. code-block:: c

    # 时钟准备, 且使能
    clk_prepare_enable(struct clk *clk);

    # 时钟准备, 工作在非原子上下文
    int clk_prepare(struct clk *clk);

    void clk_unprepare(struct clk *clk);

    static inline int clk_enable(struct clk *clk);

    static inline void clk_disable(struct clk *clk);

    static inline unsigned long clk_get_rate(struct clk *clk);

    static inline int clk_set_rate(struct clk *clk, unsigned long rate);

    static inline long clk_round_rate(struct clk *clk, unsigned long rate);

    static inline int clk_set_parent(struct clk *clk, struct clk *parent);

    static inline struct clk *clk_get_parent(struct clk *clk);


.. note:: 
    
    名称中含有prepare、unprepare字符串的API是内核后来才加入的, 过去只有clk_enable和clk_disable, 
    只有clk_enable和clk_disable 带来的问题是某些硬件的enable/disable 可能引起睡眠使得
    enable/disable不能在原子上下文进行。 
        
    加上prepare后, 把过去的clk_enable分解成clk_prepare(不可在原子上下文调用的, 该函数可能睡眠)
    和clk_enable(可以在原子上下文调用的)。
    
    clk_prepare_enable则同时完成, 所以也只能在可能睡眠的上下文调用该API