# Item Law

- 本目录定义全项目的基础物品与实例实现;例如角色物品类 CharacterItem 是对基础物品的包装.
- 同理 物品的二次包装不能修改/重写基础物品 且命名必须是'XXXItem' XXX为实现范围的代称
- 第一级类别固定为 Equipment/ Wearable/ Consumable/ Misc/ 各类别维护自己的 Catalog/

- 装备和穿戴物最大堆叠为一 消耗品与杂物按定义允许堆叠




- 基础人物身体与无装备内搭属于角色静态配置 不登记为物品
