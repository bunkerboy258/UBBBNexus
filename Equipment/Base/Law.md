# Equipment Base Law

- 本目录只定义装备的公共内容 禁止持有具体装备业务

## 物理目录

物理目录固定为以下结构

```text
Base/                                    需要继承的公共基类禁止声明 final
    Law.md
    BBBEquipment.h / .cpp                定义装备公共基类 公共组件与固定入口 具体装备必须提供 final 派生 Actor 类
    Config/                              定义公共静态配置 具体装备必须在 Instance/<类型>/Config/ 提供 final 派生配置类 公共枚举直接使用 不要求继承
    Animation/                           定义公共动画实例基类与基本事实 具体装备必须在 Instance/<类型>/Animation/ 提供 final 派生动画实例类 公共事实数据直接使用 不要求继承
    Input/                               声明实际共用的固定输入 输入包按 Actor 宪法声明 final 并直接使用 禁止继承 专属输入包放在 Instance/<类型>/Input/
    Logic/                               定义公共生命周期骨架 具体装备实现放在自身对应目录下
        Core/
            Initialization/
                BBBEquipmentInitializer.h / .cpp    定义公共初始化基类 具体装备必须提供 final 派生初始化实现
            Update/
                BBBEquipmentUpdatePipeline.h / .cpp 定义公共更新管线基类 具体装备必须提供 final 派生更新管线实现
            Shutdown/
                BBBEquipmentShutdown.h / .cpp       定义公共关闭基类 具体装备必须提供 final 派生关闭实现
```
