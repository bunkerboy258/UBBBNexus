# Consumable Instance Law

- 投出后具有运动、碰撞、引信与伤害的玩法实体由具体消耗品的业务系统提交 Mass 出生输入，后续模拟与伤害由 `Mass/Instance/Projectile/` 负责。
- 投出实体的可见网格由 Mass 表现层创建；其它纯表现网格、特效与音效通过表现通知处理，消耗品实例不得直接承担这些表现职责。
- 一种具体的'消耗品类型'应该在此目录下创建对应的实例代码实现

## 物理目录

物理目录固定为以下结构

```text
Instance/
    <消耗品类型>/
        <消耗品Actor根>.h / .cpp          唯一运行时根与对外输入入口
        Config/                          本类消耗品的静态配置
            <配置根>.h
            <领域>/                      子配置文件
        Input/                           本类消耗品的专属输入包
            LocalControl/
                <领域>/
                    <输入包>.h
            RemoteMessage/
                <领域>/
                    <输入包>.h
            AuthorityFact/
                <领域>/
                    <输入包>.h
        Network/                         本类消耗品的 UE 网络桥接组件与协议
        Logic/
            Core/
                Initialization/          本类消耗品的初始化实现
                Update/                  本类消耗品的更新主管线
                Shutdown/                本类消耗品的失活实现
            RuntimeData/                 唯一运行时黑板
            System/
                XxxSystem/
                    XxxSystem.h / .cpp   系统控制根
                    DomainData/
                        XxxDomainState.h 领域状态持有者 B
                        States/          持久状态 A
                        Definitions/     领域自定义数据 O
                        Context/         栈上临时数据 D
                    Processors/          本系统的具体逻辑
```

## 相关规定

- Config/ 直属只放配置根 子配置按领域放在二级目录下 不写 cpp
- 每种消耗品必须有 ParseSystem NetworkSystem 与至少一种业务系统 各系统都使用上述 System/ 内部结构
- 网络组件只负责本类消耗品的 UE 网络桥接 具体消息与复制字段由本类消耗品定义
- 遵守数据宪法
- 遵守Actor宪法
