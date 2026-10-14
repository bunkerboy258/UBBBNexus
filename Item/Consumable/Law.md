# Consumable Law

- 遵循 `../Law.md` 与数据宪法、Actor 宪法和网络宪法。

## 物理目录

物理目录固定为以下结构

```text
Consumable/
├─ Law.md
├─ Catalog/
├─ Base/
└─ Instance/
```

- 消耗品在独立的 `Base/` 定义公共持有组件、配置和输入，`Instance/X/` 定义具体业务，不复用装备 `Base/`
- 消耗品不创建自身 `Animation/` 与 `AnimationSystem/`
