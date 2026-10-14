# Equipment Animation Base Law

- 具体装备实现的动画基类 必须继承UBBBEquipmentAnimInstance并声明为final;
- 装备网格创建自己的动画实例 实例之间不共享可变快照
- 动画实例只保存并发布外部整理后的事实
- 动画蓝图与线程安全 getter 只能读取快照 禁止访问外界
