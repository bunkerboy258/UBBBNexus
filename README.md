# UBBBNexus

2026-10-18 手枪左手握持与步枪统一 默认使用 LeftHand 插槽 通过既有姿态快照与共用 FABRIK 发布和应用 hand_r 空间目标 四把手枪已保存网格插槽及配置 补齐装备与换弹 DisableLHandIK 曲线 UE5.8 编译通过 初始握持位置按现有 ADS 姿态换算 尚未进行视觉验收

2026-10-18 装备配置统一物品资料 装配与握持 角色动画 武器动画 开火与装填 持枪表现 镜头后坐力及专属机制分类 使用原生 PrioritizeCategories 与 DisplayPriority 控制排版 不改变字段类型 默认值或资产配置 UE5.8 编译通过 31 份装备资产保存内容保持不变 界面效果由用户验收

Unreal Engine 5.8 的角色与装备运行时源码 宿主为 E:/BBB_Evac

## 职责

- Character 保存通用人形控制与装备持有关系 不实现武器行为
- PlayerInput 将移动瞄准交给角色 将开火换弹直接交给当前装备
- PlayerCamera 负责跟随与通用相机输入
- Equipment/Catalog 保存装备定义检索
- Equipment/Base 保存公共装备组件 配置与网络边界
- Equipment/Instance/Rifle 保存步枪的 Parse Action Animation Network 四系统

角色与装备输入均按网络类别和输入类型两级分类 不保留第三层分类
角色蒙太奇一 Slot 一包 每个槽只持有一个蒙太奇引用

## 网络

可信 P2P 合作联机 本机立即执行 房主分发结果 远端还原当前状态
角色与装备分别拥有自己的同步职责 位置和移动继续使用角色移动组件
装备网络组件借用持有角色连接 不把武器事实交给角色处理
不引入预测 回滚 事件历史或额外的通用消息框架

## 验证边界

本次以 ABBB_EvacEditor Win64 Development 编译通过为验收
不修改编辑器资产 不保留旧接口兼容层 不声称完成 PIE 或联机运行验证
资产后续适配事项见 Equipment/README.md

2026-10-16：Actor 输入只贡献固定槽位并返回 void，校验与许可由 Parse 执行。攀爬领域维护 bTraversing，角色交互发送 Hide／Show；装备自行维护 bHide 与显示结果，隐藏不关闭 Tick、不解绑动画，不重播拿枪。

2026-10-17 消耗品战斗更新:消耗品改为仅服务当次使用的 UObject 装备主手与消耗品使用目标独立 快捷栏固定两个投掷位置和三个恢复位置 F 发起当前装备姿态的快速近战;G 短按高抛 长按后左键高抛、右键低抛 保持所选鼠标键维持姿势 松键释放 网格显示、释放、恢复、近战窗口和结束均由各自通知负责 角色同步使用事实与结果 数量仍由 ItemSystem 维护 血药使用 2.8 秒 在 2.1 秒恢复最大生命的 30% 满血拒绝使用 编译及 12 项 UE 战斗回归通过 双端和第三端迟加入通过;物理对象容量测试不冒充 16 客户端验收

## 武器右腕握持校准

装备定义的 `RightHandGripRotationOffset` 通过动画事实快照发布给 `UBBBAnimInstance`，以线程安全只读 getter 提供给基础动画层。实际节点在父骨骼空间对动作右腕叠加旋转，零值关闭该校准；左右手指静态握姿和各自抑制曲线保持原职责。传统枪托武器据此保留自然右腕，型号挂接与左手目标使用相同基准反补，动作 AS 不重复烘焙该增量。

角色固定站姿预览使用 `Animation/UsedAnimation/unarmed/MM_Unarmed_Idle_Ready` 的唯一正式路径。制作与原生节点数据核验不等于玩家最终运行观感通过。

## 手部层封装

主动画图的HandPose_OverrideState只保留姿势输入输出 握姿与两侧权重都在Base内部取得。GetMainAnimationCurveValue显式读取主实例曲线 与GetCharacterGripPose使用同一主实例来源 不增加角色事实写入入口。

2026-10-19 经用户批准恢复完整 FABRIK 握持链路：装备发布 hand_r 空间目标位置与旋转，恢复 LeftHandIKRotation 和 RightHandGripRotationOffset 的配置、快照与只读 getter。静态握姿层仅覆盖手指。UE5.8 完整编译通过；PIE 由用户验收。
