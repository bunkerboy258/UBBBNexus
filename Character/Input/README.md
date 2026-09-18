# 角色统一输入迁移

## 入口

通过 ABBBCharacter::GetInput().Submit(Packet) 提交固定类型 入口仅校验和入队 不调用系统

持续控制由 `FBBBCharacterControlInput` 一次提交并保留最后采样值 切枪 换弹 开火才是独立动作包 对应规则集中在 Behaviors 文件夹 不使用虚函数对象或每帧动态委托

| 输入 | 对应规则 | 提交节奏 | 冲突与黑板效果 |
|---|---|---|---|
| FBBBCharacterControlInput | FBBBCharacterControlRule | 持续快照 跳跃仅一帧 | 写入移动朝向目标 瞄准或开火时禁止冲刺 |
| FBBBEquipInput | FBBBEquipBehavior | 单帧请求 携带 EquipSlot | 有效切换取消换弹并写期望装备 |
| FBBBReloadInput | FBBBReloadBehavior | 单帧请求 | 切换或换弹中受阻 写换弹命令并接管反馈和动画阶段 |
| FBBBFireInput | FBBBFireBehavior | 单帧请求 控制快照也可保持开火 | 切换或换弹中受阻 写开火命令及已获准事实 |

输入入口只校验和收件 不表示行为获准 持续快照保留最后提交值 单帧请求按本帧到达顺序消费 不缓冲失败请求 玩家组件解绑时提交释放状态

管线按切枪 换弹 开火的固定顺序调度 冲突条件由各行为规则判断 切枪规则取消正在进行的换弹 装备成功结果和动画通知属于事件 由 FBBBReloadBehavior 按装备标识和操作序号处理 已获准的蒙太奇与相机输入属于表现贡献 镜像还原只接受网络系统投递的 FBBBCharacterRestoreInput

## 手动资产迁移

1. 在玩家控制器的 PlayerInputSystem 组件配置 Config 中的 InputAction 引用 原 CharacterConfig.Input 已删除 不会自动转移序列化配置
2. 相机参数改放该组件的 CameraConfig 瞄准投射距离改放 AimTargetDistance 角色不再持有 CameraBoom FollowCamera 或玩家输入配置
3. 清除蓝图对旧角色相机组件 ExecuteEquipmentActionMontage PlayMovementActionMontage 和 Dash Slide 的引用 普通装备 开火 换弹均由角色动画系统播放
4. 保留 FullBody UpperBody FullBodyAdditivePreAim UpperBodyAdditive AdditiveHitReact 五个 Slot 前四个保留 DefaultGroup 最后一个保留 AdditiveGroup
5. 装备和换弹蒙太奇的 UpperBody 与 UpperBodyAdditive 双轨无需拆分 共享一次播放
6. 换弹继续使用标准 Montage Notify 名称 BBB.Reload.Start 和 BBB.Reload.End ReportReloadStartNotify 与 ReportReloadEndNotify 仍是通知提交入口 避免同时重复添加自动通知和手工通知

输入动作缺失 无效槽位 播放失败均会产生日志 旧序号不推进新操作 自然播放完成但缺少换弹结束通知时取消操作 不自动补弹 镜像丢弃本地通知 不生成新因果

## 宿主编辑器模块

宿主 Source/ABBB_EvacEditor/Private/BBBPIEInputEditorLibrary.cpp 已改为从控制器查找 UBBBPlayerInputSystem 并读取 Config 删除 Dash Slide 注入项 该文件在内层 Git 外 不包含在内层推送中

## 不包含

不新增装备生成 初始武器 弹匣实体 备用弹药 拾取或新装备玩法 不修改装备内部仲裁与 Fragment 行为 编译和纯规则测试通过不代表资产配置 PIE 表现或联机效果已验证
