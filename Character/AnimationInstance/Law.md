# Character Animation Instance Law

动画实例基类提供的是“角色的基本事实和状态”
由且只能由角色animationsystem一次性写入数据
动话蓝图只读本实例的暴露成员
只保存动画图读取事实快照 固定槽位蒙太奇执行状态
禁止提供二次加工的事实
