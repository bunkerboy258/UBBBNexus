# Equipment Base Law

- 本目录只定义装备的公共内容 禁止持有具体装备业务

## 物理目录

物理目录固定为以下结构

```text
Base/
    Law.md
    BBBEquipment.h / .cpp                定义装备公共基类 公共组件与固定入口
    Config/                              定义公共静态配置
    Animation/                           定义公共动画实例基类与基本事实
    Input/                               声明实际共用的固定输入
    Network/                             定义公共网络组件基类
    Logic/                               定义公共生命周期骨架
        Core/
            Initialization/
                BBBEquipmentInitializer.h / .cpp    定义公共初始化基类
            Update/
                BBBEquipmentUpdatePipeline.h / .cpp 定义公共更新管线基类
            Shutdown/
                BBBEquipmentShutdown.h / .cpp       定义公共关闭基类
```

## 具体装备实现要求

具体装备的以下 C++ 类必须继承 Base 对应基类并声明 final

- 装备 Actor 类
- 配置类
- 动画实例类
- 网络组件类
- 初始化类
- 更新管线类
- 关闭类(BBBXShutdown)
