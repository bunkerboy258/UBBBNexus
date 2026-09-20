# Equipment Template Input Law

- 本目录声明所有装备共享的完整输入协议和固定槽位布局。
- 固定解析顺序为 Fact、Equip、Secondary、DetachMagazine、LoadMagazine、InterruptReload、Reload、Primary。
- 每个槽位只保存激活标记、最新输入数据和 `bIsMirror`，禁止动态容器和跨帧积压。
- 新增公共输入必须同时更新固定状态、运行时调度和全部装备签名，禁止为单件武器增加公共输入。
- `FBBBEquipmentCommand` 只作为角色到装备演员的动态信封，进入具体装备后必须立即映射为静态输入。
