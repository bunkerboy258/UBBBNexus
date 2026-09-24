# Character Core Law

- 主管线是角色各个系统的唯一调度者 

## 目录职责
- `Initialization/` 存放角色依赖装配与初始值设置逻辑
- `Shutdown/` 存放角色依赖解除与对象销毁逻辑
- `Update/` 存放角色更新主管线
- 禁止在 `Character/Logic/Core/` 新增直属文件夹或文件
