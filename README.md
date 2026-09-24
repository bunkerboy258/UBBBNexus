# UBBBNexus

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
