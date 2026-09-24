# Character Animation Instance Law

角色基本事实只能由 AnimationSystem 一次性写入 动画蓝图只读
输入系统只可向固定槽位直接注册已经批准的 UAnimMontage 贡献
每个固定槽位只保存一个 UAnimMontage 引用 禁止附加播放倍率 操作序号 玩法标签 修订号 提交标记或 UE 播放状态

## 目录职责
- 本目录无直属子目录 直属文件实现角色动画实例
- 禁止在 `Character/Animation/` 新增直属文件夹或文件
