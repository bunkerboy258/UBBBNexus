# Wearable Law

- 遵循数据宪法.

## 物理目录

物理目录固定为以下结构

```text
Wearable/
+-- Catalog/
|  +-- BBBWearableCatalog.h / .cpp
|  +-- BBBWearableCatalogEntry.h
+-- Config/
   +-- BBBAppearanceResource.h
```

- `Catalog/` 登记通用物品静态定义与对应外观资源 只提供静态查找 实际穿戴显示由调用者负责.
- `Config/` 只定义静态外观资源结构;物品身份 分类 显示信息与穿戴位置沿用 `Item/BBBItemDefinition.h` 不重复定义.
