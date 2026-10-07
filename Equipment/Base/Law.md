# Equipment Base Law

- 本目录只定义装备的公共内容 禁止持有具体装备业务

## 物理目录

物理目录固定为以下结构

```text
Base/
    Law.md
    BBBEquipment.h / .cpp                定义装备公共基类 定义公共组件与固定入口
    Config/                              定义公共静态配置 专属配置放在 Instance/<类型>/Config/
    Animation/                           定义公共动画实例基类与基本事实 专属动画事实放在 Instance/<类型>/Animation/
    Input/                               声明实际共用的固定输入 专属输入包放在 Instance/<类型>/Input/
        LocalControl/
            <领域>/
                <输入包>.h
        RemoteMessage/
            <领域>/
                <输入包>.h
        AuthorityFact/
            <领域>/
                <输入包>.h
    Logic/                               定义共有初始化逻辑
        Core/
            Initialization/
                BBBEquipmentInitializer.h / .cpp
```
