# Rifle Equipment Law

严格遵守装备上级 Law 与数据宪法

物理目录固定为

- 直属根保存 `ABBBRifleEquipment` 演员实现与本 Law
- `Definition/` 保存步枪静态配置
- `RuntimeData/` 放且只放聚合黑板 C 与本目录 Law
- `DomainData/` 放 B 与本目录 Law
- `DomainData/States/` 放 A
- `DomainData/Context/` 放 D
- `Input/Packet/` 保存每种行为的编译期输入规则
- `Processors/` 保存输入共用的具体表现操作
- `Notify/` 保存将角色动画通知转为步枪输入的适配器

更新要求

- 步枪直接重写 Tick 在 TG_PostUpdateWork 执行
- 装备自身网格是步枪 Tick 的前置 角色侧负责补齐角色与角色网格的前置依赖
- 根负责初始化 输入存储维护 固定解析顺序与最终快照发布
- 具体行为规则只在输入包内定义 根不得复制开火或换弹判定
- 固定顺序为镜像事实 装备 卸下弹匣 装入弹匣 结束换弹 开始换弹 开火
- 普通同类输入只保留最后一次提交 镜像事实按提交顺序使用固定容量存储
- 镜像队列容量耗尽必须报警 禁止静默越界或临时改为无限动态队列
- 本次上下文只由步枪根在栈上构造 不得持久保存
