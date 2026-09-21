# Equipment Base Definition Law

- 配置数据不是运行时黑板，不参与 `Constitution/Data/Law.md` 的 A/B/C/D 分层。

- 本目录只定义所有装备共享的静态配置基类和只读表现事实。
- 基础配置只能包含所有装备都成立的身份、网格、动画类和角色动画层数据。
- 具体武器资源、数值和行为参数必须放入对应 `Instance/<Weapon>/Definition`。
- Definition 是配置资产，不得保存运行时状态、角色引用或输入槽位。
