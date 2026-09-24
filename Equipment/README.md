# Equipment

Catalog 物理位置不变 Base 保存实际复用的公共装备边界 Instance/Rifle 保存步枪实现

ABBBRifleEquipment 是唯一运行时根 四个系统按 Parse Action Animation Network 顺序执行

- Parse 检查来源并维护固定输入槽
- Action 消费输入 维护弹药和换弹结果 本机开火才调用 EmitShot
- Animation 计算握持目标 发布 UBBBRifleAnimInstance 快照并贡献表现
- Network 编解码当前结果 借持有角色的装备网络组件传输

RuntimeData 只持有实际有跨帧状态的 Parse Action Animation 领域 B 每个 B 直接私有持有 A 并逐个提供 const 读取方法
Network 系统没有额外黑板 复制快照和接收进度由网络组件独占

本机立即执行 房主分发结果 远端只还原当前状态 不回放装备事实
传输边界包含装备标识 切换代次 状态版本和具体装备序列化内容
代次区分切走后再切回 版本排除同代旧状态 空标识同步空手
尚未创建镜像实例时只保留当前最新状态 不建立历史队列

输入按 Local 或 Mirror 或 Shared 再按 Action 或 State 两级分类 类型目录直接保存具体包
装备只读角色黑板的网络身份 不接受外部附加的镜像参数

旧子弹类已删除 Rifle 只保留 EmitShot 枪口世界变换入口 默认不生成弹丸

## 本轮验证边界

只完成代码与 Law 重构以及 UE5.8 Editor Development 编译 不修改或验证编辑器资产
后续资产适配需要将步枪动画蓝图继承到 UBBBRifleAnimInstance 并替换角色侧已删除的换弹与开火查询节点
旧子弹蓝图父类和已删除的子弹配置引用也需要清理 编译成功不代表这些资产已经可运行
