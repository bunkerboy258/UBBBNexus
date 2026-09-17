# 角色统一输入迁移

## 入口

通过 ABBBCharacter::GetInput().Submit(Packet) 提交固定类型 入口仅校验和入队 不调用系统

| 类型 | 用途 |
|---|---|
| FBBBCharacterControlInput | 世界空间移动向量 朝向 瞄准目标 持续控制和跳跃边沿 |
| FBBBCharacterEquipmentInput | Equip Fire Reload 离散请求 |
| FBBBEquipmentActionEvent | 装备已执行结果 |
| FBBBCharacterMontagePacket | 已获准操作的蒙太奇贡献 |
| FBBBCharacterReloadAnimationInput | 带操作序号的换弹通知 |
| FBBBPlayerCameraInput | 相机冲量贡献 |
| FBBBCharacterRestoreInput | 完成因果的还原事实 仅网络系统可投递 |

移动向量保留模拟输入强度 持续控制保留最后提交值 外部控制源解绑必须提交释放状态 PlayerInputSystem 已负责控制目标解绑和菜单禁用 一次性装备请求不缓冲重试

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
