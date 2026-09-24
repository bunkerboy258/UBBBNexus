# 角色固定输入

所有角色输入通过 ABBBCharacter::SubmitInput 提交 每种包一个固定槽 同类只保留最新数据

目录只有两级分类

- Local/Control 保存连续移动与瞄准意图
- Local/Action 保存跑步 蹲伏和跳跃的独立请求
- Local/Camera 保存通用相机冲量
- Mirror/State 保存瞄准与跑步状态还原
- Shared/Animation 保存五个固定 Slot 的独立蒙太奇包

包通过 IsValid CanApply Apply 声明自身校验 条件与效果 源码顺序决定消费顺序 不使用动态消息表或队列

五个蒙太奇包直接携带 UAnimMontage 引用 空引用清除对应 Slot 不携带装备来源 倍率 动作标记或网络版本

角色输入不包含装备选择 开火 换弹 弹药及装备事实 玩家输入适配层把这些操作交给装备持有系统或当前装备

新增包只需新增具体类型 固定槽 编译期映射与显式消费位置 不创建模板占位文件
