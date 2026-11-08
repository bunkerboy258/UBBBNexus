# World Drop Item Law

- 本目录遵守 Actor 数据 网络与注释宪法.
- ABBBWorldDropItem 是掉落物的唯一运行时根 只通过模板 SubmitInput 接受外部影响.
- 房主为共享掉落物的唯一业务控制端 负责下落 落地 领取资格与终态 镜像只还原表现.
- 掉落物表示一种型号及整次数量 不转移现存装备内部状态 不自动穿戴.
- 同时只向一个角色交付 角色入包成功后才经 Shutdown 销毁 拒绝后重新开放 不以超时推断入包失败.
- 掉落物身份与递增交付序号区分每次交付 重复与迟到结果不能作用于其它交付.
- 配置与运行时状态分离 物理运动由 UE 持有 业务状态仅由 RuntimeData 持有.
- 光柱与图标只表达已成立的落地状态 图标复用物品定义的 Icon 本地朝向不复制.
- ParseSystem 消费固定类型槽位 DropSystem 维护掉落与交付 NetworkSystem 唯一维护网络语义.
- 不创建 AnimationSystem 不新增物品 Catalog 不依赖角色背包的具体实现.
