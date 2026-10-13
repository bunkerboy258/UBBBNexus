# EquipInstance Law

- 网格体生成(比如弹夹);特效/音效触发 均属于表现层的内容 装备实例不得用有他们的语义;(本项目通过自定义cpp动画通知并挂在蒙太奇上实现)
- 一种具体的'装备类型'应该在此目录下创建对应的实例代码实现 诸如'刀'和'步枪'/ '步枪'和狙击枪'的混用都是不允许的

## 物理目录

物理目录固定为以下结构

```text
Instance/
    Law.md
    <装备类型>/
        <装备Actor根>.h / .cpp           唯一运行时根与对外输入入口
        Config/                          本类装备的静态配置
            <配置根>.h
            <领域>/                      子配置文件
        Animation/                       本类装备的动画实例与基本事实
        Input/                           本类装备的专属输入包
            LocalControl/
                <领域>/
                    <输入包>.h
            RemoteMessage/
                <领域>/
                    <输入包>.h
            AuthorityFact/
                <领域>/
                    <输入包>.h
        Network/                         本类装备的 UE 网络桥接组件与协议
        Logic/
            Core/
                Initialization/          本类装备的初始化实现
                Update/                  本类装备的更新主管线
                Shutdown/                本类装备的关闭实现
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

## 目录规定

- Config/ 直属只放配置根 子配置按领域放在二级目录下 不写 cpp
- 每种装备必须有 ParseSystem AnimationSystem NetworkSystem 与至少一种业务系统 各系统都使用上述 System/ 内部结构
- 网络组件只负责本类装备的 UE 网络桥接 具体消息与复制字段由本类装备定义
- 遵守数据宪法 
- 遵守Actor宪法

## 继承要求

以下具体 C++ 类必须继承 Base 对应基类并声明 final

- 装备 Actor 类
- 配置类
- 动画实例类
- 网络组件类
- 初始化类
- 更新管线类
- 关闭类
