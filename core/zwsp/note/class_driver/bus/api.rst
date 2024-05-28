API接口
========

1 头文件
-----------

#include <linux/clk.h>

2 数据结构
-----------

3 函数接口
-----------


1 provider
------------

1.1 fix rate clock
*******************

这一类clock具有固定频率, 不能开关、调频、选择parent, 是最简单的一类clock

.. code-block:: c

    # fix rate注册接口
    struct clk *clk_register_fixed_rate(struct device *dev, const char *name,
                const char *parent_name, unsigned long flags,
                unsigned long fixed_rate);


1.2 gate clock
****************

这一类clock只能开关, 一般会提供enable/disable接口

.. code-block:: c

    # gate注册接口
    struct clk *clk_register_gate(struct device *dev, const char *name,
                    const char *parent_name, unsigned long flags,
                    void __iomem *reg, u8 bit_idx,
                    u8 clk_gate_flags, spinlock_t *lock);

1.3 divider clock
*******************

用于设置分频, 一般会提供set_rate接口

.. code-block:: c

    # 注册方案1, 注册单个分频器
    struct clk *clk_register_divider(struct device *dev, const char *name,
                    const char *parent_name, unsigned long flags,
                    void __iomem *reg, u8 shift, u8 width,
                    u8 clk_divider_flags, spinlock_t *lock);
                    
    # 注册方案2, 同时注册多个分频器
    struct clk *clk_register_divider_table(struct device *dev, const char *name,
                    const char *parent_name, unsigned long flags,
                    void __iomem *reg, u8 shift, u8 width,
                    u8 clk_divider_flags, const struct clk_div_table *table,
                    spinlock_t *lock);

1.4 mux clock
***************

时钟路径选择器, 一般会提供set_parent接口来选择时钟路径

.. code-block:: c

    # 注册单个mux
    struct clk *clk_register_mux(struct device *dev, const char *name,
                const char **parent_names, u8 num_parents, unsigned long flags,
                void __iomem *reg, u8 shift, u8 width,
                u8 clk_mux_flags, spinlock_t *lock);

    # 注册多个mux                
    struct clk *clk_register_mux_table(struct device *dev, const char *name,
                    const char **parent_names, u8 num_parents, unsigned long flags,
                    void __iomem *reg, u8 shift, u32 mask,
                    u8 clk_mux_flags, u32 *table, spinlock_t *lock);

1.5 fixed factor clock
**************************

此类clock具有固定的factor(multiplier和divider), clock的频率由parent clock * mul / div, 由于parent clock可变,
因此fixed factor clock也可以改变频率

.. code-block:: c

    # 注册fixed factor clock
    struct clk *clk_register_fixed_factor(struct device *dev, const char *name,
                    const char *parent_name, unsigned long flags,
                    unsigned int mult, unsigned int div);


1.6 composite clock
*********************

组合clock, 就是由mux、divider、gate等clock的组合

.. code-block:: c

    # 注册composite clock
    struct clk *clk_register_composite(struct device *dev, const char *name,
                    const char **parent_names, int num_parents,
                    struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
                    struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
                    struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
                    unsigned long flags);


.. note::

    以上注册函数最终都是调用clk_register注册到CCF, 然后将返回的struct clk指针保存到一个数组中



2 consumer
------------

2.1 获取时钟相关接口
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

2.2 控制时钟相关接口
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
